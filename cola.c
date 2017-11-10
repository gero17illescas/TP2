#include "cola.h"
#include <stdlib.h>


/* Definicion del struct nodo. */

struct nodo{
    void* dato;
    struct nodo* prox;
}typedef nodo_t;


/*Definicion del struct cola*/

struct cola{
    nodo_t* prim;
    nodo_t* ult;
};

nodo_t* nodo_crear(void* valor){
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if(!nodo){
        return NULL;
    }
    nodo->dato = valor;
    nodo->prox = NULL;
    return nodo;
}

/* *****************************************************************
   *                   PRIMITIVAS DE LA COLA                       *
   ***************************************************************** */


cola_t* cola_crear(void){
    cola_t* cola = malloc(sizeof(cola_t));
    if(!cola){
        return NULL;
    }
    cola->prim = NULL;
    cola->ult = NULL;
    return cola;
}

void cola_destruir(cola_t* cola, void destruir_dato(void*)){
    while(cola->prim){
        void* dato = cola_desencolar(cola);
        if(destruir_dato){
            destruir_dato(dato);
        }
    }
    free(cola);
}


bool cola_esta_vacia(const cola_t* cola) {
    return !cola->prim;
}

void* cola_ver_primero(const cola_t* cola){
    if(!cola->prim){
        return NULL;
    }
    return cola->prim->dato;
}

bool cola_encolar(cola_t* cola, void* valor){
    nodo_t* nodo = nodo_crear(valor);
    if(!nodo){
        return false;
    }
    if(!cola->prim){
        cola->prim = nodo;
    }else {
        cola->ult->prox = nodo;
    }
    cola->ult = nodo;
    return true;
}


void* cola_desencolar(cola_t* cola){
    if(!cola->prim){
        return NULL;
    }
    nodo_t* temp = cola->prim;
    cola->prim = cola->prim->prox;
    if(!cola->prim){
        cola->ult = NULL;
    }
    void* dato_aux = temp->dato;
    free(temp);
    return dato_aux;
}