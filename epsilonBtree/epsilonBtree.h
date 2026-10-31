#ifndef EPSILON_BTREE_H
#define EPSILON_BTREE_H

#include <stdio.h>
#include <stdlib.h>

// Configuracion: se elije al compilar
#ifndef C
#define C 4 // maxima cantidad de llaves divisorias
#endif
#ifndef lmax
#define lmax 506 // maxima cantidad de pares en el buffer
#endif
#define INCREMENTO_CAPACIDAD 1024

// Par llave-valor:
//  - llave: int (4 bytes), tiempo Unix en el que se toma la medición
//  - valor: float (4 bytes), temperatura medida en ese instante
typedef struct{
    int llave;
    float valor;
} ParLlaveValor;

// Nodo del árbol B epsilon, tamaño fijo de 4096 bytes
typedef struct{
    int es_interno; // 1 si es interno, 0 si es hoja
    int k_llaves; // cantidad de llaves actualmente en el nodo. Si el nodo es una hoja, este valor es 0
    int k_buffer; // cantidad de pares llave-valor actualmente en el buffer
    int llaves[C]; // las primeras k_llaves posiciones contienen las llaves divisorias ordenadas ascendentemente que separan los rangos de los hijos.
    int hijos[C + 1]; // posiciones en disco de los hijos del nodo interno
    ParLlaveValor buffer[lmax]; // buffer de pares llave-valor
} NodoBepsTree;

// Árbol B epsilon: arreglo de nodos en RAM que simula el disco.
// La raíz siempre está en nodos[0].
typedef struct{
    NodoBepsTree* nodos; // arreglo de nodos en RAM
    int cantidad_nodos; // cantidad de nodos actualmente en el arreglo
    int capacidad_nodos; // capacidad del arreglo de nodos
    long long lecturas; // cantidad de lecturas realizadas
    long long escrituras; // cantidad de escrituras realizadas
} BepsTree;

typedef struct{
    NodoBepsTree izq;
    NodoBepsTree der;
    int llave;
} Resultado;

// Enunciado
BepsTree* crear_bepsTree();
void liberar_bepsTree(BepsTree* t);
NodoBepsTree leer_nodo(BepsTree* t, int i);
void escribir_nodo(BepsTree* t, int i, NodoBepsTree nodo);
int agregar_nodo(BepsTree* t, NodoBepsTree nodo);
Resultado split_buffer_hoja(NodoBepsTree nodo_lleno);
Resultado split_nodo_interno(NodoBepsTree nodo_lleno);
int vaciar_buffer_nodo_interno(BepsTree* t, NodoBepsTree* V, double alpha);
void insertar_bepsTree(BepsTree* t, int llave, float valor, double alpha);
 
#endif