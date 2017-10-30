
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "strutil.h"

/*
 * Devuelve en un arreglo dinámico terminado en NULL con todos los subsegmentos
 * de ‘str’ separados por el carácter ‘sep’. Tanto el arreglo devuelto como las
 * cadenas que contiene son allocadas dinámicamente.
 *
 * Quien llama a la función toma responsabilidad de la memoria dinámica del
 * arreglo devuelto. La función devuelve NULL si falló alguna llamada a
 * malloc(), o si ‘sep’ es '\0'.
 */
int contar_palabras(const char* str, char sep){
    size_t len = strlen(str);
    int cant_palabras = 1;
    for(int i=0;i<len;i++){
        if (sep == str[i]){
            cant_palabras++;
        }
    }
    cant_palabras++;
    return cant_palabras;
}
char* eliminar_salto_linea(const char*cadena){
    if(!cadena) return NULL;
    size_t len = strlen(cadena);
    size_t i;

    if(!len) len++;

    if(cadena[len-1]!='\n') len++;
    char* cadena_nueva = malloc(sizeof(char)*len);
    if(!cadena_nueva) return NULL;

    for(i=0;i<len-1;i++)
        cadena_nueva[i]=cadena[i];

    cadena_nueva[i]='\0';
    return cadena_nueva;
}

char** split(const char* str_inicial, char sep){
    if (sep=='\0'||!str_inicial) return NULL;

    char* str = eliminar_salto_linea(str_inicial);
    int cant_palabras = contar_palabras(str,sep);
    size_t len = strlen(str);
    int len_palabra=0;
    int j=0;
    int i;
    char * palabra ;

    char **strv = malloc(sizeof(char*) * cant_palabras);
    if (!strv)  return NULL;


    for(i=0;i<len;i++){
        if (str[i]==sep){
            palabra = malloc(sizeof(char*)*len_palabra+1);
            if (!palabra){
                strv[j]=NULL;
                free_strv(strv);
                return NULL;
            }
            strncpy(palabra,str+i-len_palabra,len_palabra);
            palabra[len_palabra]='\0';
            strv[j]=palabra;
            j++;
            len_palabra=0;
        }
        else
            len_palabra++;
    }

    palabra = malloc(sizeof(char*)*len_palabra+1);
    strncpy(palabra,str+i-len_palabra,len_palabra);
    palabra[len_palabra]='\0';
    strv[j]=palabra;
    strv[j+1]=NULL;
    free(str);

    return strv;
}

/*
 * Libera un arreglo dinámico de cadenas, y todas las cadenas que contiene.
 */
void free_strv(char* strv[]){
    int i = 0;
    while(strv[i]!=NULL){
        free(strv[i]);
        i++;
    }
    free(strv);
}

