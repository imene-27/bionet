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
#include "../sqlite3/sqlite3.h"

void importar_farmacias(const char* fichero) {
	FILE *f = fopen(fichero, "r");
	if (f == NULL) {
		printf("Error: No se encuentra el archivo %s\n", fichero);
		return;
	}

	sqlite3 *db;
	sqlite3_open("bionet.db", &db);
	char linea[1024];
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

		printf("DEBUG -> ID: %s | Nom: %s | Dir: %s | CP: %s | Mun: %s | Gua: %s | Tel: %s\n",
				id, nom, dir, cp, municipio, guardia, tel);

		if (id && nom && dir && cp && municipio && guardia && tel) {
			char sql[1024];
			sprintf(sql, "INSERT INTO Farmacia(ID, Nombre, Direccion, CP, Municipio, Guardia, Telefono) "
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
		printf("Error: No se encuentra el archivo %s\n", fichero);
		return;
	}

	sqlite3 *db;
	sqlite3_open("bionet.db", &db);
	char linea[1024];

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

		if (id && nombre && dir && cp && municipio && horario && tipoCentro && tel) {
			char sql[1024];
			sprintf(sql, "INSERT INTO CentroSalud (ID, Nombre, Direccion, CP, Municipio, Horario, TipoCentro, Telefono) "
					"		VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');",
					id, nombre, dir, cp, municipio, horario, tipoCentro, tel);
			sqlite3_exec(db, sql, 0, 0, 0);
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
	sqlite3_open("bionet.db", &db);
	char linea[1024];

	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\n")] = 0;
		linea[strcspn(linea, "\r")] = 0;

		char *id = strtok(linea, ";");
		char *nombre = strtok(NULL, ";");
		char *especialidad = strtok(NULL, ";");
		char *id_centro = strtok(NULL, ";");

		if (id && nombre && especialidad && id_centro) {
			char sql[1024];
			sprintf(sql, "INSERT INTO Doctor (ID, Nombre, Especialidad, ID_Centro) VALUES ('%s', '%s', '%s', '%s');",
					id, nombre, especialidad, id_centro);
			sqlite3_exec(db, sql, 0, 0, 0);
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
	sqlite3_open("bionet.db", &db);
	char linea[1024];

	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\n")] = 0;
		linea[strcspn(linea, "\r")] = 0;

		char *id_farma = strtok(linea, ";");
		char *id_medicamento = strtok(NULL, ";");
		char *cantidad = strtok(NULL, ";");


		if (id_farma && id_medicamento && cantidad) {
			char sql[1024];
			sprintf(sql, "INSERT INTO Vende (ID_Farmacia, ID_Medic, Cantidad) VALUES (%s, %s, %s);",
					id_farma, id_medicamento, cantidad);
			sqlite3_exec(db, sql, 0, 0, 0);
		}
	}
	fclose(f);
	sqlite3_close(db);
	printf("Inventario de medicamentos cargado correctamente\n");
}
