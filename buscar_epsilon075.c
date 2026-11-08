
#define C 126
#define lmax 384

#include "epsilonBtree/epsilonBtree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: %s archivo.bin l u salida.dat\n", argv[0]);
        return 1;
    }

    char *nombre = argv[1];
    int l = atoi(argv[2]);
    int u = atoi(argv[3]);
    char *salida = argv[4];

    FILE *archivo = fopen(nombre, "rb");

    if (archivo == NULL) {
        printf("Error abriendo %s\n", nombre);
        return 1;
    }

    fseek(archivo, 0, SEEK_END);
    long bytes = ftell(archivo);
    rewind(archivo);

    if (bytes <= 0 || bytes % sizeof(NodoBepsTree) != 0) {
        printf("Archivo invalido\n");
        fclose(archivo);
        return 1;
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
        return 1;
    }

    if (fread(t->nodos, sizeof(NodoBepsTree),
              t->cantidad_nodos, archivo) !=
              (size_t)t->cantidad_nodos) {
        printf("Error leyendo arbol\n");
        fclose(archivo);
        liberar_bepsTree(t);
        return 1;
    }

    fclose(archivo);

    for (int i = 0; i <= t->nodos[0].k_llaves; i++) {
        int hijo = t->nodos[0].hijos[i];

        if (hijo < 0 || hijo >= t->cantidad_nodos) {
            printf("Hijo invalido en posicion %d: %d\n", i, hijo);
        }
    }

    ParLlaveValor *resultados = NULL;

    printf("Iniciando busqueda [%d, %d]\n", l, u);

    int cantidad = buscar_rango(t, l, u, &resultados);

    printf("Busqueda terminada: %d\n", cantidad);

    if (cantidad < 0) {
        printf("Error buscando rango\n");
        free(resultados);
        liberar_bepsTree(t);
        return 1;
    }

    FILE *out = fopen(salida, "w");

    if (out == NULL) {
        printf("Error creando salida\n");
        free(resultados);
        liberar_bepsTree(t);
        return 1;
    }

    fprintf(out, "%d\n", cantidad);

    for (int i = 0; i < cantidad; i++) {
        fprintf(out, "%d %.9g\n",
                resultados[i].llave,
                resultados[i].valor);
    }

    fclose(out);
    free(resultados);
    liberar_bepsTree(t);

    printf("Resultados guardados en %s\n", salida);

    return 0;
}
