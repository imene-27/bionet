/*
 ============================================================================
 Name        : bionet.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include "entidades.h"

int main() {
	printf("--- BIENVENIDO A BIONET (SERVIDOR) ---\n");

	inicializar_db();
	printf("Servidor listo para recibir comandos");

	return 0;
}

