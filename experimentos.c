
#include <stdio.h>
#include <time.h>
#include "Btree/btree.h"

int main() {
    FILE* datos = fopen("datos.bin", "rb");

    if (datos == NULL) {
        printf("Error al abrir datos.bin\n");
        return 1;
    }

    FILE* resultados = fopen("resultados_btree.csv", "w");

    if (resultados == NULL) {
        printf("Error al crear resultados_btree.csv\n");
        fclose(datos);
        return 1;
    }

    fprintf(resultados, "N,tiempo,lecturas,escrituras,nodos,tamano_bytes\n");

    for (int exp = 15; exp <= 26; exp++) {
        int N = 1 << exp;

        rewind(datos);

        Btree* btree = crear_btree();

        if (btree == NULL) {
            printf("Error al crear el arbol\n");
            break;
        }

        ParLlaveValor par;

        clock_t inicio = clock();

        for (int i = 0; i < N; i++) {
            if (fread(&par, sizeof(ParLlaveValor), 1, datos) != 1) {
                printf("Error al leer los datos\n");
                break;
            }

            insertar(btree, par);
        }

        clock_t fin = clock();

        double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

        long long tamano = (long long)btree->cantidad_nodos * sizeof(NodoBtree);

        char nombre[50];
        snprintf(nombre, sizeof(nombre), "btree_%d.bin", N);

        guardar_btree(btree, nombre);

        fprintf(resultados, "%d,%.6f,%lld,%lld,%d,%lld\n",
                N,
                tiempo,
                btree->lecturas,
                btree->escrituras,
                btree->cantidad_nodos,
                tamano);

        fflush(resultados);

        printf("N = %d terminado\n", N);
        printf("Tiempo: %.4f segundos\n", tiempo);
        printf("Lecturas: %lld\n", btree->lecturas);
        printf("Escrituras: %lld\n", btree->escrituras);
        printf("Nodos: %d\n\n", btree->cantidad_nodos);

        liberar_btree(btree);
    }

    fclose(datos);
    fclose(resultados);

    printf("Experimentos terminados.\n");

    return 0;
}
