
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "epsilonBtree/epsilonBtree.h"

#define CANT_RANGOS 50
#define SEMANA 604800

#define MIN_L 1546300800
#define MAX_L 1754006400

int main(int argc, char* argv[]) {

    if (argc != 4) {
        printf("Uso: %s <datos.bin> <alpha> <resultados.csv>\n", argv[0]);
        return 1;
    }

    FILE* archivo = fopen(argv[1], "rb");

    if (archivo == NULL) {
        printf("Error al abrir datos.bin\n");
        return 1;
    }

    double alpha = atof(argv[2]);

    FILE* csv = fopen(argv[3], "w");

    if (csv == NULL) {
        printf("Error al crear CSV\n");
        fclose(archivo);
        return 1;
    }

    // Generar los rangos una sola vez.
    int rangos_l[CANT_RANGOS];
    int rangos_u[CANT_RANGOS];

    FILE* rangos = fopen("rangos.csv", "r");

    if (rangos == NULL) {
        rangos = fopen("rangos.csv", "w");

        if (rangos == NULL) {
            printf("Error al crear rangos.csv\n");
            fclose(archivo);
            fclose(csv);
            return 1;
        }

        srand(12345);

        for (int i = 0; i < CANT_RANGOS; i++) {
            rangos_l[i] = MIN_L +
                rand() % (MAX_L - MIN_L + 1);

            rangos_u[i] = rangos_l[i] + SEMANA;

            fprintf(rangos, "%d,%d\n", rangos_l[i], rangos_u[i]);
        }

        fclose(rangos);
    } else {
        for (int i = 0; i < CANT_RANGOS; i++) {
            if (fscanf(rangos, "%d,%d",
                       &rangos_l[i], &rangos_u[i]) != 2) {
                printf("Error leyendo rangos.csv\n");
                fclose(rangos);
                fclose(archivo);
                fclose(csv);
                return 1;
            }
        }

        fclose(rangos);
    }

    fprintf(csv,
        "N,tiempo_insercion,lecturas_insercion,"
        "escrituras_insercion,nodos,tamano_bytes,"
        "tiempo_promedio_busqueda,"
        "lecturas_promedio_busqueda,"
        "escrituras_promedio_busqueda\n"
    );

    for (int i = 15; i <= 26; i++) {

        int N = 1 << i;

        BepsTree* t = crear_bepsTree();

        if (t == NULL) {
            printf("Error al crear arbol\n");
            fclose(archivo);
            fclose(csv);
            return 1;
        }

        rewind(archivo);

        int llave;
        float valor;

        // Insercion.
        clock_t inicio = clock();

        for (int j = 0; j < N; j++) {

            if (fread(&llave, sizeof(int), 1, archivo) != 1 ||
                fread(&valor, sizeof(float), 1, archivo) != 1) {

                printf("Error leyendo datos en N = %d\n", N);

                liberar_bepsTree(t);
                fclose(archivo);
                fclose(csv);
                return 1;
            }

            insertar_bepsTree(t, llave, valor, alpha);
        }

        clock_t fin = clock();

        double tiempo_insercion =
            (double)(fin - inicio) / CLOCKS_PER_SEC;

        long long lecturas_insercion = t->lecturas;
        long long escrituras_insercion = t->escrituras;

        long long tamano =
            (long long)t->cantidad_nodos * sizeof(NodoBepsTree);

        // Guardar el arbol.
        char nombre[100];

        sprintf(nombre, "epsilon_%d_%d.bin",
                i, (int)(alpha * 100));

        FILE* salida = fopen(nombre, "wb");

        if (salida == NULL) {
            printf("Error al guardar %s\n", nombre);

            liberar_bepsTree(t);
            fclose(archivo);
            fclose(csv);
            return 1;
        }

        size_t escritos = fwrite(
            t->nodos,
            sizeof(NodoBepsTree),
            t->cantidad_nodos,
            salida
        );

        fclose(salida);

        if (escritos != (size_t)t->cantidad_nodos) {
            printf("Error al guardar el arbol N = %d\n", N);

            liberar_bepsTree(t);
            fclose(archivo);
            fclose(csv);
            return 1;
        }

        // Reiniciar contadores para medir solo las busquedas.
        t->lecturas = 0;
        t->escrituras = 0;

        long long lecturas_totales = 0;
        long long escrituras_totales = 0;
        double tiempo_total = 0;

        // Realizar las 50 busquedas.
        for (int j = 0; j < CANT_RANGOS; j++) {

            ParLlaveValor* resultados = NULL;

            long long lecturas_antes = t->lecturas;
            long long escrituras_antes = t->escrituras;

            clock_t inicio_busqueda = clock();

            int cantidad = buscar_rango(
                t, rangos_l[j], rangos_u[j], &resultados
            );

            clock_t fin_busqueda = clock();

            if (cantidad < 0) {
                printf("Error en busqueda %d para N = %d\n", j + 1, N);

                free(resultados);
                liberar_bepsTree(t);
                fclose(archivo);
                fclose(csv);
                return 1;
            }

            double tiempo_busqueda =
                (double)(fin_busqueda - inicio_busqueda) /
                CLOCKS_PER_SEC;

            tiempo_total += tiempo_busqueda;

            lecturas_totales += t->lecturas - lecturas_antes;
            escrituras_totales += t->escrituras - escrituras_antes;

            free(resultados);
        }

        double tiempo_promedio = tiempo_total / CANT_RANGOS;

        double lecturas_promedio =
            (double)lecturas_totales / CANT_RANGOS;

        double escrituras_promedio =
            (double)escrituras_totales / CANT_RANGOS;

        // Guardar resultados en el CSV.
        fprintf(csv,
            "%d,%.6f,%lld,%lld,%d,%lld,%.6f,%.2f,%.2f\n",
            N,
            tiempo_insercion,
            lecturas_insercion,
            escrituras_insercion,
            t->cantidad_nodos,
            tamano,
            tiempo_promedio,
            lecturas_promedio,
            escrituras_promedio
        );

       

        liberar_bepsTree(t);
    }

    fclose(archivo);
    fclose(csv);

    printf("Experimentos terminados.\n");

    return 0;
}
