#ifndef ANALOG_H
#define ANALOG_H

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
#include "cola.h"

struct recurso;
struct solicitud;

bool analizar_dos(abb_t* abb_ip, char** strv);

bool analizar_resources(hash_t* hash_resources, char** strv);

void ver_dos(abb_t* abb_ip);

void ver_visitantes(abb_t* abb_ip, char* inicio, char* final);

void ver_mas_visitados(hash_t* hash_resources, int n);

void agregar_archivo(abb_t* abb_ip, hash_t* hash_reesources, char* file);


#endif