#include "abb.h"
#include "math.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pila.h"

/* *****************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct nodo {
	struct nodo* izq;
	struct nodo* der;
	char *clave;
	void *dato;
}abb_nodo_t;

typedef struct abb {
	abb_nodo_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cant_nodos;
}abb_t;

typedef struct abb_iter{
	abb_nodo_t* actual;
	pila_t* pila;
}abb_iter_t;


/* *****************************************************************
 *                DEFINICION DE FUNCIONES AUXILIARES
 * *****************************************************************/
/* Crea un nodo para un arbol binario de busqueda. Si fallo el pedido
 * de memoria devuleve NULL.
 */
abb_nodo_t* crear_nodo_abb(const char* clave,void* dato){
	if(!clave) return NULL;
	abb_nodo_t* nodo_abb = malloc(sizeof(abb_nodo_t));
	if (!nodo_abb) return NULL;

	nodo_abb->izq = NULL;
	nodo_abb->der = NULL;
	nodo_abb->dato = dato;
	nodo_abb->clave = malloc(sizeof(char)*strlen(clave)+1);

	if(! nodo_abb->clave){
		free(nodo_abb);
		return NULL;
	}

	strcpy(nodo_abb->clave,clave);
	return nodo_abb;
}

void* nodo_destruir(abb_nodo_t* nodo){
	if(!nodo)
		return NULL;
	void* dato = nodo->dato;
	free(nodo->clave);
	free(nodo);
	return dato;
}
/* Recibe un nodo de arbol binario de busqueda, un clave y una fucnion
 * de comparacion. Busca en los subarboles del nodo el nodo identificado
 * con la clave pasada por parametro. Si no se encuntra devuelve NULL.
 * Pre: cmp fue definida.
 */
abb_nodo_t* abb_nodo_buscar(abb_nodo_t* nodo,const char* clave, abb_comparar_clave_t cmp){
	if(!nodo) return NULL;
	int i = cmp(nodo->clave, clave);
	if(i > 0)
		return abb_nodo_buscar(nodo->izq,clave,cmp);
	if(i < 0)
		return abb_nodo_buscar(nodo->der,clave,cmp);
	return nodo;
}

/* Recibe un nodo de arbol binario de busqueda, un clave, un nodo 
 * anterior y una fucnion de comparacion. Busca en los subarboles del 
 * nodo el nodo identificado con la clave pasada por parametro y devuelve
 * el nodo padre.Si no se encuntra devuelve NULL.
 * Pre: cmp fue definida.
 */
abb_nodo_t* abb_nodo_buscar_padre(abb_nodo_t* nodo,abb_nodo_t* padre,const char* clave, abb_comparar_clave_t cmp){
	if(!nodo) return NULL;
	int i = cmp(nodo->clave, clave);
	if(i > 0)
		return abb_nodo_buscar_padre(nodo->izq,nodo,clave,cmp);
	if(i < 0)
		return abb_nodo_buscar_padre(nodo->der,nodo,clave,cmp);
	return padre;
}

/* Busca el nodo cuya clave sea la minima a partir del nodo pasado.
 * Pre: El nodo pasado por parametro existe
 * Post: Devuelve el nodo de minima clave
 */
abb_nodo_t* abb_nodo_buscar_minimo(abb_nodo_t* nodo){
	if(!nodo) return NULL;
	if(!nodo->izq)
		return nodo;
	return abb_nodo_buscar_minimo(nodo->izq);
}

 
/* *****************************************************************
 *                     PRIMITIVAS DEL ABB
 * *****************************************************************/
/* Recibe funciones para comparar los datos entre si y para destruirlos
 * Devuelve un puntero a un arbol binario vacio (raiz nula)
 * Pre: cmp y destruir_dato fueron previamente definidas.
 */
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t * arbol = malloc(sizeof(abb_t));
	if(!arbol) return NULL;
	arbol->raiz = NULL;
	arbol->destruir_dato = destruir_dato;
	arbol->cmp = cmp;
	arbol->cant_nodos = 0;
	return arbol;
}

/* Reciben un valor y una clave asociada al valor y lo guarda en el arbol.
 * Devuleve un boolenao si la operacion fue definida.
 * Pre: el arbol fue previamente creado.
 */

bool _abb_guardar(abb_nodo_t *nodo, abb_nodo_t* padre,abb_t *arbol, const char *clave, void *dato){
	if(!nodo){
		nodo = crear_nodo_abb(clave,dato);
		if(!nodo)
			return false;
		if(arbol->cmp(padre->clave,clave)<0)
			padre->der = nodo;
		else
			padre->izq = nodo;
		arbol->cant_nodos++;
		return true;
	}

	int i = arbol->cmp(nodo->clave, clave);
	//es positivo si la clave a guardar es menor
	//es negativo si la clave a guardar es mayor
	if(i > 0)
		return _abb_guardar(nodo->izq,nodo,arbol,clave,dato);
	if(i < 0)
		return _abb_guardar(nodo->der,nodo,arbol,clave,dato);
	if(arbol->destruir_dato)
		arbol->destruir_dato(nodo->dato);
	nodo->dato = dato;
	return true;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	if(!arbol->raiz){
		arbol->raiz = crear_nodo_abb(clave,dato);
		if(!arbol->raiz)
			return false;
		arbol->cant_nodos++;
		return true;
	}
	if(!_abb_guardar(arbol->raiz,NULL,arbol,clave,dato))
		return false;
	return true;
}


void* abb_borrar_sin_hijos(abb_t* arbol, abb_nodo_t* nodo, abb_nodo_t* padre){
	if(!padre) {
		arbol->raiz = NULL;
		arbol->cant_nodos--;
		return nodo_destruir(nodo);

	}
	int i = arbol->cmp(padre->clave, nodo->clave);
	if(i > 0)   padre->izq = NULL;
	if(i < 0)   padre->der = NULL;

	arbol->cant_nodos--;
	return nodo_destruir(nodo);
}

void* abb_borrar_hijo_unico(abb_t* arbol, abb_nodo_t* nodo, abb_nodo_t* padre){
	abb_nodo_t* aux;
	if(nodo->izq && !nodo->der){//Tiene hijo izquierdo
		if(!padre)
			arbol->raiz = nodo->izq;
		else
			aux = nodo->izq;
	}
	if(nodo->der && !nodo->izq){//Tiene hijo derecho
		if (!padre)
			arbol->raiz = nodo->der;
		else
			aux = nodo->der;
	}
	if(padre){
		int i = arbol->cmp(padre->clave,nodo->clave);
		if (i > 0)	padre->izq = aux;
		if (i < 0)	padre->der = aux;
	}
	
	arbol->cant_nodos--;
	return nodo_destruir(nodo);
}

void* abb_borrar_dos_hijos(abb_t* arbol, abb_nodo_t* nodo, abb_nodo_t* padre){
	abb_nodo_t* reemplazo = abb_nodo_buscar_minimo(nodo->der);
	abb_nodo_t* padre_reemplazo = abb_nodo_buscar_padre(nodo->der,nodo,reemplazo->clave,arbol->cmp);
	if(arbol->cmp(padre_reemplazo->clave, nodo->clave) == 0)
	    reemplazo->izq = nodo->izq;
	else{
		padre_reemplazo->izq = reemplazo->der;
		reemplazo->der = nodo->der;
		reemplazo->izq = nodo->izq;
	}

	if(!padre)
		arbol->raiz = reemplazo;
	else{
		int i = arbol->cmp(padre->clave,nodo->clave);
		if(i > 0)
			padre->izq = reemplazo;
		if(i < 0)
			padre->der = reemplazo;
	}
	
	arbol->cant_nodos--;
	return nodo_destruir(nodo);;
}

/* Recibe una clave y se encarga de eliminar el dato asociado y lo 
 * devuelve. Si la clave no pertenece a ningun dato del arbol, o este
 * esta vacio se devuelve NULL.
 * Pre: el arbol fue creado.
 */
void* abb_borrar(abb_t *arbol, const char *clave){
	if(!abb_pertenece(arbol, clave))	return NULL;

	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz,clave,arbol->cmp);
	abb_nodo_t* padre = abb_nodo_buscar_padre(arbol->raiz,NULL,clave,arbol->cmp);
	if(!nodo->der && !nodo->izq)
		return abb_borrar_sin_hijos(arbol, nodo, padre);
	if(!nodo->izq || !nodo->der)
		return abb_borrar_hijo_unico(arbol, nodo, padre);
	if(nodo->izq && nodo->der)
		return abb_borrar_dos_hijos(arbol, nodo, padre);

	return NULL;
}


/* Recibe un arbol y una clave asociada a un valor, se buca en el arbol
 * el valor asociado, si se lo encuentra se lo devuelve. Caso contrario
 * devuelve NULL.
 * Pre: el arbol fue creado.
 */
void *abb_obtener(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz,clave,arbol->cmp);
	if(!nodo)
		return NULL;
	return nodo->dato;
}


/* Recibe un arbol y una clave asociada a un valor, se buca en el arbol
 * el valor asociado, si se lo encuentra se devulve true. Caso contrario
 * devuelve false.
 * Pre: el arbol fue creado.
 */
bool abb_pertenece(const abb_t *arbol, const char *clave){
	if(!arbol->raiz)
		return false;
	return abb_nodo_buscar(arbol->raiz, clave, arbol->cmp) != NULL;
}

/* Devuelve la cantida de claves que hay en el arbol.
 * Pre: el arbol fue creado.
 */
size_t abb_cantidad(abb_t *arbol){
	if(!arbol)
		return 0;
	return arbol->cant_nodos;
}


void _abb_destruir(abb_nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	if(nodo->izq)
		_abb_destruir(nodo->izq, destruir_dato);

	if(nodo->der)
		_abb_destruir(nodo->der, destruir_dato);

	void* dato = nodo_destruir(nodo);
	if(destruir_dato)
		destruir_dato(dato);
}

void abb_destruir(abb_t *arbol){
	if(arbol->raiz){
		_abb_destruir(arbol->raiz, arbol->destruir_dato);
	}
	free(arbol);
}

/* Itera de manera inorder sobre el arbol aplicandole la funcion visitar
 * a cada clave y dato asociado que se encuentre.
 * Pre: el arbol fue creado.
 */
bool _abb_in_order(abb_nodo_t* nodo, bool visitar(const char*, void*, void*), void* extra){
	if(!nodo) return true;
	if(_abb_in_order(nodo->izq,visitar,extra)){
		if(visitar(nodo->clave,nodo->dato,extra))
			return _abb_in_order(nodo->der,visitar,extra);
	}
	return false;
}
void abb_in_order(abb_t* arbol, bool visitar(const char*, void*, void*), void* extra){
	if(!arbol->raiz) return;
	_abb_in_order(arbol->raiz,visitar,extra);
}


/* *****************************************************************
 *                     PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

bool apilar_hijos_izquierdos(abb_iter_t* iter, abb_nodo_t* nodo){
    while(nodo){
        if(!pila_apilar(iter->pila, nodo))
            return false;
        nodo = nodo->izq;
    }
    return true;
}

abb_iter_t* abb_iter_in_crear(const abb_t* arbol){
    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if(!iter)
        return NULL;

    pila_t* pila = pila_crear();
    if(!pila){
        free(iter);
        return NULL;
    }
    iter->pila = pila;
    if(!apilar_hijos_izquierdos(iter, arbol->raiz)){
        pila_destruir(iter->pila);
        abb_iter_in_destruir(iter);
        return NULL;
    }
    iter->actual = pila_ver_tope(iter->pila);
    return iter;
}

const char* abb_iter_in_ver_actual(const abb_iter_t* iter){
	if(iter->actual == NULL)
		return NULL;
	return iter->actual->clave;
}

bool abb_iter_in_al_final(const abb_iter_t* iter){
	return pila_esta_vacia(iter->pila);
}

bool abb_iter_in_avanzar(abb_iter_t* iter){
    if(abb_iter_in_al_final(iter))
        return false;
    abb_nodo_t* nodo = pila_desapilar(iter->pila);
    if(nodo->der)
        apilar_hijos_izquierdos(iter, nodo->der);
    iter->actual = pila_ver_tope(iter->pila);
    return true;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}
