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
	char *sql = "CREATE TABLE IF NOT EXISTS Usuario(DNI TEXT PRIMARY KEY, Nombre TEXT, Email TEXT, Pass TEXT Tipo TEXT, Historial_Breve TEXT, Municipio TEXT);"
				"CREATE TABLE IF NOT EXISTS Farmacia(ID INTEGER PRIMARY KEY, Nombre TEXT, Direccion TEXT, CP INTEGER, Municipio TEXT, Guardia INTEGER, Telefono INTEGER);"
				"CREATE TABLE IF NOT EXISTS CentroSalud(ID INTEGER PRIMARY KEY, Nombre TEXT, Direccion TEXT, CP INTEGER, Municipio TEXT, Horario TEXT, Tipo TEXT, Telefono INTEGER);"
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


void buscar_farmacias(char *busqueda){
	sqlite3 *db;
	sqlite3_stmt *res;
	char *sql;
	int es_numero = 1;

	//Comprobamosn si la busqueda es un numero (CP) o text (Municipio)
	for (int i=0; busqueda[i] != '\0'; i++){
		if(!isdigit(busqueda[i])){
			es_numero = 0;
			break;
		}
	}

	sqlite3_open("bionet.db", &db);

	if(es_numero == 1){
		//Si es un numero (CP)
		sql = "SELECT Nombre, Direccion, Guardia FROM Farmacia WHERE CP_FK = ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_int(res, 1, atoi(busqueda));
	} else {
		//Si es un texto (nombre de un municipio)
		sql = "SELECT Nombre, Direccion, Guardia FROM Farmacia WHERE Municipio = ?";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_int(res, 1, busqueda, -1, SQLITE_STATIC);
	}

	//Imprimimos los resultados
	printf("--- RESULTADOS PARA: %s ---\n", busqueda);
	int encontradas = 0;
	while(sqlite3_step(res) == SQLITE_ROW){
		char *nombre = sqlite3_column_text(res, 0);
		char *dir = sqlite3_column_text(res, 1);
		int guardia = sqlite3_column_text(res, 2);

		if(guardia == 1){
			printf("[GUARDIA]    %s - %s\n", nombre, dir);
		} else {
			printf("[NO GUARDIA]    %s - %s\n", nombre, dir);
		}
		encontradas++;
	}

	if(encontradas == 0){
		printf("No se han encontrado farmacias\n");
	}
	sqlite3_finalize(res);
	sqlite3_close(db);
}


void buscar_centros(char *busqueda){
	sqlite3 *db;
	sqlite3_stmt *res;
	char *sql;
	int es_numero = 1;

	//Comprobamosn si la busqueda es un numero (CP) o text (Municipio)
	for (int i=0; busqueda[i] != '\0'; i++){
		if(!isdigit(busqueda[i])){
			es_numero = 0;
			break;
		}
	}

	sqlite3_open("bionet.db", &db);

	if(es_numero == 1){
		//Busqueda por CP
		sql = "SELECT Nombre, Direccion, Municipio FROM CentroSalud WHERE CP_FK = ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_int(res, 1, atoi(busqueda));
	} else {
		//Busqueda por nombre de municipio
		sql = "SELECT Nombre, Direccion, Municipio FROM CentroSalud WHERE Municipio = ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_text(res, 1, busqueda, -1, SQLITE_STATIC);
	}

	printf("--- CENTROS DE SALUD EN: %s ---\n", busqueda);
	int encontrados = 0;
	while(sqlite3_step(res) == SQLITE_ROW){
		char *nombre = sqlite3_column_text(res, 0);
		char *dir = sqlite3_column_text(res, 1);
		char *municipio = sqlite3_column_text(res, 2);

		printf("ID: %d | %s - %s (%s)\n", encontrados + 1, nombre, dir, municipio);
		encontrados++;
	}

	if(encontrados == 0){
		printf("No se han encontrado centros de salud en esta zona\n");
	}
	sqlite3_finalize(res);
	sqlite_close(db);
}

void buscar_medicamento(char *nombre_med, char *localidad){
	sqlite3 *db;
	sqlite3_stmt *res;
	char *sql;
	int es_numero = 1;

	//Comprobamosn si la busqueda es un numero (CP) o text (Municipio)
	for (int i=0; localidad[i] != '\0'; i++){
		if(!isdigit(localidad[i])){
			es_numero = 0;
			break;
		}
	}

	sqlite3_open("bionet.db", &db);

	if(es_numero == 1){
		//Busqueda del medicamento por CP de farmacia
		sql = "SELECT f.Nombre, s.Cantidad, FROM Farmacia f "
			   "JOIN Stock s ON f.ID_Farmacia = s.Id_Farmacia_FK "
				"WHERE s.Nombre_Medicamento = ? AND f.CP_FK = ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_text(res, 1, nombre_med, -1, SQLITE_STATIC);
		sqlite_bind_int(res, 2, atoi(localidad));
	} else {
		sql = "SELECT f.Nombre, s.Cantidad, FROM Farmacia f "
		      "JOIN Stock s ON f.ID_Farmacia = s.Id_Farmacia_FK "
			   "WHERE s.Nombre_Medicamento = ? AND f.Municipio = ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_text(res, 1, nombre_med, -1, SQLITE_STATIC);
		sqlite_bind_text(res, 2, localidad, -1, SQLITE_STATIC);


		PRINTF("--- STOCK DE `%s´ EN `%s´ ---\n", nombre_med, localidad);
		int encontrados = 0;
		while(sqlite3_step(res) == SQLITE_ROW){
			char *farmacia = sqlite_column_text(res, 0);
			int cantidad = sqlite_column_int(res, 1);

			if(cantidad >0){
				printf("Farmacia: %-20s | Stock: %d unidades\n", farmacia, cantidad);
			} else {
				printf("Farmacia: %-20s | [AGOTAADO]\n", farmacia);
			}
			encontrados++;
		}

		if(encontrados == 0){
			printf("No se ha encontrado el medicamento en esta zona\n");
		}
	}
}



void ver_ficha_medica(char *dni_usuario){
	sqlite3 *db;
	sqlite3_stmt *res;
	sqlite3_open("bionet.db", &db);

	//Parte de los datos personales
	char *sql_user = "SELECT Nombre, Apellido, Municipio FROM Usuario WHERE DNI = ?;";
	if(sqlite3_prepare_v2(db, sql_user, -1, &res, 0) == SQLITE_OK){
		sqlite3_bind_text(res, 1, dni_usuario, -1, SQLITE_STATIC);

		if(sqlite3_step(res) == SQLITE_ROW){
			printf("============================================\n");
			printf("FICHA MEDICA: %s          \n", dni_usuario);
			printf("============================================\n");
			printf(" PACIENTE: %s %s\n", sqlite3_column_text(res, 0), sqlite3_column_text(res, 1));
			printf(" RESIDENCIA: %s\n", sqlite3_column_text(res, 2));
		} else{
			printf("[!] Usuario no encontrado\n");
			sqlite3_finalize(res);
			sqlite3_close(db);
			return;
		}
	}
	sqlite3_finalize(res);

	//Tratamientos activos del usuario
	printf("\n--- TRATAMIENTOS ACTIVOS ---\n");
	char *sql_tratamiento = "SELECT Nombre_Medicamento, Dosis FROM Tratamiento WHERE DNI_Usuario_FK = ?:";
	if (sqlite3_prepare_v2(db, sql_tratamiento, -1, &res, 0) == SQLITE_OK) {
		sqlite3_bind_text(res, 1, dni_usuario, -1, SQLITE_STATIC);
		int hay_tratamiento = 0;
		while (sqlite3_step(res) == SQLITE_ROW) {
			printf(" > %s (%s)\n", (char*)sqlite3_column_text(res, 0), (char*)sqlite3_column_text(res, 1));
			hay_tratamiento = 1;
		}
		if (!hay_tratamiento) {
			printf(" No hay tratamientos registrados.\n");
		}
	}
	sqlite3_finalize(res);


	//Citas programadas del usuario
	printf("\n--- CITAS PROGRAMADAS ---\n");
	char *sql_citas = "SELECT c.Fecha, c.Hora, m.Apellido, m.Especialidad "
					  "FROM Cita c JOIN Medico m ON c.ID_Medico_FK = m.ID_Medico "
					  "WHERE c.DNI_Usuario_FK = ? ORDER BY c.Fecha ASC;";

	if (sqlite3_prepare_v2(db, sql_citas, -1, &res, 0) == SQLITE_OK) {
		sqlite3_bind_text(res, 1, dni_usuario, -1, SQLITE_STATIC);
		int hay_citas = 0;
		while (sqlite3_step(res) == SQLITE_ROW) {
			printf(" > %s a las %s - Dr. %s (%s)\n",
				   (char*)sqlite3_column_text(res, 0), (char*)sqlite3_column_text(res, 1),
				   (char*)sqlite3_column_text(res, 2), (char*)sqlite3_column_text(res, 3));
			hay_citas = 1;
		}
		if (!hay_citas) printf(" No tiene citas pendientes.\n");
	}
	printf("============================================\n");

	sqlite3_finalize(res);
	sqlite3_close(db);
}


void buscar_medicos_especialidad(char *especialidad, char *localidad){
	sqlite3 *db;
	sqlite3_stmt *res;
	int es_numero = 1;

	for(int i=0; localidad[i] != `\0´; i++){
		if(!isdigit(localidad[i])){
			es_numero = 0;
			break;
		}
	}

	sqlite3_open("bionet.db", &db);
	char *sql;

	if(es_numero == 1){
		//Buscar medico con esa especialidad por CP
		sql = "SELECT m.ID_Medico, m.Nombre, m.Apellido, c.Nombre FROM Medico m "
	          "JOIN CentroSalud c ON m.ID_Centro_FK = c.ID_Centro "
	          "WHERE m.Especialidad = ? AND c.CP_FK = ?;";

		if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
			sqlite3_bind_text(res, 1, especialidad, -1, SQLITE_STATIC);
			sqlite3_bind_int(res, 2, atoi(localidad));
		}
	} else {
		//Buscar medico con esa especialidad por localidad
		sql = "SELECT m.ID_Medico, m.Nombre, m.Apellido, c.Nombre FROM Medico m "
		      "JOIN CentroSalud c ON m.ID_Centro_FK = c.ID_Centro "
		      "WHERE m.Especialidad = ? AND c.Municipio = ?;";

		if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
			sqlite3_bind_text(res, 1, especialidad, -1, SQLITE_STATIC);
			sqlite3_bind_text(res, 2, localidad, -1, SQLITE_STATIC);
		}
	}

	//iImprimimos resultados
	printf("\n--- MEDICOS DE %s EN %s ---\n", especialidad, localidad);
	int encontrados = 0;
	while(sqlite3_step(res) == SQLITE_ROW){
		int id = sqlite_column_int(res, 0);
		char *nombre = sqlite_column_text(res, 1);
		char *apellido = sqlite_column_text(res, 2);

		printf("ID: %d | Dr. %s %s (%s)\n", id, nombre, apellido, especialidad);

		encontrados++;
	}

	if(encontrados == 0){
		printf("No se han encontrado medicos con estos criterios\n");
	}
	sqlite3_finalize(res);
	sqlite3_close(db);
}


int comprobar_y_reservar(char *dni, int id_medico, char *fecha, char *hora){
	sqlite3 *db;
	sqlite3_stmt *res;
	int ocupado = 0;

	sqlite_open("bionet.db", &db);

	//Comprobamos si el medico esta libre ese dia a esa hora
	char *sql_compr = "SELECT COUNT(*) FROM Cita WHERE ID_Medico_FK = ? AND Fecha = ? AND Hora = ?;";
	sqlite3_prepare(db, sql_compr, -1, &res, 0);
	sqlite_bind_int(res, 1, id_medico);
	sqlite_bind_text(res, 2, fecha, -1, SQLITE_STATIC);
	sqlite_bind_text(res, 3, hora, -1, SQLITE_STATIC);

	if(sqlite3_step(res) == SQLITE_ROW){
		ocupado = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);

	//Si esta ocupado, salimos
	if(ocupado > 0){
		sqlite_close(db);
		return 0;
	}

	//Si esta libre, hacemos el insert en la tabla Cita
	char *sql_insert = "INSERT INTO Cita(DNI_Usuario_FK, ID_Medico_FK, Fecha, Hora) VALUES (?, ?, ?, ?);";
	sqlite3_prepare16_v2(db, sql_insert, -1, &res, 0);
	sqlite3_bind_text(res, 1, dni, -1, SQLITE_STATIC);
	sqlite3_bind_int(res, 2, id_medico);
	sqlite3_bind_text(res, 3, fecha, -1, SQLITE_STATIC);
	sqlite3_bind_text(res, 4, hora, -1, SQLITE_STATIC);

	sqlite3_step(res);
	sqlite3_finalize(res);
	sqlite3_close(db);

	return 1;
}
