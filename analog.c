#define _POSIX_C_SOURCE 200809L //getline

#define _XOPEN_SOURCE           //time
#define TIME_FORMAT "%FT%T%z"
#define AGREGAR "agregar_archivo"
#define VISITANTES "ver_visitantes"
#define VISITADOS "ver_visitados"
#define ERROR "Error en comando %s\n"
#define BUFFERSIZE 100

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "strutil.h"
#include "abb.h"
#include "hash.h"
#include "heap.h"
typedef struct recurso {
	const char* nombre;
	int cant;
}recurso_t;

int cmp_heap(const void* dato_1,const void* dato_2){
	recurso_t* recurso_1 = dato_1;
	recurso_t* recurso_2 = dato_2;
	if(recurso_1->cant < recurso_2->cant)
		return 1;
	if(recurso_1->cant > recurso_2->cant)
		return -1;
	return 0;
}

bool es_numero(char* input){
	size_t largo = strlen(input);
	for(size_t i = 0; i < largo; i++){
		if(input[i] == '.') i++;
		if(!isdigit(input[i]))
			return false;
	}
	return true;
}

/*
	Dada una cadena en formato ISO-8601 devuelve una variable de tipo time_t
	que representa un instante en el tiempo.
*/
time_t iso8601_to_time(const char* iso8601){
	struct tm bktime = { 0 };
	strptime(iso8601, TIME_FORMAT, &bktime);
	return mktime(&bktime);
}


/* 
 * Devuleve un bool segun si el comando que se ingreso es posible
 *  ejecutar correctamente o no.
 */
bool verificar_comando(char** strv, size_t strvc,char* comando){
	size_t i = 0;
	while(strv[i])
		i++;
	return strcmp(strv[0],comando)==0 && strvc==i;
}
/* 
 * Debe recibir dos punteros del tipo char que son cadenas de ip y
 * debe devolver:
 *   menor a 0  si  ip1 < ip2
 *       0      si  ip1 == ip2
 *   mayor a 0  si  ip1 > ip2
 */
int cmp_ip(const char* ip1, const char* ip2){
	char** strv_1 = split(ip1,'.');
	char** strv_2 = split(ip2,'.');

	int i = 0;
	int j = 0;
	while(i==0 && j<4){
		i = strcmp(strv_1[j],strv_2[j]);
		j++;
	}

	free_strv(strv_1);
	free_strv(strv_2);
	return i;

}
bool analizar_dos (abb_t* abb_ip,char** strv){
	char* ip = strv[0];	char* fecha= strv[1];
	int* cont_dos;
	hash_t* hash_ip = abb_obtener(abb_ip,ip);
	
	if(hash_ip){
		int* cant_dos = hash_obtener(hash_ip,"dos");
		if(*cant_dos==1)	return true; //Cuidado si es null
		//si para esa ip ya hay dos no se hace nada, pues ya tuvo un ataque.

		time_t utlima_fecha = iso8601_to_time(hash_obtener(hash_ip,"ultima_fecha"));
		time_t nueva_fecha = iso8601_to_time(fecha);
		double dif_tiempo = difftime(utlima_fecha,nueva_fecha);

		if(dif_tiempo<2){
			cont_dos = hash_obtener(hash_ip,"cont_dos"); 
			(*cont_dos)++;
			if(*cont_dos==5){
				*cont_dos=0;
				int dos = 1;//Asumo que esto es lo que querias hacer
				if(!hash_guardar(hash_ip,"dos", &dos))	return false;
				if(!hash_guardar(hash_ip,"ultima_fecha",fecha)) return false;
				printf("DoS: %s\n",ip);
			}
			if(!hash_guardar(hash_ip,"dos",cont_dos)) return false;
		}
		else
			if(!hash_guardar(hash_ip,"ultima_fecha",fecha)) return false;
		return true;
	}

	cont_dos =  malloc(sizeof(int*));
	int* dos = malloc(sizeof(int*));

	*cont_dos=0;
	*dos=0;

	if(!hash_guardar(hash_ip,"cont_dos",cont_dos))	return false;
	if(!hash_guardar(hash_ip,"dos",dos))			return false;
	if(!hash_guardar(hash_ip,"ultima_fecha",fecha))	return false;

	return abb_guardar(abb_ip,ip,hash_ip);

}
bool analizar_resources(hash_t* hash_resources,char** strv){
	char* resource = strv[3];
    int* cont;
	if((cont = hash_obtener(hash_resources,resource))){
		(*cont)++;
		return hash_guardar(hash_resources,resource,cont);
	}
	cont = malloc(sizeof(int*));
	*cont = 1;
	return hash_guardar(hash_resources,resource,cont);

}
/* Recibe un archivo, verifica si lo puede abrir y devuelve una lista
 * de usuarios en memoria dinamica. Si hubo un problema con el arhcivo
 * devuelve NULL. Si el archivo existe y esta vacio devuelve una lista
 * vacia.
 */
void agregar_archivo(abb_t* abb_ip, hash_t* hash_resources, char* file){
	FILE* archivo = fopen(file,"r");
	if (archivo == NULL){
		fprintf(stderr,"Hubo un problema al procesar el comando.\n");
		return;
	}

	char* linea = NULL; size_t capacidad = 0; //combo getline
	char** strv;
	bool ok; bool ok2;

	while(getline(&linea,&capacidad, archivo) > 0){
		strv = split(linea,'\t');
		//208.115.111.72	2015-05-17T11:05:15+00:00	GET	/corrector.html
		ok = analizar_dos(abb_ip,strv);
		ok2= analizar_resources(hash_resources,strv);
        free_strv(strv);
	}

	free(linea);
	fclose(archivo);

	if(ok && ok2)
		printf("OK");
	else 
		fprintf(stderr,"Hubo un problema al procesar el comando.\n");
}

void ver_visitantes(abb_t* abb_ip,char* inicio,char* final){
	abb_iter_t* iter = crear_inorder_acotado(abb_ip,inicio,final);
	printf("Visitantes:");
	while(!abb_iter_in_al_final(iter)){
		printf("\t%s",abb_iter_in_ver_actual(iter));
	}
	printf("OK");
	abb_iter_in_destruir(iter);
}
void ver_mas_visitados(hash_t* hash_resources,int n){
	
	hash_iter_t* iter = hash_iter_crear(hash_resources);
	heap_t* heap = heap_crear(cmp_heap);
	int i=0;
	while(!hash_iter_al_final(iter) && i<n){
		recurso_t* recurso = malloc(sizeof(recurso_t*));
		recurso->nombre = hash_iter_ver_actual(iter);
		recurso->cant = *(int*)hash_obtener(hash_resources,recurso->nombre);
		heap_encolar(heap,recurso);
		hash_iter_avanzar(iter);
		i++;
	}
	while(!hash_iter_al_final(iter)){
		recurso_t* recurso = malloc(sizeof(recurso_t*));
		recurso->nombre = hash_iter_ver_actual(iter);
		recurso->cant = *(int*)hash_obtener(hash_resources,recurso->nombre);
		recurso_t* aux = heap_ver_max(heap);
		if(recurso->cant > aux->cant){ //es ver_min
			recurso_t* aux = heap_desencolar(heap);
			free(aux);
			heap_encolar(heap,recurso);
		}else
			free(recurso);

		hash_iter_avanzar(iter);
	}
	printf("Sitios más visitados:");
	}
	heap_destruir(heap,free); //es un simple free puesto que no quiero liberar la cadena
	printf("OK");
}
/* *****************************************************************
 *               	     FUNCION PRINCIPAL
 * *****************************************************************/
int main(int argc, char* argv[]){
    size_t buffersize = BUFFERSIZE;
    char* buffer  = malloc(sizeof(char) * buffersize);
    if(!buffer)
        return 0;

	hash_t* hash_resources = hash_crear(free);
	abb_t* abb_ip = abb_crear(cmp_ip,hash_destruir);

    while(getline(&buffer, &buffersize, stdin) > 0){
		char** strv = split(buffer,' ');
		if(verificar_comando(strv,2, "AGREGAR"))	agregar_archivo(abb_ip, hash_resources, strv[1]);
		if(verificar_comando(strv,3, "VISITANTES"))	ver_visitantes(abb_ip,strv[1],strv[2]);
		if(verificar_comando(strv,2, "VISITADOS") && es_numero(strv[1])) {
			int n = (int) strtol(strv[1], NULL, 10);
			ver_mas_visitados(hash_resources, n);
		}
		fprintf(stderr,"Error en comando %s\n",strv[0]);
        free(buffer);
		free_strv(strv);
		break;
	}
	hash_destruir(hash_resources);
	abb_destruir(abb_ip);
	return 0;
}