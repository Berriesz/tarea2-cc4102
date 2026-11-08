
#include "Btree/btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CANT_RANGOS 50

int main() {
    FILE *rangos = fopen("rangos.csv", "r");

    if (rangos == NULL) {
        printf("No se pudo abrir rangos.csv\n");
        return 1;
    }

    int l[CANT_RANGOS];
    int u[CANT_RANGOS];

    for (int i = 0; i < CANT_RANGOS; i++) {
        if (fscanf(rangos, "%d,%d", &l[i], &u[i]) != 2) {
            printf("Error leyendo rangos.csv\n");
            fclose(rangos);
            return 1;
        }
    }

    fclose(rangos);

    FILE *salida = fopen("busquedas_btree.csv", "w");

    if (salida == NULL) {
        printf("No se pudo crear el CSV\n");
        return 1;
    }

    fprintf(salida, "N,tiempo_promedio_busqueda,lecturas_promedio_busqueda,escrituras_promedio_busqueda\n");

    for (int i = 15; i <= 26; i++) {
        int N = 1 << i;

        char nombre[100];
        sprintf(nombre, "btree_%d.bin", N);

        long long lecturas_totales = 0;
        double tiempo_total = 0;

        for (int j = 0; j < CANT_RANGOS; j++) {
            long long lecturas = 0;

            clock_t inicio = clock();

            Resultado res = buscar_rango(nombre, l[j], u[j], &lecturas);

            clock_t fin = clock();

            tiempo_total += (double)(fin - inicio) / CLOCKS_PER_SEC;
            lecturas_totales += lecturas;

            liberar_resultado(&res);
        }

        double tiempo_promedio = tiempo_total / CANT_RANGOS;
        double lecturas_promedio = (double)lecturas_totales / CANT_RANGOS;

        fprintf(salida, "%d,%.9f,%.2f,0\n",
                N, tiempo_promedio, lecturas_promedio);

        printf("N = %d terminado\n", N);
        printf("Tiempo promedio: %.9f segundos\n", tiempo_promedio);
        printf("Lecturas promedio: %.2f\n\n", lecturas_promedio);
    }

    fclose(salida);

    return 0;
}
