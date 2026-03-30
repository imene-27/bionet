/*
 * importador.c
 *
 *  Created on: 29 mar 2026
 *      Author: nahia.epelde
 */


#include "importador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"


void importar_farmacias(const char* nombre_archivo){
	FILE *f = fopen(nombre_archivo, "r");
	if(f = NULL){
		printf("Error: No se encuentra el archivo %s\n", nombre_archivo);
		return;
	}

	sqlite3 *db;
	if(sqlite3_open("bionet.db", &db) != SQLITE_OK){
		printf("Error al abrir la BD para importar el archivo\n");
		flcose(f);
		return;
	}

	char linea[1024];
	int contador = 0;

	//Vamos leyendo el archivo linea a linea
	while (fgets(linea, sizeof(linea), f)){
		//Para quitar el salto de linea del final
		linea[strcspn(linea, "\n")] = 0;

		//Separamos por punto y coma
		char *id = strtok(linea, ";");
		char *nom = strtok(NULL, ";");
		char *dir = strtok(NULL, ";");
		char *cp = strtok(NULL, ";");
		char *guardia = strtok(NULL, ";");

		if(id && nom && dir && cp && guardia){
			char sql[1024];

			//Creamos la sentencia sql
			sprintf(sql, "INSERT INTO Farmacia(ID, Nombre, Direccion, CP_FK; Guardia) "
						  "VALUES (%s, %s, %s, %s, %s);",
						  id, nom, dir, cp, guardia);

			char *errMsg = 0;
			int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
			if(rc != SQLITE_OK){
				sqlite3_free(errMsg);
			} else {
				contador++;
			}
		}

		printf("--- Importación finalizada: %d farmacias añadidas ---\n", contador);

		fclose(f);
		sqlite3_close(db);
	}
}
