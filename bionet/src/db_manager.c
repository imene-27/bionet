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

	if(rc != SQLITE_OK){
		fprintf(stderr, "Error al abrir DB: %S\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}
}
