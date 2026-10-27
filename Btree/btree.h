#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>

#define B 340 // se puede cambiar para probar arboles chicos!!
#define INCREMENTO_CAPACIDAD 1024

// Par llave-valor:
//  - llave: int (4 bytes), tiempo Unix en el que se toma la medición
//  - valor: float (4 bytes), temperatura medida en ese instante
typedef struct{
    int llave;
    float valor;
} ParLlaveValor;

// Nodo del árbol B, tamaño fijo de 4096 bytes (con B = 340)
typedef struct{
    int es_interno; // 1 si es interno, 0 si es hoja
    int k; // cantidad de pares llave-valor actualmente en el nodo
    ParLlaveValor llaves_valores[B]; // pares llave-valor, SIEMPRE en orden ascendenteq
    int hijos[B + 1]; // posiciones en el arreglo de los hijos del nodo
    int padding; // siempre 0, para que el nodo mida 4096 bytes
} NodoBtree;

// Árbol B: arreglo de nodos en RAM que simula el disco.
// La raíz siempre está en nodos[0].
typedef struct{
    NodoBtree* nodos; // arreglo de nodos en RAM
    int cantidad_nodos; // cantidad de nodos actualmente en el arreglo
    int capacidad_nodos; // capacidad del arreglo de nodos
    long long lecturas; // cantidad de lecturas realizadas
    long long escrituras; // cantidad de escrituras realizadas
} Btree;

Btree* crear_btree();
NodoBtree leer_nodo(Btree* btree, int i);
void escribir_nodo(Btree* btree, int i, NodoBtree nodo);
int agregar_nodo(Btree* btree, NodoBtree nodo);
ParLlaveValor split(NodoBtree nodo_lleno, NodoBtree* izq, NodoBtree* der);

#endif