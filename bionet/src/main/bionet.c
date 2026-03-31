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
#include "importador.h"
#include "db_manager.h"


int main() {
	printf("--- BIENVENIDO A BIONET (SERVIDOR) ---\n");

	inicializar_db();
	printf("Servidor listo para recibir comandos\n");

	importar_farmacias("farmacias.csv");
	importar_centros_salud("centros.csv");
	importar_medicos("medicos.csv");
	importar_stock("stock.csv");

	return 0;
}
