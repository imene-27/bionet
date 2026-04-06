/*
 * db_manager.c
 *
 *  Created on: 28 mar 2026
 *      Author: nahia.epelde
 */


#include "../sqlite3/sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include "db_manager.h"
#include "constantes.h"


void inicializar_db() {
	sqlite3 *db;
	char *err_msg = 0;

	//Abrimos o creamos el archivo de la bbdd
	int rc = sqlite3_open("bionet.db", &db);

	//Comprobamos si se ha abierto o creado correctamente
	if(rc != SQLITE_OK){
		fprintf(stderr, "Error al abrir DB: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	//Sentencia SQL para crear todas las tablas
	char *sql = "CREATE TABLE IF NOT EXISTS Usuario(DNI TEXT PRIMARY KEY, "
			"										Nombre TEXT, "
			"										Email TEXT, "
			"										Pass TEXT, "
			"										Tipo TEXT, "
			"										Historial_Breve TEXT, "
			"										Municipio TEXT);"


				"CREATE TABLE IF NOT EXISTS Farmacia(ID INTEGER PRIMARY KEY, "
				"									 Nombre TEXT, "
				"									 Direccion TEXT, "
				"									 CP INTEGER, "
				"									 Municipio TEXT, "
				"									 Guardia INTEGER, "
				"  									 Telefono TEXT);"


				"CREATE TABLE IF NOT EXISTS CentroSalud(ID INTEGER PRIMARY KEY, "
				"										Nombre TEXT, "
				"										Direccion TEXT, "
				"										CP INTEGER, "
				"										Municipio TEXT, "
				"										Horario TEXT, "
				"										TipoCentro TEXT, "
				"										Telefono TEXT);"


				"CREATE TABLE IF NOT EXISTS Medicamento(ID INTEGER PRIMARY KEY, "
				"    									ID_FARMA INTEGER, "
				"		                                Nombre TEXT, "
				"                                       Tipo TEXT, "
				"                                       Precio DOUBLE, "
				"                     					Unidades INTEGER) REFERENCES Farmacia(ID));"


				"CREATE TABLE IF NOT EXISTS Doctor(ID INTEGER PRIMARY KEY, "
				"								   Nombre TEXT, "
				"								   Especialidad TEXT, "
				"								   ID_Centro INTEGER, "
				"								   FOREIGN KEY (ID_Centro) REFERENCES CentroSalud(ID));"


				"CREATE TABLE IF NOT EXISTS Cita(ID INTEGER PRIMARY KEY, "
				"								DNI_Usuario TEXT, "
				"								ID_Doctor INTEGER, "
				"								Fecha TEXT, "
				"								Hora TEXT, "
				"								FOREIGN KEY (DNI_Usuario) REFERENCES Usuario(DNI), "
				"								FOREIGN KEY (ID_Doctor) REFERENCES Doctor(ID));"

				"CREATE TABLE IF NOT EXISTS Tratamiento(ID INTEGER PRIMARY KEY, "
				"										Nombre TEXT, "
				"										ID_Medicamento INTEGER, "
				"										Dosis TEXT, "
				"										FOREIGN KEY (ID_Medicamento) REFERENCES Medicamento (ID));"


				"CREATE TABLE IF NOT EXISTS Vende(ID_Farmacia INTEGER, "
				"								  ID_Medic INTEGER, "
				"								  Cantidad INTEGER, "
				"								  PRIMARY KEY (ID_Farmacia, ID_Medic),"
				"								  FOREIGN KEY (Id_Farmacia) REFERENCES Farmacia(ID),"
				"								  FOREIGN KEY (ID_Medic) REFERENCES Medicamento(ID)); "

				"CREATE TABLE IF NOT EXISTS FichaMedica(DNI_User TEXT PRIMARY KEY, "
				"										Enfermedades TEXT, "
				"										Operaciones TEXT, "
				"										ID_Tratamiento INTEGER,"
				"										FOREIGN KEY (DNI_User) REFERENCES Usuario(DNI),"
				"										FOREIGN KEY(ID_Tratamiento) REFERENCES Tratamiento(ID));";


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
	int encontradas = 0;

	busqueda[strcspn(busqueda, "\n")] = 0;
	busqueda[strcspn(busqueda, "\r")] = 0;

	//Comprobamosn si la busqueda es un numero (CP) o text (Municipio)
	for (int i=0; busqueda[i] != '\0'; i++){
		if(!isdigit(busqueda[i])){
			es_numero = 0;
			break;
		}
	}
	if(sqlite3_open("bionet.db", &db) != SQLITE_OK){
		printf("ERROR: No se pudo abrir la BD\n");
		return;
	}


	if(es_numero == 1){
		//Si es un numero (CP)
		sql = "SELECT Nombre, Direccion, Guardia FROM Farmacia WHERE CP LIKE '%' || ? || '%';";
		//sqlite3_prepare_v2(db, sql, -1, &res, 0);
		//sqlite3_bind_text(res, 1, busqueda, -1, SQLITE_STATIC);
	} else {
		//Si es un texto (nombre de un municipio)
		sql = "SELECT Nombre, Direccion, Guardia FROM Farmacia WHERE Municipio LIKE '%' || ? || '%';";
		//sqlite3_prepare_v2(db, sql, -1, &res, 0);
		//sqlite3_bind_text(res, 1, busqueda, -1, SQLITE_STATIC);
	}

	int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	if(rc != SQLITE_OK){
		printf("ERROR DE SQLITE AL BUSCAR : %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	sqlite3_bind_text(res, 1, busqueda, -1, SQLITE_STATIC);

	//Imprimimos los resultados
	printf("--- RESULTADOS PARA: %s ---\n", busqueda);

	while(sqlite3_step(res) == SQLITE_ROW){
		char *nombre = (char*)sqlite3_column_text(res, 0);
		char *dir =(char*)sqlite3_column_text(res, 1);
		int guardia = sqlite3_column_int(res, 2);

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
	int encontrados = 0;

	busqueda[strcspn(busqueda, "\n")] = 0;
	busqueda[strcspn(busqueda, "\r")] = 0;

	//Comprobamosn si la busqueda es un numero (CP) o text (Municipio)
	for (int i=0; busqueda[i] != '\0'; i++){
		if(!isdigit(busqueda[i])){
			es_numero = 0;
			break;
		}
	}

	if(sqlite3_open("bionet.db", &db) != SQLITE_OK){
		printf("ERROR: No se pudo abrir la BD");
		return;
	}
	char texto_busqueda[200];
	sprintf(texto_busqueda, "%%%s%%", busqueda);

	if(es_numero == 1){
		//Busqueda por CP
		sql = "SELECT Nombre, Direccion FROM CentroSalud WHERE CP LIKE ?;";
		//sqlite3_prepare_v2(db, sql, -1, &res, 0);
		//sqlite3_bind_int(res, 1, atoi(busqueda));
	} else {
		//Busqueda por nombre de municipio
		sql = "SELECT Nombre, Direccion FROM CentroSalud WHERE Municipio LIKE ?;";
		//sqlite3_prepare_v2(db, sql, -1, &res, 0);
		//sqlite3_bind_text(res, 1, busqueda, -1, SQLITE_STATIC);
	}

	int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	if(rc!= SQLITE_OK){
		printf("ERROR DE SQLITE AL BUSCAR CENTROS: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}
	sqlite3_bind_text(res, 1, busqueda, -1, SQLITE_TRANSIENT);

	printf("--- CENTROS DE SALUD EN: %s ---\n", busqueda);

	while(sqlite3_step(res) == SQLITE_ROW){
		char *nombre = (char*)sqlite3_column_text(res, 0);
		char *dir = (char*)sqlite3_column_text(res, 1);


		printf("%s - %s\n", nombre, dir);
		encontrados++;
	}

	if(encontrados == 0){
		printf("No se han encontrado centros de salud en esta zona\n");
	}
	sqlite3_finalize(res);
	sqlite3_close(db);
}

void buscar_medicamento(char *nombre_med, char *localidad){
	sqlite3 *db;
	sqlite3_stmt *res;
	char *sql;
	int es_numero = 1;

	nombre_med[strcspn(nombre_med, "\n")] = 0;
	nombre_med[strcspn(nombre_med, "\r")] = 0;
	localidad[strcspn(localidad, "\n")] = 0;
	localidad[strcspn(localidad, "\r")] = 0;

	//Comprobamosn si la busqueda es un numero (CP) o text (Municipio)
	for (int i=0; localidad[i] != '\0'; i++){
		if(!isdigit((unsigned char)localidad[i])){
			es_numero = 0;
			break;
		}
	}

	if(sqlite3_open("bionet.db", &db) != SQLITE_OK){
		printf("Error al abrir la base de datos.\n");
		return;
	}

	char busqueda_med[250];
	sprintf(busqueda_med, "%%%s%%", nombre_med);

	char busqueda_loc[250];
	sprintf(busqueda_loc, "%%%s%%", localidad);

	if(es_numero == 1){
		//Busqueda del medicamento por CP de farmacia
		sql = "SELECT f.Nombre, m.Unidades FROM Farmacia f "
				"JOIN Medicamento m ON f.ID = m.ID_FARMA "
				"WHERE m.Nombre LIKE ? AND f.CP = ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_text(res, 1, busqueda_med, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(res, 2, atoi(localidad));

	} else {
		//Busqueda por municipio
		sql = "SELECT f.Nombre, m.Unidades FROM Farmacia f "
			   "JOIN Medicamento m ON f.ID = m.ID_FARMA "
			   "WHERE m.Nombre LIKE ? AND f.Municipio LIKE ?;";
		sqlite3_prepare_v2(db, sql, -1, &res, 0);
		sqlite3_bind_text(res, 1, busqueda_med, -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(res, 2, busqueda_loc, -1, SQLITE_TRANSIENT);

	}

	printf("\n--- STOCK DE '%s' EN '%s' ---\n", nombre_med, localidad);
	int encontrados = 0;
	while(sqlite3_step(res) == SQLITE_ROW){
		char *farmacia = (char*)sqlite3_column_text(res, 0);
		int cantidad = sqlite3_column_int(res, 1);

		if(cantidad >0){
			printf("Farmacia: %-20s | Stock: %d unidades\n", farmacia, cantidad);
		} else {
			printf("Farmacia: %-20s | [AGOTADO]\n", farmacia);
		}
		encontrados++;
	}

	if(encontrados == 0){
		printf("[!] No se han encontrado farmacias con ese medicamento en la zona indicada.\n");
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
}


void ver_ficha_medica(char *dni_usuario){
	sqlite3 *db;
	sqlite3_stmt *res;
	sqlite3_open(DB_NAME, &db);

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

			// Registro de LOG
			char info_log[MAX_INFO_LOG];
			sprintf(info_log, "Consulta de ficha medica completa por el usuario: %s", dni_usuario);
			registrar_log("[ACCESO PRIVADO]", info_log);


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
	char *sql_tratamiento = "SELECT Nombre, Dosis FROM Tratamiento WHERE DNI_Usuario = ?;";
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
	char *sql_citas = "SELECT c.Fecha, c.Hora, d.Nombre, d.Especialidad "
					  "FROM Cita c JOIN Doctor d ON c.ID_Doctor = d.ID "
					  "WHERE c.DNI_Usuario = ? ORDER BY c.Fecha ASC;";

	if (sqlite3_prepare_v2(db, sql_citas, -1, &res, 0) == SQLITE_OK) {
		sqlite3_bind_text(res, 1, dni_usuario, -1, SQLITE_STATIC);
		int hay_citas = 0;
		while (sqlite3_step(res) == SQLITE_ROW) {
			printf(" > %s a las %s - Dr. %s (%s)\n",
				   (char*)sqlite3_column_text(res, 0), (char*)sqlite3_column_text(res, 1),
				   (char*)sqlite3_column_text(res, 2), (char*)sqlite3_column_text(res, 3));
			hay_citas = 1;
		}
		if (!hay_citas) printf("No tiene citas pendientes.\n");
	}


	sqlite3_finalize(res);
	sqlite3_close(db);
}


int buscar_medicos_especialidad(char *especialidad, char *localidad){
	sqlite3 *db;
	sqlite3_stmt *res;
	int es_numero = 1;


	for(int i=0; localidad[i] != '\0'; i++){
		if(!isdigit((unsigned char)localidad[i])){
			es_numero = 0;
			break;
		}
	}

	sqlite3_open(DB_NAME, &db);
	char *sql;

	if(es_numero == 1){
		//Buscar medico con esa especialidad por CP
		sql = "SELECT d.ID, d.Nombre, c.Nombre FROM Doctor d "
	          "JOIN CentroSalud c ON d.ID_Centro = c.ID "
	          "WHERE d.Especialidad = ? AND c.CP = ?;";

		if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
			sqlite3_bind_text(res, 1, especialidad, -1, SQLITE_STATIC);
			sqlite3_bind_int(res, 2, atoi(localidad));
		}
	} else {
		//Buscar medico con esa especialidad por localidad
		sql = "SELECT d.ID, d.Nombre, c.Nombre FROM Doctor d "
		      "JOIN CentroSalud c ON d.ID_Centro = c.ID "
		      "WHERE d.Especialidad = ? AND c.Municipio = ?;";

		if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
			sqlite3_bind_text(res, 1, especialidad, -1, SQLITE_STATIC);
			sqlite3_bind_text(res, 2, localidad, -1, SQLITE_STATIC);
		}
	}

	//Imprimimos resultados
	printf("\n--- MÉDICOS DE %s EN %s ---\n", especialidad, localidad);
	int encontrados = 0;
	while(sqlite3_step(res) == SQLITE_ROW){
		int id = sqlite3_column_int(res, 0);
		char *nombre = (char*)sqlite3_column_text(res, 1);
		char *apellido = (char*)sqlite3_column_text(res, 2);

		printf("ID: %d | Dr. %s %s (%s)\n", id, nombre, apellido, especialidad);

		encontrados++;
	}

	if(encontrados == 0){
		printf("[!] No se han encontrado medicos con estos criterios\n");
	}
	sqlite3_finalize(res);
	sqlite3_close(db);

	return encontrados;
}


int comprobar_y_reservar(char *dni, int id_medico, char *fecha, char *hora){
	sqlite3 *db;
	sqlite3_stmt *res;
	int ocupado = 0;

	sqlite3_open("bionet.db", &db);

	//Comprobamos si el medico esta libre ese dia a esa hora
	char *sql_compr = "SELECT COUNT(*) FROM Cita WHERE ID_Doctor = ? AND Fecha = ? AND Hora = ?;";
	sqlite3_prepare(db, sql_compr, -1, &res, 0);
	sqlite3_bind_int(res, 1, id_medico);
	sqlite3_bind_text(res, 2, fecha, -1, SQLITE_STATIC);
	sqlite3_bind_text(res, 3, hora, -1, SQLITE_STATIC);

	if(sqlite3_step(res) == SQLITE_ROW){
		ocupado = sqlite3_column_int(res, 0);
	}
	sqlite3_finalize(res);

	//Si esta ocupado, salimos
	if(ocupado > 0){
		sqlite3_close(db);
		return 0;
	}

	//Si esta libre, hacemos el insert en la tabla Cita
	char *sql_insert = "INSERT INTO Cita(DNI_Usuario, ID_Doctor, Fecha, Hora) VALUES (?, ?, ?, ?);";
	sqlite3_prepare_v2(db, sql_insert, -1, &res, 0);
	sqlite3_bind_text(res, 1, dni, -1, SQLITE_STATIC);
	sqlite3_bind_int(res, 2, id_medico);
	sqlite3_bind_text(res, 3, fecha, -1, SQLITE_STATIC);
	sqlite3_bind_text(res, 4, hora, -1, SQLITE_STATIC);



	int rc = sqlite3_step(res);
	if(rc == SQLITE_DONE){
		//Mensaje informativo para añadir en el log
		char info[MAX_INFO_LOG];
		sprintf(info, "Cita resevada por %s con Medico ID %d el %s", dni, id_medico, fecha);
		registrar_log("[ACCION USUARIO]", info);
		printf("[OK] Reserva confirmada. \n");

	}else{
		printf("[ERROR] No se pudo guardar la cita en la DB. \n");
	}


	sqlite3_finalize(res);
	sqlite3_close(db);

	return 1;
}


void db_insertar_centro(char* nom, char* dir, char* cp, char* mun, char* hor,
						char* tipo, char* tel){

	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "INSERT INTO CentroSalud (Nombre, Direccion, CP, Municipio, Horario, TipoCentro, Telefono) "
				  " VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s');",
				  nom, dir, cp, mun, hor, tipo, tel);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("CENTRO", sql);
		printf("[DB] Centro guardado con extio. \n");

	} else {
		printf("[DB] Error al guardar el centro. \n");

	}
	sqlite3_close(db);

}

void db_modificar_centro(int id, char* nuevo_nombre){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "UPDATE CentroSalud SET Nombre = '%s' WHERE ID = %d;", nuevo_nombre, id);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("CENTRO", sql);
		printf("[DB] Centro actualizado correctamente. \n");
	}

	sqlite3_close(db);
}

void db_eliminar_centro(int id){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "DELETE FROM CentroSalud WHERE ID = %d;", id);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("CENTRO", sql);
		printf("[DB] Centro con ID %d eliminado. \n", id);
	}

	sqlite3_close(db);
}

void db_insertar_medico(char* nombre, char* especialidad, int id_centro){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "INSERT INTO Doctor(Nombre, Especialidad, ID_Centro) VALUES ('%s', '%s', %d);",
			nombre, especialidad, id_centro);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("MEDICO", sql);
		printf("[OK] Medico dado de alta correctamente. \n");

	} else{
		printf("[ERROR] No se pudo dar de alta al medico.\n");

	}

	sqlite3_close(db);

}

void db_eliminar_medico(int id){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "DELETE FROM Doctor where ID = %d;", id);

	if(sqlite3_exec(db, sql, 0,0,0) == SQLITE_OK){
		registrar_log("MEDICO", sql);
		printf("[OK] Medico con ID = %d eliminado del sistema.\n", id);
	}

	sqlite3_close(db);

}

void db_eliminar_usuario(char* dni){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "DELETE FROM Usuario WHERE DNI = '%s';", dni);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("USUARIO", sql);
		printf("[OK] Usuario con DNI %s eliminado.\n", dni);

	}else{
		printf("[ERROR] No se pudo eliminar al usuario.\n");
	}

	sqlite3_close(db);

}

void db_modificar_password_usuario(char* dni, char* nueva_pass){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "UPDATE Usuario SET Pass = '%s' WHERE DNI = '%s';", nueva_pass, dni);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("USUARIO", sql);
		printf("[OK] Contraseña actualizada correctamente para el DNI %s.\n", dni);

	} else{
		printf("[ERROR] No se pudo actualizar la contraseña.\n");

	}

	sqlite3_close(db);
}

void db_insertar_farmacia(char* nom, char* dir, char* cp, char* mun, char* tel, int guardia){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "INSERT INTO Farmacia (Nombre, Direccion, CP, Municipio, Telefono, Guardia) "
				" VALUES ('%s', '%s', '%s', '%s', '%s', %d);", nom, dir, cp, mun, tel, guardia);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("FARMACIA",sql);
		printf("[OK] Farmacia añadida correctamente.\n");

	} else{
		printf("[ERROR] No se pudo añadir la farmacia en el sistema. \n");
	}

	sqlite3_close(db);

}

void db_modificar_farmacia(int id, char* nuevo_nom, char* nueva_dir, char* nuevo_tel){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "UPDATE Farmacia SET Nombre = '%s', Direccion = '%s', Telefono = '%s' WHERE ID = %d;",
			nuevo_nom, nueva_dir, nuevo_tel, id);

	if(sqlite3_exec(db, sql, 0,0,0) == SQLITE_OK){
		registrar_log("FARMACIA", sql);
		printf("[OK] Datos de la farmacia actualizados correctamente.\n");

	} else{
		printf("[ERROR] No se pudo modificar la farmacia.\n");
	}

	sqlite3_close(db);
}

void db_modificar_guardia(int id, int estado){
	sqlite3 *db;
	char sql[MAX_SQL];
	sqlite3_open(DB_NAME, &db);

	sprintf(sql, "UPDATE Farmacia SET Guardia = %d WHERE ID = %d;", estado, id);

	if(sqlite3_exec(db, sql, 0,0,0) == SQLITE_OK){
		registrar_log("FARMACIA", sql);
		printf("[OK] Estado de guardia actualizado correctamente.\n");

	} else{
		printf("[ERROR] No se pudo actualizar el estado de guardia.\n");
	}

	sqlite3_close(db);

}

void db_eliminar_farmacia(int id){
	sqlite3 *db;
	char sql[MAX_SQL];
	int rc = sqlite3_open(DB_NAME, &db);

	if(rc != SQLITE_OK){
		printf("[ERROR] No se pudo abrir la base de datos. \n");
		return;
	}

	sprintf(sql, "DELETE FROM Farmacia WHERE ID = %d;", id);

	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		registrar_log("FARMACIA", sql);
		printf("[OK] Farmacia con ID %d eliminada correctamente. \n", id);

	} else {
		printf("[ERROR] No se encontro ninguna farmacia con el ID %d.\n", id);
	}

	sqlite3_close(db);

}

void registrar_log(const char* categoria, const char* detalle){
	FILE *f = fopen("log.txt", "a");
	if(f != NULL){
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

		fprintf(f, "[%d-%02d-%02d  %02d:%02d]  [%s] %s\n",
						tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
						tm.tm_hour, tm.tm_min, categoria, detalle);
		fclose(f);
	}

}


int validar_paciente(char* dni, char* pass){
	sqlite3 *db;
	sqlite3_stmt *res;
	int login_correcto = 0;

	sqlite3_open(DB_NAME, &db);

	//Verificamos cuantos usuarios coinciden con DNI y Contraseña
	char *sql = "SELECT COUNT(*) FROM Usuario WHERE DNI = ? AND Pass = ?;";

	if(sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK){
		sqlite3_bind_text(res, 1, dni, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 2, pass, -1, SQLITE_STATIC);

		if(sqlite3_step(res) == SQLITE_ROW){
			login_correcto = sqlite3_column_int(res, 0);
		}

	}
	sqlite3_finalize(res);

	//Registramos el intento en el log
	if(login_correcto){
		char info_log[MAX_INFO_LOG];
		sprintf(info_log, "LOGIN EXITOSO: Paciente con DNI %s", dni);
		registrar_log("[ACCESO]", info_log);

	} else{
		char info_log[MAX_INFO_LOG];
		sprintf(info_log, "LOGIN FALLIDO: Acceso del paciente con DNI %s denegado", dni);
		registrar_log("[SEGURIDAD]", info_log);
	}

	sqlite3_close(db);
	return login_correcto;

}

void registrar_usuario(char* dni, char* nombre, char* email, char* municipio, char* pass, char* historial){
	sqlite3 *db;
	char sql[MAX_SQL];
	int rc = sqlite3_open(DB_NAME, &db);

	if(rc != SQLITE_OK){
		printf("[ERROR] No se pudo acceder a la base de datos para el registro. \n");
		return;
	}

	//Preparamos la sentencia SQL
	sprintf(sql, "INSERT INTO Usuario (DNI, Nombre, Email, Pass, Tipo, Historial_Breve, Municipio) "
			"       VALUES ('%s', '%s', '%s', '%s', 'PACIENTE', '%s', '%s');",
					dni, nombre, email, pass, historial, municipio);


	if(sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK){
		char info[MAX_INFO_LOG];
		sprintf(info, "NUEVO REGISTRO: %s (DNI: %s) con historial medico inicial.", nombre, dni);
		registrar_log("[SISTEMA]", info);
		printf("\n[OK] Registro completado con éxito.\n");

	}else{
		printf("\n[!] Error: El DNI ya existe o los datos son incorrectos.\n");
	}

	sqlite3_close(db);
}




























