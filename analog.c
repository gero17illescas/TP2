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


int cmp_heap(void* dato_1,void* dato_2){
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
time_t actualizar_fechas(time_t *primera_fecha,time_t *actual_fecha){
	struct tm* prim_fecha = gmtime(primera_fecha); //paso a estructura para restales los seg
	struct tm* act_fecha = gmtime(actual_fecha);
	struct tm* aux= malloc(sizeof(struct tm*));
	aux->tm_sec = act_fecha->tm_sec - (act_fecha->tm_sec - prim_fecha->tm_sec);
	aux->tm_min = act_fecha->tm_min - (act_fecha->tm_min - prim_fecha->tm_min);
	aux->tm_hour = act_fecha->tm_hour - (act_fecha->tm_hour - prim_fecha->tm_hour);
	aux->tm_mday = act_fecha->tm_mday - (act_fecha->tm_mday - prim_fecha->tm_mday);
	aux->tm_mon = act_fecha->tm_mon - (act_fecha->tm_mon - prim_fecha->tm_mon);
	aux->tm_year = act_fecha->tm_year - (act_fecha->tm_year - prim_fecha->tm_year);
	aux->tm_isdst = act_fecha->tm_isdst - (act_fecha->tm_isdst - prim_fecha->tm_isdst);
	time_t nueva_fecha = mktime(aux);
	free(aux);
	return nueva_fecha;
}
bool analizar_dos (abb_t* abb_ip,char** strv){
	char* ip = strv[0];	char* fecha= strv[1];
	int* c_solicitudes;
	hash_t* hash_ip = abb_obtener(abb_ip,ip);
	
	if(hash_ip){ //si hay una ip en el arbol la analizamos
		int* cant_dos = hash_obtener(hash_ip,"dos");
		if(!cant_dos)	return false;
		if(*cant_dos==1)	return true;	//si para esa ip ya hay dos no se hace nada, pues ya tuvo un ataque.

		time_t primera_fecha = iso8601_to_time(hash_obtener(hash_ip,"primera fecha"));
		time_t actual_fecha = iso8601_to_time(fecha);

		double dif_tiempo = difftime(primera_fecha,actual_fecha);

		c_solicitudes = hash_obtener(hash_ip,"solicitudes"); 
		(*c_solicitudes)++;

		if(dif_tiempo<2 && *c_solicitudes>4){	//vemos si en un intervalo menor a 2 hubo 5 solicitudes
			int dos = 1;
			if(!hash_guardar(hash_ip,"dos", &dos))	return false;
		}else{
			(*c_solicitudes)--;
			time_t nueva_fecha = actualizar_fechas(&primera_fecha,&actual_fecha);
			char* nueva_fecha_str = ctime(&nueva_fecha); //movemos el indicador de tiempo
			if(!hash_guardar(hash_ip,"primera fecha",nueva_fecha_str)) return false;
		}

		return hash_guardar(hash_ip,"dos",c_solicitudes);
	}
	//todo esto pasa si la ip no esta en el arbol

	c_solicitudes =  malloc(sizeof(int*));
	int* dos = malloc(sizeof(int*));

	*c_solicitudes=0;
	*dos=0;

	if(!hash_guardar(hash_ip,"solicitudes",c_solicitudes))	return false;
	if(!hash_guardar(hash_ip,"dos",dos))			return false;
	if(!hash_guardar(hash_ip,"primera fecha",fecha))	return false;

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
void ver_dos(abb_t* abb_ip){
	abb_iter_t* iter = abb_iter_in_crear(abb_ip);
	while(!abb_iter_in_al_final(iter)){
		const char* ip = abb_iter_in_ver_actual(iter);
		hash_t* hash_ip = abb_obtener(abb_ip,ip);
		int *cant_dos = hash_obtener(hash_ip,"dos");
		if(*cant_dos==1)
			printf("DoS: %s\n",ip);
		abb_iter_in_avanzar(iter);
	}
}
/* Recibe un archivo, verifica si lo puede abrir y devuelve una lista
 * de usuarios en memoria dinamica. Si hubo un problema con el arhcivo
 * devuelve NULL. Si el archivo existe y esta vacio devuelve una lista
 * vacia.
 */
void agregar_archivo(abb_t* abb_ip, hash_t* hash_resources, char* file){
	FILE* archivo = fopen(file,"r");
	if (archivo == NULL)
		return;

	char* linea = NULL; size_t capacidad = 0; //combo getline
	char** strv;
	bool ok; bool ok2;

	while(getline(&linea,&capacidad, archivo) > 0){
		strv = split(linea,'\t');
		ok = analizar_dos(abb_ip,strv);
		ok2= analizar_resources(hash_resources,strv);
        free_strv(strv);
	}
	free(linea);
	fclose(archivo);

	if(ok && ok2){
		ver_dos(abb_ip);
		printf("OK");
	}
}

void ver_visitantes(abb_t* abb_ip,char* inicio,char* final){
	abb_iter_t* iter = crear_iter_inorder_acotado(abb_ip,inicio,final);
	printf("Visitantes:");
	while(!abb_iter_in_al_final(iter)){
		printf("\t%s",abb_iter_in_ver_actual(iter));
	}
	printf("OK");
	abb_iter_in_destruir(iter);
}

void ver_mas_visitados(hash_t* hash_resources,int n){
	
	hash_iter_t* iter = hash_iter_crear(hash_resources);
	heap_t* heap = heap_crear((cmp_func_t) cmp_heap);
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
			aux = heap_desencolar(heap);
			free(aux);
			heap_encolar(heap,recurso);
		}else
			free(recurso);

		hash_iter_avanzar(iter);
	}
	printf("Sitios más visitados:");
	
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
	abb_t* abb_ip = abb_crear(cmp_ip,(abb_destruir_dato_t) hash_destruir);

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