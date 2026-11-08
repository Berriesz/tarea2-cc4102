# Tarea-2-B-tree-vs-Bε-tree

## Requisitos

Para ejecutar el proyecto se requiere:

* GCC
* Estándar C11

---

## Compilación

Desde la carpeta raíz del proyecto, ejecutar:

```bash
gcc -O2 -std=c11 experimentos.c Btree/btree.c -o experimentos

gcc -O2 -std=c11 -DC=4 -Dlmax=506 experimentos_epsilon.c epsilonBtree/epsilonBtree.c -o experimentos025

gcc -O2 -std=c11 -DC=126 -Dlmax=384 experimentos_epsilon.c epsilonBtree/epsilonBtree.c -o experimentos075

gcc -O2 -std=c11 busquedas_btree.c Btree/btree.c -o busquedas_btree
```

---

## Ejecución

```bash
./experimentos

./experimentos025 datos.bin 0.25 resultados_epsilon025.csv

./experimentos075 datos.bin 0.75 resultados_epsilon075.csv

./busquedas_btree
```

Se requiere el archivo `datos.bin` para ejecutar los experimentos. El archivo `rangos.csv` permite utilizar los mismos rangos de búsqueda en las distintas ejecuciones.