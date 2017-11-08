#include "pila.h"
#include <stdlib.h>
#define TAM_INICIAL 8
#define COEF_REDIM 2
#define VAL_MINIMO 4

bool pila_redimensionar(pila_t* pila, size_t tam_nuevo);

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
   *                   PRIMITIVAS DE LA PILA                       *
   ***************************************************************** */


pila_t* pila_crear(void){
    pila_t* pila = malloc(sizeof(pila_t));
    if(!pila) {
        return NULL;
    }
    pila->capacidad = TAM_INICIAL;
    pila->datos = malloc(sizeof(void*) * TAM_INICIAL);
    if(!pila->datos){
        free(pila);
        return NULL;
    }
    pila->cantidad = 0;
    return pila;
}

void pila_destruir(pila_t* pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t* pila){
    return !pila->cantidad;
}

void* pila_ver_tope(const pila_t* pila){
    if(!pila->cantidad) {
        return NULL;
    }
    return pila->datos[pila->cantidad-1];
}

bool pila_apilar(pila_t* pila, void* valor) {
    if (pila->cantidad == pila->capacidad) {
        size_t tam_nuevo = pila->capacidad * COEF_REDIM;
        if (!pila_redimensionar(pila, tam_nuevo)) {
            return false;
        }
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad++;
    return true;
}

void* pila_desapilar(pila_t* pila){
    if(!pila->cantidad){
        return NULL;
    }
    if(pila->cantidad * VAL_MINIMO <= pila->capacidad && pila->cantidad * VAL_MINIMO >= TAM_INICIAL){
        size_t tam_nuevo = pila->cantidad * COEF_REDIM;
        if(!pila_redimensionar(pila, tam_nuevo)){
            return false;
        }
    }
    pila->cantidad--;
    return pila->datos[pila->cantidad];
}


bool pila_redimensionar(pila_t* pila, size_t tam_nuevo){
    void* datos_nuevos = realloc(pila->datos, tam_nuevo * sizeof(void*));
    if(tam_nuevo > 0 && datos_nuevos == NULL){
        return false;
    }
    pila->capacidad = tam_nuevo;
    pila->datos = datos_nuevos;
    return true;
}