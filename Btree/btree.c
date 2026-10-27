
#define B 340 // se puede cambiar para probar arboles chicos!!
#define INCREMENTO_CAPACIDAD 1024

//btree
//pares llave-valor: 
//  -int: 4 bytes tiempo en el que se toma medición
//  -float: 4 bytes temperatura medida en ese instante

typedef struct{
    int llave; // tiempo en el que se toma medición
    float valor; // temperatura medida en ese instante
} ParLlaveValor;



// nodo --> tamaño fijo de 4096bytes
//  nodo que permite un máximo de 𝑏 = 340 elementos en su interior


typedef struct{
    int es_interno; // 1 si es interno, 0 si es hoja
    int k ; // cantidad pares llave-valor actualmente en el nodo.
    ParLlaveValor llaves_valores[B]; // arreglo de pares llave-valor. DEBEN estar siempre en orden ASCENDENTE
    int hijos[B + 1]; // arreglo de posiciones en disco de los hijos del nodo.
    int padding; // usado para que el nodo tenga un tamaño fijo de 4096 bytes

} NodoBtree;

// • El árbol será representado como un arreglo (o vector) de nodos en RAM; el primer elemento del arreglo siempre será el nodo raíz.
// • Cada nodo interno guardará el índice en el cual está ubicado cada uno de sus hijos.
// • Cada vez que se acceda al arreglo de nodos para leer un elemento, se considerará como una lectura.
// • Cada vez que se escriba un elemento en el arreglo (ya sea modificando una posición existente, o agregando un nuevo elemento al final), se considerará como una escritura.

typedef struct{
    NodoBtree* nodos; // arreglo de nodos en RAM
    int cantidad_nodos; // cantidad de nodos actualmente en el arreglo
    int capacidad_nodos; // capacidad del arreglo de nodos
    long long lecturas; // cantidad de lecturas realizadas
    long long escrituras; // cantidad de escrituras realizadas
} Btree;



// Crear árbol vacio
Btree* crear_btree() {
    Btree* btree = (Btree*)malloc(sizeof(Btree));
    btree->escrituras = 0;
    btree->lecturas = 0;
    btree->cantidad_nodos = 1;
    btree->capacidad_nodos = INCREMENTO_CAPACIDAD;

    btree->nodos = (NodoBtree*)malloc(sizeof(NodoBtree));

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