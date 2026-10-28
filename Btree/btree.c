#include "btree.h"
#include <stdio.h>
#include <stdlib.h>



// crear_btree
// crea un árbol B vacío: pide memoria para INCREMENTO_CAPACIDAD nodos y deja
// en la posición 0 una raíz que es una hoja sin pares.
// los contadores de lecturas y escrituras parten en 0 (crear la raíz no se cuenta como escritura).
// retorna: puntero al árbol creado (se debe liberar con liberar_btree)
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

    btree->escrituras++;
    btree->cantidad_nodos++;

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
//retorna el par mediano
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
// inserta un par en el nodo, manteniendo los pares ordenados por llave.
// el nodo no debe estar lleno.
// nodo: nodo donde se inserta
// par: par a insertar
void insertar_en_nodo(NodoBtree* nodo, ParLlaveValor par){

    int i = nodo->k - 1;

    while( i >= 0 && nodo->llaves_valores[i].llave > par.llave ){
        nodo->llaves_valores[i+1] = nodo->llaves_valores[i];
        i--;
    }

    nodo->llaves_valores[i+1] = par;
    nodo->k += 1;

}

// buscar_hijo
// busca a qué hijo de un nodo interno hay que bajar para insertar una llave.
// nodo: nodo interno
// llave: llave que se quiere insertar
// retorna: índice del hijo (posición en el arreglo hijos del nodo)
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

// insertar_rec
// inserta un par bajando recursivamente desde un nodo hasta una hoja.
// si el hijo al que hay que bajar está lleno, lo divide antes de entrar (split preventivo).
// btree: puntero al árbol
// pos: posición del nodo en el arreglo
// nodo: copia del nodo, ya leída
// par: par a insertar
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
// insertar
// inserta un par en el árbol, partiendo desde la raíz.
// si la raíz está llena, la divide y crea una nueva raíz en la posición 0.
// btree: puntero al árbol
// par: par a insertar
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

// guardar_btree
// escribe todos los nodos del árbol, en orden, en un archivo binario.
// el nodo de la posición i queda en el byte i * sizeof(NodoBtree) del archivo.
// btree: puntero al árbol
// nombre_archivo: nombre del archivo donde se guarda
void guardar_btree(Btree* btree, const char* nombre_archivo){

    FILE* archivo = fopen(nombre_archivo, "wb");
    fwrite(btree->nodos, sizeof(NodoBtree), btree->cantidad_nodos, archivo);
    fclose(archivo);
}

// leer_nodo_archivo
// lee el nodo de la posición i desde el archivo binario (cuenta 1 lectura).
// archivo: archivo binario abierto para lectura
// i: posición del nodo
// lecturas: contador de lecturas, se le suma 1
// retorna: el nodo leído
NodoBtree leer_nodo_archivo(FILE* archivo, int i, long long* lecturas){
    NodoBtree nodo;
    fseek(archivo, i*sizeof(NodoBtree), SEEK_SET);
    fread(&nodo, sizeof(NodoBtree), 1, archivo);

    (*lecturas)++;
    return nodo;
}
// debo_bajar
// revisa si el hijo i de un nodo interno puede tener llaves dentro del rango [l, u].
// considera llaves repetidas: el hijo i puede tener llaves iguales a las de sus extremos.
// nodo: nodo interno
// i: índice del hijo
// l: inicio del rango
// u: fin del rango
// retorna: 1 si hay que bajar al hijo, 0 si no
int debo_bajar(NodoBtree nodo, int i, int l, int u){
    int izq = (i == 0) || (nodo.llaves_valores[i-1].llave <= u);
    int der = (i == nodo.k) || (nodo.llaves_valores[i].llave >= l);
    return izq && der;
}

// agregar_resultado
// agrega un par al final del resultado de una búsqueda, agrandando el arreglo si está lleno.
// res: puntero al resultado
// par: par a agregar
void agregar_resultado(Resultado* res, ParLlaveValor par){
    if(res->cantidad == res -> capacidad){
        res->capacidad += INCREMENTO_CAPACIDAD;
        res->pares = realloc(res->pares, res->capacidad*(sizeof(ParLlaveValor)));
    }

    int posicion = res->cantidad;
    res->pares[posicion] = par;
    res->cantidad++;
}


// buscar_rec
// busca recursivamente los pares con llave en [l, u], partiendo desde un nodo del archivo.
// revisa los pares del nodo (sea hoja o interno) y baja a los hijos que pueden tener llaves en el rango.
// archivo: archivo binario abierto para lectura
// pos: posición del nodo en el archivo
// l: inicio del rango
// u: fin del rango
// res: resultado donde se agregan los pares encontrados
// lecturas: contador de lecturas
void buscar_rec(FILE* archivo, int pos, int l, int u, Resultado* res, long long* lecturas){
    NodoBtree nodo = leer_nodo_archivo(archivo, pos, lecturas);

    for(int i = 0; i <= nodo.k -1; i++){
        if(l <= nodo.llaves_valores[i].llave && nodo.llaves_valores[i].llave <= u){
            agregar_resultado(res, nodo.llaves_valores[i]);
        }
    }

    if(nodo.es_interno){
        for(int i = 0; i <= nodo.k; i++){
            if(debo_bajar(nodo, i, l, u)){
                buscar_rec(archivo, nodo.hijos[i], l, u, res, lecturas);
            }
        }
    }
}


// buscar_rango
// busca todos los pares con llave en [l, u], leyendo el árbol desde un archivo binario.
// nombre_archivo: archivo donde se guardó el árbol (con guardar_btree)
// l: inicio del rango
// u: fin del rango
// lecturas: contador de lecturas, se le suma la cantidad de nodos leídos
// retorna: los pares encontrados (se deben liberar con liberar_resultado)
Resultado buscar_rango(const char* nombre_archivo, int l, int u, long long* lecturas){
    FILE* archivo = fopen(nombre_archivo,"rb");
    Resultado res;
    res.cantidad = 0;
    res.capacidad = 0;
    res.pares = NULL;

    buscar_rec(archivo, 0, l, u, &res, lecturas);
    fclose(archivo);

    return res;

}

// liberar_btree
// libera toda la memoria del árbol (el arreglo de nodos y el struct).
// btree: puntero al árbol
void liberar_btree(Btree* btree){
    free(btree->nodos);
    free(btree);
}

// liberar_resultado
// libera el arreglo de pares de un resultado de búsqueda.
// res: puntero al resultado
void liberar_resultado(Resultado* res){
    free(res->pares);
}
