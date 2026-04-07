/*
 * importador.c
 *
 *  Created on: 30 mar 2026
 *      Author: nahia.epelde
 */

#include "importador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constantes.h"
#include "../sqlite3/sqlite3.h"

void importar_farmacias(const char* fichero) {
	FILE *f = fopen(fichero, "r");
	if (f == NULL) {
		printf("[!] Error: No se encuentra el archivo %s\n", fichero);
		return;
	}

	sqlite3 *db;
	sqlite3_open(DB_NAME, &db);
	char linea[MAX_LINEA];
	int contador = 0;

	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\n")] = 0;
		linea[strcspn(linea, "\r")] = 0;

		char *id = strtok(linea, ";");
		char *nom = strtok(NULL, ";");
		char *dir = strtok(NULL, ";");
		char *cp = strtok(NULL, ";");
		char *municipio = strtok(NULL, ";");
		char *guardia = strtok(NULL, ";");
		char *tel = strtok(NULL, ";");

		//printf("DEBUG -> ID: %s | Nom: %s | Dir: %s | CP: %s | Mun: %s | Gua: %s | Tel: %s\n",
			//	id, nom, dir, cp, municipio, guardia, tel);

		if (id && nom && dir && cp && municipio && guardia && tel) {
			char sql[MAX_SQL];
			sprintf(sql, "INSERT OR REPLACE INTO Farmacia(ID, Nombre, Direccion, CP, Municipio, Guardia, Telefono) "
						  "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s');",
						  id, nom, dir, cp, municipio, guardia, tel);

			char *errMsg = 0;
			int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
			if (rc != SQLITE_OK) {
				printf("ERROR AL IMPORTAR: %s\n", errMsg);
				sqlite3_free(errMsg);
			} else {
				contador++;
			}
		}
	} // Cierra el while

	printf("--- Importación finalizada: %d farmacias añadidas ---\n", contador);
	fclose(f);
	sqlite3_close(db);
} // Cierra la función importar_farmacias

void importar_centros_salud(const char* fichero) {
	FILE *f = fopen(fichero, "r");
	if (f == NULL) {
		printf("[!] Error: No se encuentra el archivo %s\n", fichero);
		return;
	}

	sqlite3 *db;
	sqlite3_open(DB_NAME, &db);
	char linea[MAX_LINEA];

	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\n")] = 0;
		linea[strcspn(linea, "\r")] = 0;

		char *id = strtok(linea, ";");
		char *nombre = strtok(NULL, ";");
		char *dir = strtok(NULL, ";");
		char *cp = strtok(NULL, ";");
		char *municipio = strtok(NULL, ";");
		char *horario = strtok(NULL, ";");
		char *tipoCentro = strtok(NULL, ";");
		char *tel = strtok(NULL, ";");

		//printf("DEBUG CENTROS -> ID: %s | Nom: %s | Dir: %s | CP: %s | Mun: %s | Hor: %s | Tipo: %s | Tel: %s\n",
		               // id, nombre, dir, cp, municipio, horario, tipoCentro, tel);

		if (id && nombre && dir && cp && municipio && horario && tipoCentro && tel) {
			char sql[MAX_SQL];
			sprintf(sql, "INSERT OR REPLACE INTO CentroSalud (ID, Nombre, Direccion, CP, Municipio, Horario, TipoCentro, Telefono) "
					"		VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');",
					id, nombre, dir, cp, municipio, horario, tipoCentro, tel);
			char *errMsg = 0;
			int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
			if (rc != SQLITE_OK) {
				printf("ERROR AL IMPORTAR: %s\n", errMsg);
				sqlite3_free(errMsg);
			}
		}
	}
	fclose(f);
	sqlite3_close(db);
	printf("Centros de salud cargados\n");
}

void importar_medicos(const char* fichero) {
	FILE *f = fopen(fichero, "r");
	if (f == NULL) {
		printf("Error: No se encuentra el archivo %s\n", fichero);
		return;
	}

	sqlite3 *db;
	sqlite3_open(DB_NAME, &db);
	char linea[MAX_LINEA];

	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\n")] = 0;
		linea[strcspn(linea, "\r")] = 0;

		char *id = strtok(linea, ";");
		char *nombre = strtok(NULL, ";");
		char *especialidad = strtok(NULL, ";");
		char *id_centro = strtok(NULL, ";");

		if (id && nombre && especialidad && id_centro) {
			char sql[MAX_SQL];
			sprintf(sql, "INSERT OR REPLACE INTO Doctor (ID, Nombre, Especialidad, ID_Centro) VALUES ('%s', '%s', '%s', '%s');",
					id, nombre, especialidad, id_centro);

			char *errMsg = 0;
			int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
			if(rc != SQLITE_OK){
				printf("ERROR Fallo al insertar medico");
				sqlite3_free(errMsg);
			}
		}
	}

	fclose(f);
	sqlite3_close(db);
	printf("Plantilla de medicos cargada\n");
}

void importar_stock(const char* fichero) {
	FILE *f = fopen(fichero, "r");
	if (f == NULL) {
		printf("Error: No se encuentra el archivo %s\n", fichero);
		return;
	}

	sqlite3 *db;
	sqlite3_open(DB_NAME, &db);
	char linea[MAX_LINEA];

	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\n")] = 0;
		linea[strcspn(linea, "\r")] = 0;

		char *id = strtok(linea, ";");
		char *id_farma = strtok(NULL, ";");
		char *nombre = strtok(NULL, ";");
		char *tipo = strtok(NULL, ";");
		char *precio = strtok(NULL, ";");
		char *cantidad = strtok(NULL, ";");


		if (id && id_farma && nombre && tipo && precio && cantidad) {
			char sql[1024];
			char *err_msg = 0;

			sprintf(sql, "INSERT INTO Stock (ID, ID_Farmacia, Nombre, Tipo, Precio, Cantidad)"
					"VALUES (%s, %s, '%s', '%s', %s, %s);",
					id, id_farma, nombre, tipo, precio, cantidad);
			int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
			if(rc != SQLITE_OK){
				printf("ERROR al importar %s: %s\n", nombre, err_msg);
				sqlite3_free(err_msg);
			}
		}
	}
	fclose(f);
	sqlite3_close(db);
	printf("Inventario de medicamentos cargado correctamente\n");

}


