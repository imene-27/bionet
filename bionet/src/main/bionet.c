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
#include "../db_manager/entidades.h"
#include "../db_manager/db_manager.h"
#include "../importador/importador.h"


int main() {
	int opcion = -1;
	int auxiliar_int

	//Inicializamos la base de datos
	inicializar_db();

	//Importamos todos los csvs
	importar_farmacias("farmacias.csv");
	importar_centros_salud("centros.csv");
	importar_medicos("medicos.csv");
	importar_stock("stock.csv");

	//Menu
	while(opcion !=0){
		printf("==================================\n");
		printf(" 	SERVIDOR BIONET		\n");
		printf("==================================\n");
		printf("1. Buscar farmacias por CP\n");
		printf("2. Ver Medicos de un Centro\n");
		printf("3. Consultar Stock de Farmacia");
		printf("0. Salir y Cerrar sServidor\n")
		printf("----------------------------------\n");
		printf("Seleccione una opcion. ");
		scanf("%d", opcion);

		switch(opcion){
			case 1:
				printf("Introduzca el Codigo Postal: ")
			case 2:
			case 3:
			case 0:
			default:
		}
	}

	return 0;
}
