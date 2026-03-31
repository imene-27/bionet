/*
 * db_manager.c
 *
 *  Created on: 28 mar 2026
 *      Author: nahia.epelde
 */


#include "../sqlite3/sqlite3.h"
#include <stdio.h>
#include "db_manager.h"


void inicializar_db() {
	sqlite3 *db;
	char *err_msg = 0;

	//Abrimos o creamos el archivo de la bbdd
	int rc = sqlite3_open("bionet.db", &db);

	//Comprobamos si se ha abierto o creado correctamente
	if(rc != SQLITE_OK){
		fprintf(stderr, "Error al abrir DB: %S\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	//Sentencia SQL para crear todas las tablas
	char *sql = "CREATE TABLE IF NOT EXISTS Usuario(DNI TEXT PRIMARY KEY, Nombre TEXT, Email TEXT, Pass TEXT Tipo TEXT);"
				"CREATE TABLE IF NOT EXISTS Farmacia(ID INTEGER PRIMARY KEY, Nombre TEXT, Direccion TEXT, CP INTEGER, Guardia INTEGER);"
				"CREATE TABLE IF NOT EXISTS CentroSalud(ID INTEGER PRIMARY KEY, Nombre TEXT, Direccion TEXT, CP INTEGER, Urgencias INTEGER);"
				"CREATE TABLE IF NOT EXISTS Medicamento(ID INTEGER PRIMARY KEY, Nombre TEXT, Unidades INTEGER);"
				"CREATE TABLE IF NOT EXISTS Doctor(ID INTEGER PRIMARY KEY, Nombre TEXT, Especialidad TEXT, ID_Centro INTEGER);"
				"CREATE TABLE IF NOT EXISTS Cita(ID INTEGER PRIMARY KEY, DNI_User TEXT, ID_Doctor INTEGER, Fecha TEXT, Hora TEXT);"
				"CREATE TABLE IF NOT EXISTS Tratamiento(ID INTEGER PRIMARY KEY, ID_Medicamento_FK INTEGER, Dosis TEXT);"
				"CREATE TABLE IF NOT EXISTS FichaMedica(DNI_FK TEXT PRIMARY KEY, Enfermedades TEXT, Operaciones TEXT, ID_Tratamiento_FK INTEGER);"
				"CREATE TABLE IF NOT EXISTS Cita(ID INTEGER PRIMARY KEY, DNI_User_FK TEXT, ID_Doctor_FK INTEGER, Fecha TEXT, Hora TEXT);";

	//Ejecutar la sentencia sql
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if(rc != SQLITE_OK) {
		fprintf(stderr, "Error de SQL: %s\n", err_msg);
		sqlite3_free(err_msg);
	} else {
		printf("Sistema de archivos BioNet: Base de datos cargada correctamente\n");
	}

	//Cerrar la base de datos
	sqlite3_close(db);
}


void buscar_farmacias_por_cp(int cp_buscado){
	sqlite3 *db;
	sqlite3_stmt *res;

	//Abrimos la bbdd
	if(sqlite3_open("bionet.db", &db) != SQLITE_OK){
		printf("Error al abrir la base de datos");
		return;
	}

	//Creamos la sentencia sql
	char *sql = "SELECT Nombre, Direccion, Guardia FROM Farmacia WHERE CP_FK = ?;";

	if(sqlite3_prepare16_v2(db, sql, -1, &res, 0) == SQLITE_OK){
		sqlite3_bind_int(res, 1, cp_buscado);

		printf("--- RESULTADOS PARA EL CP %d ---\n", cp_buscado);
		int filas = 0;

		while(sqlite3_step(res) == SQLITE_ROW){
			char *nombre = sqlite3_column_text(res, 0);
			char *dir = sqlite3_column_text(res, 1);
			int guardia = sqlite3_column_int(res, 2);

			if(guardia == 1){
				printf("[GUARDIA 24h] %s - %s\n", nombre, dir);
			} else {
				printf("[NO GUARDIA] %s - %s\n", nombre, dir);
			}
			filas++;
		}

		if(filas == 0){
			printf("No hay farmacias registradas en este CP\n");

		}
	} else {
		printf("Error en la consulta SQL: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
}


void buscar_medicos_por_centro(int id_centro){
	sqlite3 *db;
	sqlite3_stmt *res;
	sqlite3_open("bionet.db", &db);

	char *sql = "SELECT Nombre, Apellido, Especialidad FROM Medico WHERE ID_Centro_FK = ?;";

	if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
		sqlite3_bind_int(res, 1, id_centro);

		printf("--- MEDICOS DEL CENTRO %d ---\n", id_centro);
		int encontrados = 0;

		while(sqlite3_step(res) == SQLITE_ROW){
			char *nombre = sqlite3_column_text(res, 0);
			char *apellido = sqlite3_column_text(res, 1);
			char *especialidad sqlite3_column_text(res, 2);

			printf("- %s %s [%s]\n", nombre, apellido, especialidad);
			encontrados++;
		}

		if(encontrados == 0){
			printf("No hay medicos en este centro\n");
		}
		sqlite3_finalize(res);
		sqlite3_close(db);
	}
}


void ver_stock_farmacia(int id_farmacia){
	sqlite3 *db;
	sqlite3_stmt *res;
	sqlite3_open("bionet.db", &db);

	char *sql = "SELECT Nombre_Medicamento, Cantidad FROM Stock WHERE ID_Farmacia_FK = ?;";

	if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
		sqlite3_bind_int(res, 1, id_farmacia);

		printf("--- INVENTARIO FARMACIA %d ---\n", id_farmacia);
		while(sqlite3_step(res) == SQLITE_ROW){
			char *medicamento = sqlite_column_text(res, 0);
			int cant = sqlite_column_int(res, 1);

			if(cant > 0){
				printf(" > %s: %d unidades\n", medicamento, cant);
			} else {
				printf(" > %s: AGOTADO\n", medicamento);
			}
		}
	}
	sqlite3_finalize(res);
	sqlite3_close(db);
}
