#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "strutil.h"


int cmp_ip(char* ip1,char* ip2){
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
int main(){
	char* ip_1 = "0.0.0.0";
	char* ip_2 = "0.0.0.1";
	char* ip_3 = "0.0.1.0";
	char* ip_4 = "0.1.0.0";
	char* ip_5 = "1.0.0.0";
	char* ip_6 = "255.0.0.0";
	char* ip_7 = "0.0.0.255";
	char* ip_8 = "192.160.0.255";
	char* ip_9 = "192.165.0.255";

	printf("Debe ser  0: %d\n",cmp_ip(ip_1,ip_1));
	printf("Debe ser  1: %d\n",cmp_ip(ip_2,ip_1));
	printf("Debe ser  1: %d\n",cmp_ip(ip_3,ip_2));
	printf("Debe ser  1: %d\n",cmp_ip(ip_4,ip_3));
	printf("Debe ser  1: %d\n",cmp_ip(ip_5,ip_4));
	printf("Debe ser  1: %d\n",cmp_ip(ip_6,ip_5));
	printf("Debe ser  1: %d\n",cmp_ip(ip_6,ip_7));
	printf("Debe ser  1: %d\n",cmp_ip(ip_6,ip_8));
	printf("Debe ser  1: %d\n",cmp_ip(ip_9,ip_8));

	return 0;
}