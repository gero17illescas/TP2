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

struct abb {
	abb_nodo_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cant_nodos;
};

struct abb_iter{
	pila_t* pila;
};


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
/* Destruye el nodo que recibe por parametro y destruyte el dato que 
 * que contiene si la funcion destruir dato esta definida.
 */
void* nodo_destruir(abb_nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	if(!nodo)
		return NULL;
	void* dato = nodo->dato;
	free(nodo->clave);
	free(nodo);
	if(destruir_dato){
		destruir_dato(dato);
		return NULL;
	}
	return dato;
}
/* Recibe un nodo de arbol binario de busqueda, un clave y una fucnion
 * de comparacion. Busca en los subarboles del nodo o el padre,segun el
 * parametro aux, si j es uno busca el padre del nodo, si es 0 devuelve 
 * el nodo que coincida con la clave pasada por parametro. Si no se 
 * encuntra devuelve NULL.
 * Pre: cmp fue definida.
 */
abb_nodo_t* abb_nodo_buscar(abb_nodo_t* nodo,abb_nodo_t* padre,const char* clave, abb_comparar_clave_t cmp,bool aux){
	if(!nodo && aux) return padre;
	if(!nodo) return NULL;
	int i = cmp(nodo->clave, clave);
	if(i > 0)
		return abb_nodo_buscar(nodo->izq,nodo,clave,cmp,aux);
	if(i < 0)
		return abb_nodo_buscar(nodo->der,nodo,clave,cmp,aux);
	if(aux) 
		return padre;
	return nodo;
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
bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	if(!arbol->raiz){
		arbol->raiz = crear_nodo_abb(clave,dato);
		if(!arbol->raiz)	return false;
		arbol->cant_nodos++;
		return true;
	}

	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz,NULL,clave,arbol->cmp,false);

	if(!nodo){
		abb_nodo_t* padre = abb_nodo_buscar(arbol->raiz,NULL,clave,arbol->cmp,true);
		nodo = crear_nodo_abb(clave,dato);

		if(!nodo)	
			return false;
		if(arbol->cmp(padre->clave,clave)<0)
			padre->der = nodo;
		else
			padre->izq = nodo;

		arbol->cant_nodos++;
	}else{
		if(arbol->destruir_dato)
			arbol->destruir_dato(nodo->dato);
		nodo->dato = dato;
	}
	return true;
}

void* reemplazar_y_borrar(abb_t* arbol,abb_nodo_t* nodo, abb_nodo_t* padre,abb_nodo_t* aux){
	if(!padre)
		arbol->raiz = aux;
	else{
		int i = arbol->cmp(padre->clave,nodo->clave);
		if (i > 0)	padre->izq = aux;
		if (i < 0)	padre->der = aux;
	}
	return nodo_destruir(nodo,NULL);
}


/* Recibe una clave y se encarga de eliminar el dato asociado y lo 
 * devuelve. Si la clave no pertenece a ningun dato del arbol, o este
 * esta vacio se devuelve NULL.
 * Pre: el arbol fue creado.
 */
void* abb_borrar(abb_t *arbol, const char *clave){
	if(!abb_pertenece(arbol, clave))	return NULL;
	abb_nodo_t* aux;
	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz,NULL,clave,arbol->cmp,false);
	abb_nodo_t* padre = abb_nodo_buscar(arbol->raiz,NULL,clave,arbol->cmp,true);

	if(!nodo->der && !nodo->izq)
		aux = NULL;

	if(!nodo->izq || !nodo->der){
		if(nodo->izq)	aux = nodo->izq;
		else aux = nodo->der;
	}

	if(nodo->izq && nodo->der){
		aux = abb_nodo_buscar_minimo(nodo->der);
		abb_nodo_t* padre_reemplazo = abb_nodo_buscar(nodo->der,nodo,aux->clave,arbol->cmp,true);
		if(arbol->cmp(padre_reemplazo->clave, nodo->clave) == 0)
		    aux->izq = nodo->izq;
		else{
			padre_reemplazo->izq = aux->der;
			aux->der = nodo->der;
			aux->izq = nodo->izq;
		}
	}
	arbol->cant_nodos--;
	return reemplazar_y_borrar(arbol,nodo,padre,aux);
}


/* Recibe un arbol y una clave asociada a un valor, se buca en el arbol
 * el valor asociado, si se lo encuentra se lo devuelve. Caso contrario
 * devuelve NULL.
 * Pre: el arbol fue creado.
 */
void *abb_obtener(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz,NULL,clave,arbol->cmp,false);
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
	return abb_nodo_buscar(arbol->raiz,NULL, clave, arbol->cmp,false);
}

/* Devuelve la cantida de claves que hay en el arbol.
 * Pre: el arbol fue creado.
 */
size_t abb_cantidad(abb_t *arbol){
	return arbol->cant_nodos;
}


void _abb_destruir(abb_nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	if(!nodo)	return;

	_abb_destruir(nodo->izq, destruir_dato);
	_abb_destruir(nodo->der, destruir_dato);
	nodo_destruir(nodo,destruir_dato);
}
	
void abb_destruir(abb_t *arbol){
	_abb_destruir(arbol->raiz, arbol->destruir_dato);
	free(arbol);
}

/* Itera de manera inorder sobre el arbol aplicandole la funcion visitar
 * a cada clave y dato asociado que se encuentre.
 * Pre: el arbol fue creado.
 */
bool _abb_in_order(abb_nodo_t* nodo, bool visitar(const char*, void*, void*), void* extra,char* inicio,char* final,abb_comparar_clave_t cmp){
	if(!nodo) 
		return true;
	int i = cmp(nodo->clave, inicio);//necesito que sea 1,0 para quye este dentro del intervalo
	int f = cmp(nodo->clave, final);//necesito que sea -1.0 para quye este dentro del intervalo

	if(i < 0)	//si el nodo es mas chico que el inicio me voy hacia la derecha
		return _abb_in_order(nodo->der,visitar,extra,inicio,final,cmp);
	
	if(f > 0)	//si el nodo es mas grande que el final me voy hacia la izquierda
		return _abb_in_order(nodo->izq,visitar,extra,inicio,final,cmp);
		
	if(!_abb_in_order(nodo->izq,visitar,extra,inicio,final,cmp))
		return false;
	if(visitar(nodo->clave,nodo->dato,extra))
		return _abb_in_order(nodo->der,visitar,extra,inicio,final,cmp);
	return false;
}
void abb_in_order(abb_t* arbol, bool visitar(const char*, void*, void*), void* extra,char* inicio,char* final){
	_abb_in_order(arbol->raiz,visitar,extra,inicio,final,arbol->cmp);
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
    return iter;
}

const char* abb_iter_in_ver_actual(const abb_iter_t* iter){
	abb_nodo_t* nodo = pila_ver_tope(iter->pila);
	if(nodo)	
		return nodo->clave;
	return NULL;
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
		
    return true;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}