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

/// Incersión de un par en nodo 3.4.2 ///

// insertar_en_nodo

void insertar_en_nodo(NodoBtree* nodo, ParLlaveValor par){

    int i = nodo->k - 1;

    while( i >= 0 && nodo->llaves_valores[i].llave > par.llave ){
        nodo->llaves_valores[i+1] = nodo->llaves_valores[i];
        i--;
    }

    nodo->llaves_valores[i+1] = par;
    nodo->k += 1;

}

// buscar hijo
// Recibe el nodo interno y la llave a insertar, y devuelve el índice del hijo al que hay que bajar.

int buscar_hijo(NodoBtree nodo, int llave){

    int i = 0;

    while( i < nodo.k && nodo.llaves_valores[i].llave < llave ){
        i++;
    }

    return i;


}
// insertar_en_padre: inserta en el padre el mediano despues del split
// padre: el nodo interno que recibe el mediano.
// i: el índice que dio buscar_hijo, ahi queda el mediano
// mediano: el par que devolvió split, debe subir al padre
// pos_der: el estante donde quedó el nodo derecho (lo que devolvió agregar_nodo).
void insertar_en_padre(NodoBtree* padre, int i, ParLlaveValor mediano, int pos_der){

    
    for(int j = padre->k -1 ; j >= i ; j--){
        padre->llaves_valores[j+1] = padre->llaves_valores[j];
    }

    padre->llaves_valores[i] = mediano;

    for(int j = padre->k; j >= i +1; j--){
        padre->hijos[j+1] = padre->hijos[j];
    }

    padre->hijos[i + 1] = pos_der;
    padre->k++;

}


void insertar_rec(Btree* btree, int pos, NodoBtree nodo, ParLlaveValor par){

    if(!nodo.es_interno){
        insertar_en_nodo(&nodo, par);
        escribir_nodo(btree, pos, nodo);
        return;
    }

    int i = buscar_hijo(nodo, par.llave);
    int pos_hijo = nodo.hijos[i];
    NodoBtree hijo = leer_nodo(btree, pos_hijo);

    if(hijo.k != B){
        insertar_rec(btree, pos_hijo, hijo, par);
        return;
    }

    NodoBtree izq, der;
    ParLlaveValor mediano = split(hijo, &izq, &der);
    escribir_nodo(btree, pos_hijo, izq);
    int pos_der = agregar_nodo(btree, der);
    insertar_en_padre(&nodo, i , mediano, pos_der);
    escribir_nodo(btree, pos, nodo);

    if(par.llave <= mediano.llave){
        insertar_rec(btree, pos_hijo, izq, par);
    }else{
        insertar_rec(btree, pos_der, der, par);
    }
    
}

void insertar(Btree* btree, ParLlaveValor par){

    NodoBtree raiz = leer_nodo(btree, 0);

    if(raiz.k != B){
        insertar_rec(btree, 0, raiz, par);
        return;
    }

    NodoBtree izq, der;
    ParLlaveValor mediano = split(raiz, &izq, &der);
    int pos_izq = agregar_nodo(btree, izq);
    int pos_der = agregar_nodo(btree, der);

    NodoBtree nueva_raiz;

    nueva_raiz.es_interno = 1;
    nueva_raiz.k = 1;
    nueva_raiz.padding = 0;
    nueva_raiz.llaves_valores[0] = mediano;
    nueva_raiz.hijos[0] = pos_izq;
    nueva_raiz.hijos[1] = pos_der;
    escribir_nodo(btree, 0, nueva_raiz);

    if(par.llave <= mediano.llave){
        insertar_rec(btree, pos_izq, izq, par);
    }else{
        insertar_rec(btree, pos_der, der, par);
    }

}