#ifndef BTREE_H
#define BTREE_H
#include <stdio.h>
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

void insertar_en_nodo(NodoBtree* nodo, ParLlaveValor par);
int buscar_hijo(NodoBtree nodo, int llave);
void insertar_en_padre(NodoBtree* padre, int i, ParLlaveValor mediano, int pos_der);
void insertar_rec(Btree* btree, int pos, NodoBtree nodo, ParLlaveValor par);
void insertar(Btree* btree, ParLlaveValor par);
//guardar en archivo binario
void guardar_btree(Btree* btree, const char* nombre_archivo);
NodoBtree leer_nodo_archivo(FILE* archivo, int i, long long* lecturas);


//Busqueda por rango

int debo_bajar(NodoBtree nodo, int i, int l, int u);

// Resultado
// guarda los pares encontrados en una búsqueda por rango.
// el arreglo pares crece solo a medida que se agregan pares (con agregar_resultado).
// se debe liberar con liberar_resultado.
typedef struct{
    ParLlaveValor* pares; // pares encontrados
    int cantidad; // cuántos pares hay
    int capacidad; // cuántos pares caben antes de agrandar el arreglo
} Resultado;

void agregar_resultado(Resultado* res, ParLlaveValor par);
void buscar_rec(FILE* archivo, int pos, int l, int u, Resultado* res, long long* lecturas);
Resultado buscar_rango(const char* nombre_archivo, int l, int u, long long* lecturas);

void liberar_btree(Btree* btree);
void liberar_resultados(Resultado* res);
#endif