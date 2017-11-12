#include <stdbool.h>  /* bool */
#include <stddef.h>	  /* size_t */
#include <stdlib.h>
#include "heap.h"

#define TAM_INICIAL 10
#define COEF_REDIM 2
#define VAL_MINIMO 4



/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* Tipo utilizado para el heap. */
struct heap{
	cmp_func_t cmp;
	void** tabla;
	size_t cant;
	size_t tam;
};

/* *****************************************************************
 *                DEFINICION DE FUNCIONES AUXILIARES
 * *****************************************************************/

/* Intercambia dos valores genericos.
 */
void swap (void** x, void** y) {
	void* aux = *x;
	*x = *y;
	*y = aux;
}
/* Recibe un heap y una posicion valida y devuelve la posicion del
 * hijo con el mayor valor
 */
size_t hijo_max(heap_t* heap, size_t pos_actual){
    size_t pos_hijo_izq = 2 * pos_actual + 1;
    size_t pos_hijo_der = pos_hijo_izq + 1;
    if (pos_hijo_izq >= heap->cant)
        return heap->cant;
    if (pos_hijo_der >= heap->cant)
        return pos_hijo_izq;
    int i = heap->cmp(heap->tabla[pos_hijo_izq], heap->tabla[pos_hijo_der]);
    if (i > 0)
        return pos_hijo_izq;
    return pos_hijo_der;
}
/* Swapea un elemento del heap hacia el inicio del array*/
void upheap(heap_t* heap, size_t pos){
    void** tabla = heap->tabla;
    while(pos>0 && heap->cmp(tabla[pos],tabla[(pos-1)/2])>0){
        swap (&tabla[pos], &tabla[(pos-1)/2]);
        pos = (pos-1)/2;
    }
}
/* Swapea un elemento del heap hacia el final del array*/
void downheap(heap_t* heap, size_t pos){
    size_t pos_max = hijo_max(heap, pos);
    if(pos_max == heap->cant)
        return;
    int i = heap->cmp(heap->tabla[pos], heap->tabla[pos_max]);
    if(i < 0){
        swap(&heap->tabla[pos], &heap->tabla[pos_max]);
        downheap(heap, pos_max);
    }
}
/* Redimensiona la tabla de heap con el tamaño nuevo 
 */
bool heap_redimensionar(heap_t* heap, size_t tam_nuevo) {
    void **tabla_nueva = realloc(heap->tabla, tam_nuevo * sizeof(void*));

    if(!tabla_nueva) return false;

    heap->tabla = tabla_nueva;
    heap->tam = tam_nuevo;
    return true;
}

/* *****************************************************************
 *                  FUNCION DE HEAP SORT
 * *****************************************************************/

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
    heap_t* heap = heap_crear(cmp);
    if(!heap) return NULL;

    size_t i;
    for(i = 0; i < n; i++) {
        if(!heap_encolar(heap, arreglo[i])) {
            free(heap);
            return NULL;
        }
    }
    return heap;
}

/* Función de heapsort genérica. Esta función ordena mediante heap_sort
 * un arreglo de punteros opacos, para lo cual requiere que se
 * le pase una función de comparación. Modifica el arreglo "in-place".
 * Nótese que esta función NO es formalmente parte del TAD Heap.
 */
void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    heap_t* auxiliar = heap_crear_arr(elementos, cant, cmp);
    if(!auxiliar) return;

    size_t i;
    for(i = 0; i < cant; i++){
        elementos[i] = heap_desencolar(auxiliar);
    }
    heap_destruir(auxiliar,NULL);
}

/* *****************************************************************
 *                  PRIMITIVAS DEL HEAP
 * *****************************************************************/

/*
 * Implementación de un TAD cola de prioridad, usando un max-heap.
 *
 * Notar que al ser un max-heap el elemento mas grande será el de mejor
 * prioridad. Si se desea un min-heap, alcanza con invertir la función de
 * comparación.
 */

/* Crea un heap. Recibe como único parámetro la función de comparación a
 * utilizar. Devuelve un puntero al heap, el cual debe ser destruido con
 * heap_destruir().
 */
heap_t* heap_crear(cmp_func_t cmp){
	heap_t* heap = malloc(sizeof(heap_t));
	if(!heap) return NULL;

	heap->cmp = cmp;
	heap->tam = TAM_INICIAL;
	heap->cant = 0;
	heap->tabla = calloc(sizeof(void*)*heap->tam,heap->tam);

	if(!heap->tabla){
		free(heap);
		return NULL;
	}

	return heap;
}

/* Elimina el heap, llamando a la función dada para cada elemento del mismo.
 * El puntero a la función puede ser NULL, en cuyo caso no se llamará.
 * Post: se llamó a la función indicada con cada elemento del heap. El heap
 * dejó de ser válido. */
void heap_destruir(heap_t* heap, void destruir_elemento(void* e)){
    if(destruir_elemento){
        for(int i=0; i<heap->tam; i++)
			destruir_elemento(heap->tabla[i]);
	}
	free(heap->tabla);
	free(heap);
}

/* Devuelve la cantidad de elementos que hay en el heap. */
size_t heap_cantidad(const heap_t *heap){
	return heap->cant;
}

/* Devuelve true si la cantidad de elementos que hay en el heap es 0, false en
 * caso contrario. */
bool heap_esta_vacio(const heap_t *heap){
	return heap->cant == 0;
}

/* Agrega un elemento al heap. El elemento no puede ser NULL.
 * Devuelve true si fue una operación exitosa, o false en caso de error.
 * Pre: el heap fue creado.
 * Post: se agregó un nuevo elemento al heap.
 */
bool heap_encolar(heap_t *heap, void *elem){
    if(!elem) return false;
    if(heap->cant == heap->tam){
        size_t tam_nuevo = heap->tam * COEF_REDIM;
        if(!heap_redimensionar(heap, tam_nuevo))
            return false;
    }
    heap->tabla[heap->cant] = elem;
    upheap(heap, heap->cant);
    heap->cant++;
	return true;
}

/* Devuelve el elemento con máxima prioridad. Si el heap esta vacío, devuelve
 * NULL.
 * Pre: el heap fue creado.
 */
void *heap_ver_max(const heap_t *heap){
    if(heap_esta_vacio(heap))
        return NULL;
    return heap->tabla[0];
}

/* Elimina el elemento con máxima prioridad, y lo devuelve.
 * Si el heap esta vacío, devuelve NULL.
 * Pre: el heap fue creado.
 * Post: el elemento desencolado ya no se encuentra en el heap.
 */
void *heap_desencolar(heap_t *heap){
    if(heap_esta_vacio(heap)) return NULL;
    if(heap->cant * VAL_MINIMO <= heap->tam && heap->cant * VAL_MINIMO >= TAM_INICIAL){
        size_t tam_nuevo = heap->cant * COEF_REDIM;
        heap_redimensionar(heap, tam_nuevo);
    }
    void* dato = heap->tabla[0];
    heap->tabla[0] = heap->tabla[heap->cant-1];
    heap->tabla[heap->cant -1] = NULL;
    heap->cant--;
    downheap(heap, 0);
    return dato;
}
