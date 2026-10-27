#include "btree.h"
#include <stdio.h>
#include <stdlib.h>



// Crear árbol vacio
Btree* crear_btree() {
    Btree* btree = (Btree*)malloc(sizeof(Btree));
    btree->escrituras = 0;
    btree->lecturas = 0;
    btree->cantidad_nodos = 1;
    btree->capacidad_nodos = INCREMENTO_CAPACIDAD;

    btree->nodos = (NodoBtree*)malloc(sizeof(NodoBtree)* btree->capacidad_nodos);

    btree->nodos[0].es_interno = 0; // nodo hoja
    btree->nodos[0].k = 0; // sin elementos
    btree->nodos[0].padding = 0; // siempre vale 0
    return btree;
}


//leer_nodo
// suma 1 a la cantidad de lecturas realizadas y retorna el nodo leido en posicion i
// btree: puntero al arbol
// i: posicion del nodo a leer
// retorna una copia del nodo leido


NodoBtree leer_nodo(Btree* btree, int i){
    btree->lecturas++;
    return btree->nodos[i];
}

//escribir_nodo
// suma 1 a la cantidad de escrituras realizadas y escribe el nodo en posicion i reemplazando el que estaba en esa posiscion
// btree: puntero al arbol
// i : posicion del nodo a escribir
// nodo: nodo a escribir

void escribir_nodo(Btree* btree, int i, NodoBtree nodo){
    btree->escrituras++;
    btree->nodos[i] = nodo;
}

//agregar_nodo
// agrega un nodo al final del arreglo de nodos del arbol
// btree: puntero al arbol
// nodo: nodo a agregar
// retorna la posicion en la que se agrego el nodo


int agregar_nodo(Btree* btree, NodoBtree nodo){
    
    if(btree->cantidad_nodos == btree->capacidad_nodos){
        btree->capacidad_nodos += INCREMENTO_CAPACIDAD;
        btree->nodos = realloc(btree->nodos, btree->capacidad_nodos * (sizeof(NodoBtree)));
    }

    int posicion = btree->cantidad_nodos;
    btree->nodos[posicion] = nodo;

    btree->escrituras += 1;
    btree->cantidad_nodos += 1;

    return posicion;
}

/// SPLIT seccion 3.4.1////

// a) Seleccionar el par llave-valor en la posición 𝑏/2 (contando desde 1). Este será nuestro par mediano.
// b) Guardar los 𝑏/2 − 1 pares a la izquierda del mediano en un nuevo nodo izquierdo, y los 𝑏/2 pares a la derecha del mediano en un nuevo nodo derecho.
// c) Si el nodo es interno, guardar los primeros 𝑏/2 hijos en el nuevo nodo izquierdo, y los otros 𝑏/2 + 1 en el nuevo nodo derecho.
// d) Retornar los nuevos nodos y el par mediano.


//split
// recibe un nodo lleno y lo divide en 2 nuevo nodos
// nodo_lleno: nodo a dividir
// izq: nuevo nodo a la izquierda
// der: nuevo nodo a la derecha
ParLlaveValor split(NodoBtree nodo_lleno, NodoBtree* izq, NodoBtree* der){

    int mediano = B/2 -1;

    izq->es_interno = nodo_lleno.es_interno;
    izq->k = B/2-1;
    izq->padding = 0;

    der->es_interno = nodo_lleno.es_interno;
    der->k = B/2;
    der->padding = 0;


    for(int i = 0; i < mediano ; i++){
        izq->llaves_valores[i] = nodo_lleno.llaves_valores[i];
    }

    for(int j = 0; j < der->k ; j++){
        der->llaves_valores[j] = nodo_lleno.llaves_valores[j + (mediano + 1)];
    }

    if(nodo_lleno.es_interno){
        for (int h_i = 0; h_i < mediano + 1 ; h_i++){
            izq->hijos[h_i] = nodo_lleno.hijos[h_i];
        }
        for (int h_d = 0; h_d < der->k + 1; h_d++){
            der->hijos[h_d] = nodo_lleno.hijos[h_d + (mediano + 1)];
        }
        
    }
    
    return nodo_lleno.llaves_valores[mediano];
}
