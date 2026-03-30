/*
 * db_manager.c
 *
 *  Created on: 28 mar 2026
 *      Author: nahia.epelde
 */


#include "sqlite3.h"
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
