
#include "Btree/btree.h"

#define ParLlaveValor ParLlaveValorEpsilon
#define Resultado ResultadoEpsilon
#define buscar_rango buscar_rango_epsilon
#define leer_nodo leer_nodo_epsilon
#define escribir_nodo escribir_nodo_epsilon
#define agregar_nodo agregar_nodo_epsilon

#include "epsilonBtree/epsilonBtree.h"

#undef ParLlaveValor
#undef Resultado
#undef buscar_rango
#undef leer_nodo
#undef escribir_nodo
#undef agregar_nodo

#include <stdio.h>
#include <stdlib.h>

#define N_PEQUENO 32768
#define N_GRANDE 67108864

int comparar_btree(const void *a, const void *b) {
    ParLlaveValor *p1 = (ParLlaveValor *)a;
    ParLlaveValor *p2 = (ParLlaveValor *)b;

    if (p1->llave < p2->llave) return -1;
    if (p1->llave > p2->llave) return 1;
    if (p1->valor < p2->valor) return -1;
    if (p1->valor > p2->valor) return 1;
    return 0;
}

int comparar_epsilon(const void *a, const void *b) {
    ParLlaveValorEpsilon *p1 = (ParLlaveValorEpsilon *)a;
    ParLlaveValorEpsilon *p2 = (ParLlaveValorEpsilon *)b;

    if (p1->llave < p2->llave) return -1;
    if (p1->llave > p2->llave) return 1;
    if (p1->valor < p2->valor) return -1;
    if (p1->valor > p2->valor) return 1;
    return 0;
}

BepsTree *cargar_epsilon(char *nombre) {
    FILE *archivo = fopen(nombre, "rb");

    if (archivo == NULL) {
        printf("Error al abrir %s\n", nombre);
        return NULL;
    }

    fseek(archivo, 0, SEEK_END);
    long bytes = ftell(archivo);
    rewind(archivo);

    if (bytes <= 0 || bytes % sizeof(NodoBepsTree) != 0) {
        printf("Archivo invalido: %s\n", nombre);
        fclose(archivo);
        return NULL;
    }

    BepsTree *t = crear_bepsTree();

    free(t->nodos);

    t->cantidad_nodos = bytes / sizeof(NodoBepsTree);
    t->capacidad_nodos = t->cantidad_nodos;
    t->nodos = malloc(bytes);

    if (t->nodos == NULL) {
        printf("Error reservando memoria\n");
        fclose(archivo);
        liberar_bepsTree(t);
        return NULL;
    }

    if (fread(t->nodos, sizeof(NodoBepsTree),
              t->cantidad_nodos, archivo) != (size_t)t->cantidad_nodos) {
        printf("Error leyendo %s\n", nombre);
        fclose(archivo);
        liberar_bepsTree(t);
        return NULL;
    }

    t->lecturas = 0;
    t->escrituras = 0;

    fclose(archivo);
    return t;
}

int leer_epsilon075(char *nombre,
                    ParLlaveValorEpsilon **pares) {
    FILE *archivo = fopen(nombre, "r");

    if (archivo == NULL) {
        return -1;
    }

    int cantidad;

    if (fscanf(archivo, "%d", &cantidad) != 1 || cantidad < 0) {
        fclose(archivo);
        return -1;
    }

    *pares = NULL;

    if (cantidad > 0) {
        *pares = malloc(cantidad * sizeof(ParLlaveValorEpsilon));

        if (*pares == NULL) {
            fclose(archivo);
            return -1;
        }
    }

    for (int i = 0; i < cantidad; i++) {
        if (fscanf(archivo, "%d %f",
                   &(*pares)[i].llave,
                   &(*pares)[i].valor) != 2) {
            free(*pares);
            *pares = NULL;
            fclose(archivo);
            return -1;
        }
    }

    fclose(archivo);
    return cantidad;
}

int guardar_btree_datos(char *nombre, Resultado *res) {
    FILE *archivo = fopen(nombre, "w");

    if (archivo == NULL) {
        printf("Error creando %s\n", nombre);
        return 0;
    }

    if (res->cantidad > 1) {
        qsort(res->pares, res->cantidad, sizeof(ParLlaveValor),
              comparar_btree);
    }

    for (int i = 0; i < res->cantidad; i++) {
        fprintf(archivo, "%d %.6f\n",
                res->pares[i].llave, res->pares[i].valor);
    }

    fclose(archivo);
    return 1;
}

int guardar_epsilon_datos(char *nombre,
                          ParLlaveValorEpsilon *pares,
                          int cantidad) {
    FILE *archivo = fopen(nombre, "w");

    if (archivo == NULL) {
        printf("Error creando %s\n", nombre);
        return 0;
    }

    if (cantidad > 1) {
        qsort(pares, cantidad, sizeof(ParLlaveValorEpsilon),
              comparar_epsilon);
    }

    for (int i = 0; i < cantidad; i++) {
        fprintf(archivo, "%d %.6f\n",
                pares[i].llave, pares[i].valor);
    }

    fclose(archivo);
    return 1;
}

int iguales_btree_epsilon(Resultado *btree,
                          ParLlaveValorEpsilon *epsilon,
                          int cantidad_epsilon) {
    if (btree->cantidad != cantidad_epsilon) {
        return 0;
    }

    for (int i = 0; i < btree->cantidad; i++) {
        if (btree->pares[i].llave != epsilon[i].llave ||
            btree->pares[i].valor != epsilon[i].valor) {
            return 0;
        }
    }

    return 1;
}

int iguales_epsilon(ParLlaveValorEpsilon *a, int cantidad_a,
                    ParLlaveValorEpsilon *b, int cantidad_b) {
    if (cantidad_a != cantidad_b) {
        return 0;
    }

    for (int i = 0; i < cantidad_a; i++) {
        if (a[i].llave != b[i].llave ||
            a[i].valor != b[i].valor) {
            return 0;
        }
    }

    return 1;
}

int procesar_tamano(int N, int l, int u) {
    int exponente = N == N_PEQUENO ? 15 : 26;

    char nombre_btree[100];
    char nombre_025[100];
    char nombre_075[100];

    sprintf(nombre_btree, "btree_%d.bin", N);
    sprintf(nombre_025, "epsilon_%d_25.bin", exponente);
    sprintf(nombre_075, "epsilon_%d_75.bin", exponente);

    char archivo_btree[100];
    char archivo_025[100];
    char archivo_075[100];

    sprintf(archivo_btree, "temperaturas_btree_%d.dat", N);
    sprintf(archivo_025, "temperaturas_eps025_%d.dat", N);
    sprintf(archivo_075, "temperaturas_eps075_%d.dat", N);

    printf("\nProcesando N = %d\n", N);

    long long lecturas = 0;
    Resultado rb = buscar_rango(nombre_btree, l, u, &lecturas);

    printf("B-tree: %d resultados\n", rb.cantidad);

    BepsTree *t25 = cargar_epsilon(nombre_025);

    if (t25 == NULL) {
        free(rb.pares);
        return 0;
    }

    ParLlaveValorEpsilon *r25 = NULL;
    int cantidad25 = buscar_rango_epsilon(t25, l, u, &r25);

    if (cantidad25 < 0) {
        printf("Error buscando en epsilon 0.25\n");
        free(rb.pares);
        liberar_bepsTree(t25);
        return 0;
    }

    printf("Bε (0.25): %d resultados\n", cantidad25);

    char comando[300];

    sprintf(comando,
            "./buscar_epsilon075 %s %d %d epsilon075_resultados.dat",
            nombre_075, l, u);

    if (system(comando) != 0) {
        printf("Error ejecutando busqueda epsilon 0.75\n");
        free(rb.pares);
        free(r25);
        liberar_bepsTree(t25);
        return 0;
    }

    ParLlaveValorEpsilon *r75 = NULL;
    int cantidad75 = leer_epsilon075("epsilon075_resultados.dat", &r75);

    if (cantidad75 < 0) {
        printf("Error leyendo resultados epsilon 0.75\n");
        free(rb.pares);
        free(r25);
        liberar_bepsTree(t25);
        return 0;
    }

    printf("Bε (0.75): %d resultados\n", cantidad75);

    if (rb.cantidad > 1) {
        qsort(rb.pares, rb.cantidad, sizeof(ParLlaveValor),
              comparar_btree);
    }

    if (cantidad25 > 1) {
        qsort(r25, cantidad25, sizeof(ParLlaveValorEpsilon),
              comparar_epsilon);
    }

    if (cantidad75 > 1) {
        qsort(r75, cantidad75, sizeof(ParLlaveValorEpsilon),
              comparar_epsilon);
    }


    for (int i = 0; i < cantidad75; i++) {
        if (rb.pares[i].llave != r75[i].llave) {
            printf("Primera llave diferente en posicion %d:\n", i);
            printf("B-tree: %d\n", rb.pares[i].llave);
            printf("Bε 0.75: %d\n", r75[i].llave);
            break;
        }

        if (rb.pares[i].valor != r75[i].valor) {
            printf("Temperatura diferente en llave %d:\n", rb.pares[i].llave);
            printf("B-tree: %.9g\n", rb.pares[i].valor);
            printf("Bε 0.75: %.9g\n", r75[i].valor);
            break;
        }
    }

    printf("\nComparacion de resultados:\n");

    printf("B-tree vs Bε (0.25): %s\n",
           iguales_btree_epsilon(&rb, r25, cantidad25)
           ? "CORRECTO" : "DIFERENTES");

    printf("B-tree vs Bε (0.75): %s\n",
           iguales_btree_epsilon(&rb, r75, cantidad75)
           ? "CORRECTO" : "DIFERENTES");

    printf("Bε (0.25) vs Bε (0.75): %s\n",
           iguales_epsilon(r25, cantidad25, r75, cantidad75)
           ? "CORRECTO" : "DIFERENTES");

    int guardado = guardar_btree_datos(archivo_btree, &rb);
    guardado = guardar_epsilon_datos(archivo_025, r25, cantidad25) && guardado;
    guardado = guardar_epsilon_datos(archivo_075, r75, cantidad75) && guardado;

    free(rb.pares);
    free(r25);
    free(r75);

    liberar_bepsTree(t25);

    return guardado;
}

int main(void) {
    FILE *rangos = fopen("rangos.csv", "r");

    if (rangos == NULL) {
        printf("No se pudo abrir rangos.csv\n");
        return 1;
    }

    int l, u;

    if (fscanf(rangos, "%d,%d", &l, &u) != 2) {
        printf("Error leyendo el primer rango\n");
        fclose(rangos);
        return 1;
    }

    fclose(rangos);

    printf("Rango utilizado: [%d, %d]\n", l, u);

    if (!procesar_tamano(N_PEQUENO, l, u)) {
        return 1;
    }

    if (!procesar_tamano(N_GRANDE, l, u)) {
        return 1;
    }

    FILE *gp = fopen("temperaturas.gp", "w");

    if (gp == NULL) {
        printf("Error creando temperaturas.gp\n");
        return 1;
    }

    fprintf(gp,
        "set terminal pngcairo size 1400,800\n"
        "set grid\n"
        "set xdata time\n"
        "set timefmt '%%s'\n"
        "set format x '%%Y-%%m-%%d'\n"
        "set xlabel 'Fecha de medicion'\n"
        "set ylabel 'Temperatura'\n"
        "set key left top\n"

        "set output 'temperaturas_N32768.png'\n"
        "set title 'Temperaturas recuperadas para N = 32768'\n"
        "plot 'temperaturas_btree_32768.dat' using 1:2 with linespoints title 'B-tree', \\\n"
        "     'temperaturas_eps025_32768.dat' using 1:2 with linespoints title 'Bε (0.25)', \\\n"
        "     'temperaturas_eps075_32768.dat' using 1:2 with linespoints title 'Bε (0.75)'\n"

        "set output 'temperaturas_N67108864.png'\n"
        "set title 'Temperaturas recuperadas para N = 67108864'\n"
        "plot 'temperaturas_btree_67108864.dat' using 1:2 with linespoints title 'B-tree', \\\n"
        "     'temperaturas_eps025_67108864.dat' using 1:2 with linespoints title 'Bε (0.25)', \\\n"
        "     'temperaturas_eps075_67108864.dat' using 1:2 with linespoints title 'Bε (0.75)'\n"
    );

    fclose(gp);

    if (system("gnuplot temperaturas.gp") != 0) {
        printf("Error ejecutando gnuplot\n");
        return 1;
    }

    printf("\nGraficos de temperaturas generados correctamente.\n");

    return 0;
}
