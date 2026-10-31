#include "epsilonBtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// crear_bepsTree
// crea un árbol B epsilon vacío: pide memoria para INCREMENTO_CAPACIDAD nodos y deja
// en la posición 0 una raíz que es una hoja sin pares.
// los contadores de lecturas y escrituras parten en 0 (crear la raíz no se cuenta como escritura).
// retorna: puntero al árbol creado (se debe liberar con liberar_bepsTree)
BepsTree* crear_bepsTree() {
    BepsTree* bepsTree = (BepsTree*)malloc(sizeof(BepsTree));
    bepsTree->escrituras = 0;
    bepsTree->lecturas = 0;
    bepsTree->cantidad_nodos = 1;
    bepsTree->capacidad_nodos = INCREMENTO_CAPACIDAD;

    bepsTree->nodos = (NodoBepsTree*)malloc(sizeof(NodoBepsTree)* bepsTree->capacidad_nodos);

    memset(&bepsTree->nodos[0], 0, sizeof(NodoBepsTree));
    
    return bepsTree;
}

// liberar_bepsTree
// libera la memoria del árbol
// t: árbol a liberar
void liberar_bepsTree(BepsTree* t){
    free(t->nodos);
    free(t);
}

// leer_nodo
// suma 1 a la cantidad de lecturas realizadas y retorna el nodo leido en posicion i
// bepsTree: puntero al árbol
// i: posicion del nodo a leer
// retorna una copia del nodo leido
NodoBepsTree leer_nodo(BepsTree* bepsTree, int i){
    bepsTree->lecturas++;
    return bepsTree->nodos[i];
}


// escribir_nodo
// suma 1 a la cantidad de escrituras realizadas y escribe el nodo en posicion i reemplazando el que estaba en esa posiscion
// bepsTree: puntero al árbol
// i : posicion del nodo a escribir
// nodo: nodo a escribir
void escribir_nodo(BepsTree* bepsTree, int i, NodoBepsTree nodo){
    bepsTree->escrituras++;
    bepsTree->nodos[i] = nodo;
}

// agregar_nodo
// escribe un nodo al final del arreglo (agrandándolo si hace falta) y cuenta 1 escritura.
// bepsTree: puntero al árbol
// nodo: nodo a agregar
// retorna índice donde quedó el nodo
int agregar_nodo(BepsTree* bepsTree, NodoBepsTree nodo){
    if (bepsTree->cantidad_nodos == bepsTree->capacidad_nodos){
        bepsTree->capacidad_nodos *= 2; // duplicar evita copiar el arreglo entero muchas veces
        NodoBepsTree* nuevo = (NodoBepsTree*)realloc(bepsTree->nodos, sizeof(NodoBepsTree) * bepsTree->capacidad_nodos);
        bepsTree->nodos = nuevo;
    }
    int idx = bepsTree->cantidad_nodos++;
    escribir_nodo(bepsTree, idx, nodo);
    return idx;
}

// indice_hijo
// busca el indice de un nodo hijo i usando que contiene llaves > llaves[i-1] y <= llaves[i]
// n: nodo interno
// llave: llave a ubicar
// retorna: posición del hijo (0..k_llaves) que le corresponde a la llave
int indice_hijo(NodoBepsTree* n, int llave){
    int i = 0;
    while (i < n->k_llaves && llave > n->llaves[i]) i++;
    return i;
}

// Insertar_en_padre
// Utilidad para insertar una llave divisoria en el padre de un nodo que se ha dividido.
// padre: nodo padre donde se inserta la llave
// llave: llave divisoria a insertar
// idx_hijo_der: indice del nuevo hijo derecho, queda justo despues del hijo que se dividio
void insertar_en_padre(NodoBepsTree* padre, int llave, int idx_hijo_der){
    int pos = indice_hijo(padre, llave);
    for (int j = padre->k_llaves; j > pos; j--)     padre->llaves[j] = padre->llaves[j-1];
    for (int j = padre->k_llaves + 1; j > pos + 1; j--) padre->hijos[j] = padre->hijos[j-1];
    padre->llaves[pos] = llave;
    padre->hijos[pos + 1] = idx_hijo_der;
    padre->k_llaves++;
}

// insertar_par_ordenado
// inserta un par en un arreglo ordenado por llave, manteniéndolo ordenado
// buf: arreglo (con espacio para un elemento más)
// k: cantidad de elementos (se incrementa)
void insertar_par_ordenado(ParLlaveValor* buf, int* k, ParLlaveValor par){
    int i = *k - 1;
    while (i >= 0 && buf[i].llave > par.llave){
        buf[i + 1] = buf[i];
        i--;
    }
    buf[i + 1] = par;
    (*k)++;
}

// mezclar
// mezcla dos arreglos ordenados por llave en out (de tamaño na + nb, distinto de a y b)
void mezclar(ParLlaveValor* a, int na, ParLlaveValor* b, int nb, ParLlaveValor* out){
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) out[k++] = (b[j].llave < a[i].llave) ? b[j++] : a[i++];
    while (i < na) out[k++] = a[i++];
    while (j < nb) out[k++] = b[j++];
}

// SPLIT Y VACIADO DE BUFFER, SECCION 3.4.3

// partir_hoja
// parte un arreglo ordenado de pares en dos hojas. La mitad izquierda se queda con los
// primeros n/2 pares y su última llave sube como divisoria.
// izq, der: nodos de salida
// llave: llave divisoria de salida
// No retorna ya que pasa la informacion directamente a los valores de memoria entregados
void partir_hoja(ParLlaveValor* arr, int n, NodoBepsTree* izq, NodoBepsTree* der, int* llave){
    int p = n / 2;
    for (int d = 0; d < n; d++){
        int c1 = n/2 - d, c2 = n/2 + d;
        if (c1 >= 1 && c1 < n && arr[c1-1].llave != arr[c1].llave && c1 <= lmax && n - c1 <= lmax){ p = c1; break; }
        if (c2 >= 1 && c2 < n && arr[c2-1].llave != arr[c2].llave && c2 <= lmax && n - c2 <= lmax){ p = c2; break; }
    }
    memset(izq, 0, sizeof(NodoBepsTree));
    memset(der, 0, sizeof(NodoBepsTree));
    izq->k_buffer = p;
    der->k_buffer = n - p;
    memcpy(izq->buffer, arr, p * sizeof(ParLlaveValor));
    memcpy(der->buffer, arr + p, (n - p) * sizeof(ParLlaveValor));
    *llave = arr[p - 1].llave;
}
 
// split_buffer_hoja
// split de una hoja cuando se llena su buffer, divide su contenido en dos
// nodo_lleno: nodo hoja que se llena
// retorna: resultado con los nodos izquierdo y derecho (por valor) y la llave divisoria
Resultado split_buffer_hoja(NodoBepsTree nodo_lleno){
    Resultado res;
    partir_hoja(nodo_lleno.buffer, nodo_lleno.k_buffer, &res.izq, &res.der, &res.llave);
    return res;
}

// split_nodo_interno
// split al llenarse las llaves divisorias de un nodo interno (k_llaves == C)
Resultado split_nodo_interno(NodoBepsTree nodo_lleno){
    
    Resultado res;
    memset(&res, 0, sizeof(res));
    // LLave divisoria para el nodo padre, que es la llave del ultimo par del nodo izquierdo
    int llave_mediana = nodo_lleno.llaves[C/2 -1];

    res.izq.es_interno = res.der.es_interno = 1;
    res.izq.k_buffer = res.der.k_buffer = 0;
    res.izq.k_llaves = C/2 -1;
    res.der.k_llaves = C/2;

    for(int i = 0; i < C/2 -1 ; i++){
        res.izq.llaves[i] = nodo_lleno.llaves[i];
        res.izq.hijos[i] = nodo_lleno.hijos[i];
    }
    res.izq.hijos[C/2 -1] = nodo_lleno.hijos[C/2 -1];

    for (int j = 0; j < C/2 ; j++){
        res.der.llaves[j] = nodo_lleno.llaves[j + (C/2)];
        res.der.hijos[j] = nodo_lleno.hijos[j + (C/2)];
    }
    res.der.hijos[C/2] = nodo_lleno.hijos[C];

    for (int k = 0; k < nodo_lleno.k_buffer; k++){
        if (nodo_lleno.buffer[k].llave <= llave_mediana){
            res.izq.buffer[res.izq.k_buffer++] = nodo_lleno.buffer[k];
        } else {
            res.der.buffer[res.der.k_buffer++] = nodo_lleno.buffer[k];
        }
    }

    res.llave = llave_mediana;
    return res;

}

// entregar_a_hijo
// intenta pasar m pares (ordenados, todos del rango del hijo i) del buffer de V al hijo i.
// NO modifica el buffer de V: quien llama borra los pares solo si el retorno es 0.
// Retorna:
//   0: los pares quedaron en el hijo (o en sus mitades si se dividió una hoja). V pudo ganar una llave.
//   1: haría falta agregar una llave a V pero V ya tiene C llaves. No se perdió ni duplicó nada.
//      Quien llama debe devolver 1 hacia arriba para que V sea dividido.
//   2: el hijo interno se llenó de llaves y fue dividido (V ganó una llave). Los pares NO se entregaron,
//      siguen en el buffer de V; hay que recalcular los grupos y volver a intentar.
// bepsTree: árbol; V: nodo padre (copia en memoria, quien llama lo escribe); i: posición del hijo
// elems: pares a entregar; m: cantidad; alpha: parámetro del flush (para flush recursivos)

int entregar_a_hijo(BepsTree* bepsTree, NodoBepsTree* V, int i, ParLlaveValor* elems, int m, double alpha){
    int idx = V->hijos[i];
    NodoBepsTree U = leer_nodo(bepsTree, idx);
 
    if (U.es_interno == 0){
        // Hoja que no se llena: se mezclan los pares con los de la hoja
        if (U.k_buffer + m <= lmax){
            ParLlaveValor tmp[lmax];
            mezclar(U.buffer, U.k_buffer, elems, m, tmp);
            U.k_buffer += m;
            memcpy(U.buffer, tmp, U.k_buffer * sizeof(ParLlaveValor));
            escribir_nodo(bepsTree, idx, U);
            return 0;
        }
        // Hoja se llena: hay que dividir la hoja, lo que agrega una llave a V
        if (V->k_llaves == C) return 1; // V ya tiene maximo de llaves, retorna 1 para que sea dividido
        // Se mezcla primero y se parte después: así ninguna mitad puede desbordarse (n <= 2*lmax)
        int n = U.k_buffer + m;
        ParLlaveValor* tmp = (ParLlaveValor*)malloc(n * sizeof(ParLlaveValor));
        mezclar(U.buffer, U.k_buffer, elems, m, tmp);
        NodoBepsTree izq, der;
        int llave;
        partir_hoja(tmp, n, &izq, &der, &llave);
        free(tmp);
        int idx_der = agregar_nodo(bepsTree, der);  // der va al final
        escribir_nodo(bepsTree, idx, izq);          // izq reemplaza a la hoja original
        insertar_en_padre(V, llave, idx_der);
        return 0;
    }
 
    // Nodo interno: se vacía su buffer hasta que quepan los m pares
    while (U.k_buffer + m > lmax){
        int r = vaciar_buffer_nodo_interno(bepsTree, &U, alpha);
        if (r == 1){
            // U quedó con C llaves y necesita agregar otra: hay que dividir U, lo que agrega una llave a V
            if (V->k_llaves == C){
                escribir_nodo(bepsTree, idx, U); // dejar a U consistente con lo que ya bajó a sus hijos
                return 1;
            }
            Resultado res = split_nodo_interno(U);
            int idx_der = agregar_nodo(bepsTree, res.der);
            escribir_nodo(bepsTree, idx, res.izq);
            insertar_en_padre(V, res.llave, idx_der);
            return 2;
        }
    }
    ParLlaveValor tmp[lmax];
    mezclar(U.buffer, U.k_buffer, elems, m, tmp);
    U.k_buffer += m;
    memcpy(U.buffer, tmp, U.k_buffer * sizeof(ParLlaveValor));
    escribir_nodo(bepsTree, idx, U);
    return 0;
}

// vaciar_buffer_nodo_interno
// flush del buffer de un nodo interno V, modifica la copia en memoria V
// bepsTree: arbol
// V: nodo interno a vaciar
// alpha: parametro para umbral
// retorna: 0 si terminó bien, 1 si V necesita ser dividido
int vaciar_buffer_nodo_interno(BepsTree* bepsTree, NodoBepsTree* V, double alpha){
    int k_inicial = V->k_buffer;
    if (k_inicial == 0) return 0;
 
    do {
        // Parte a) Agrupar. Como el buffer está ordenado, los pares de cada hijo son un tramo contiguo
        int cantidad_hijos = V->k_llaves + 1;
        int inicio[C + 1], cuenta[C + 1];
        int pos = 0;
        for (int i = 0; i < cantidad_hijos; i++){
            inicio[i] = pos;
            if (i < V->k_llaves){
                while (pos < V->k_buffer && V->buffer[pos].llave <= V->llaves[i]) pos++;
            } else {
                pos = V->k_buffer;
            }
            cuenta[i] = pos - inicio[i];
        }
 
        // Parte b) umbral
        double f = ceil((double)V->k_buffer / cantidad_hijos);
        int umbral = (int)ceil(f * alpha - 1e-9);
        if (umbral < 1) umbral = 1;
 
        // Parte c) Procesar de derecha a izquierda
        for (int i = cantidad_hijos - 1; i >= 0; i--){
            if (cuenta[i] == 0 || cuenta[i] < umbral) continue;
            int r = entregar_a_hijo(bepsTree, V, i, V->buffer + inicio[i], cuenta[i], alpha);
            if (r == 1) return 1;
            if (r == 0){
                // Sacar el tramo entregado del buffer de V (los tramos de hijos menores no se mueven)
                int resto = V->k_buffer - inicio[i] - cuenta[i];
                memmove(V->buffer + inicio[i], V->buffer + inicio[i] + cuenta[i], resto * sizeof(ParLlaveValor));
                V->k_buffer -= cuenta[i];
            }
            // r == 2: un hijo se dividió y V ganó una llave; los índices menores a i siguen valiendo
        }
    } while (V->k_buffer == k_inicial); // si solo hubo divisiones y no bajó nada, otra vuelta
 
    return 0;
}

// SECCION 3.4.4
// insertar_bepsTree
// inserta un par (llave, valor) en el árbol, entrando por la raiz
// bepsTree: árbol
// llave, valor: el par 
// alpha: parámetro alpha del flush
void insertar_bepsTree(BepsTree* bepsTree, int llave, float valor, double alpha){
    ParLlaveValor par = { llave, valor };
    NodoBepsTree R = leer_nodo(bepsTree, 0);
    

    if (R.es_interno == 0){
        if (R.k_buffer < lmax){
            insertar_par_ordenado(R.buffer, &R.k_buffer, par);
            escribir_nodo(bepsTree, 0, R);
            return;
        }
        // Raíz hoja llena: split, y nueva raíz interna con el par en su buffer
        Resultado res = split_buffer_hoja(R);
        int idx_izq = agregar_nodo(bepsTree, res.izq);
        int idx_der = agregar_nodo(bepsTree, res.der);
        NodoBepsTree nueva;
        memset(&nueva, 0, sizeof(nueva));
        nueva.es_interno = 1;
        nueva.k_llaves = 1;
        nueva.llaves[0] = res.llave;
        nueva.hijos[0] = idx_izq;
        nueva.hijos[1] = idx_der;
        insertar_par_ordenado(nueva.buffer, &nueva.k_buffer, par);
        escribir_nodo(bepsTree, 0, nueva);
        return;
    }
 
    // Raíz interna
    while (R.k_buffer == lmax){
        int r = vaciar_buffer_nodo_interno(bepsTree, &R, alpha);
        if (r == 1){
            // La raíz necesita otra llave y ya tiene C: dividir la raíz, nueva raíz en la posición 0
            Resultado res = split_nodo_interno(R);
            int idx_izq = agregar_nodo(bepsTree, res.izq);
            int idx_der = agregar_nodo(bepsTree, res.der);
            NodoBepsTree nueva;
            memset(&nueva, 0, sizeof(nueva));
            nueva.es_interno = 1;
            nueva.k_llaves = 1;
            nueva.llaves[0] = res.llave;
            nueva.hijos[0] = idx_izq;
            nueva.hijos[1] = idx_der;
            R = nueva; // buffer vacío: hay espacio para el par
        }
    }
    insertar_par_ordenado(R.buffer, &R.k_buffer, par);
    escribir_nodo(bepsTree, 0, R);
}
