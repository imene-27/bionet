#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "constantes.h"
#include "../db_manager/entidades.h"
#include "../db_manager/db_manager.h"
#include "../importador/importador.h"

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT  8080
#define BUFF_SIZE    4096

Config miConfig;

static void capturar_salida(void(*func)(char*), char* arg, char* sendBuff){
	FILE *tmp = fopen("_tmp_out.txt", "w");
	if (!tmp) { strcpy(sendBuff, "ERROR;No se pudo capturar salida"); return; }

	// Redirigimos el stdout al fichero temporal
	FILE *old_stdout = stdout;
	*stdout = *tmp;

	func(arg);

	fflush(stdout);
	// Restauramos stdout
	*stdout = *old_stdout;
	fclose(tmp);

	// Leemos lo que se ha escrito en el fichero
	FILE *f = fopen("_tmp_out.txt", "r");
	if (!f) {
		strcpy(sendBuff, "ERROR;No se pudo leer salida");
		return;
	}

	size_t leido = fread(sendBuff, 1, BUFF_SIZE - 1, f);
	sendBuff[leido] = '\0';
	fclose(f);
	remove("_tmp_out.txt");

	if (leido == 0) {
		strcpy(sendBuff, "Sin resultados");
	}
}

//El mismo metodo que el anteriror pero para funciones con dos argumentos
static void capturar_salida2(void (*func)(char*, char*), char* arg1, char* arg2, char* sendBuff) {
    FILE *tmp = fopen("_tmp_out.txt", "w");
    if (!tmp) { strcpy(sendBuff, "ERROR;No se pudo capturar salida"); return; }

    FILE *old_stdout = stdout;
    *stdout = *tmp;

    func(arg1, arg2);

    fflush(stdout);
    *stdout = *old_stdout;
    fclose(tmp);

    FILE *f = fopen("_tmp_out.txt", "r");
    if (!f) { strcpy(sendBuff, "ERROR;No se pudo leer salida"); return; }

    size_t leido = fread(sendBuff, 1, BUFF_SIZE - 1, f);
    sendBuff[leido] = '\0';
    fclose(f);
    remove("_tmp_out.txt");

    if (leido == 0) strcpy(sendBuff, "Sin resultados");
}

void buscar_farmacias(char* criterio){
	FarmaciaNodo* lista = buscar_farmcias_lista(criterio);

	if(lista == NULL){
		printf("No se han encontrado farmacias en esa zona o con ese CP.\n");
		return;
	}

	printf("---- FARMACIAS ENCONTRADAS ----\n");
	FarmaciaNodo* actual = lista;
	while(actual != NULL){
		printf("[%s] %s - Tlf; %s (Dir: %s)\n",
				actual->es_guardia ? "GUARDIA" : "NORMAL",
				actual->nombre,
				actual->telefono,
				actual->direccion);

		actual = actual->siguiente;
	}

	actual = lista;
	while(actual != NULL){
		FarmaciaNodo* aux = actual->siguiente;
		free(actual);
		actual = aux;
	}

}


void procesar_comando(char *recvBuff, char *sendBuff) {

    char *partes[10];
    int n = 0;

    char copia[BUFF_SIZE];
    strncpy(copia, recvBuff, BUFF_SIZE);
    char *tok = strtok(copia, ";");
    while (tok && n < 10) {
    	partes[n++] = tok;
    	tok = strtok(NULL, ";");
    }

    if (n == 0) {
    	strcpy(sendBuff, "ERROR;Mensaje vacio");
    	return;
    }

    //Para el PACIENTE
    if (strcmp(partes[0], "LOGIN") == 0 && n >= 3) {
        int ok = validar_paciente(partes[1], partes[2]);
        if (ok) {
            // Formato: OK;dni;nombre;email;municipio
            sqlite3 *db;
            sqlite3_stmt *stmt;
            if (sqlite3_open(miConfig.ruta_db, &db) == SQLITE_OK) {
                char *sql = "SELECT Nombre, Email, Municipio FROM Usuario WHERE DNI = ?;";
                if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
                    sqlite3_bind_text(stmt, 1, partes[1], -1, SQLITE_STATIC);
                    if (sqlite3_step(stmt) == SQLITE_ROW) {
                        snprintf(sendBuff, BUFF_SIZE, "OK;%s;%s;%s;%s",
                            partes[1],
                            sqlite3_column_text(stmt, 0),
                            sqlite3_column_text(stmt, 1),
                            sqlite3_column_text(stmt, 2));
                    }
                    sqlite3_finalize(stmt);
                }
                sqlite3_close(db);
            }
        } else {
            strcpy(sendBuff, "ERROR;Credenciales incorrectas");
        }

    } else if (strcmp(partes[0], "REGISTRO") == 0 && n >= 7) {
		registrar_usuario(partes[1], partes[2], partes[3],
						  partes[4], partes[5], partes[6]);
		strcpy(sendBuff, "OK");

    } else if (strcmp(partes[0], "BUSCAR_FARMACIA") == 0 && n >= 2) {
        capturar_salida(buscar_farmacias, partes[1], sendBuff);

    } else if (strcmp(partes[0], "BUSCAR_CENTRO") == 0 && n >= 2) {
        capturar_salida((void(*)(char*))buscar_centros, partes[1], sendBuff);

	} else if (strcmp(partes[0], "BUSCAR_MED") == 0 && n >= 3) {
        capturar_salida2(buscar_medicamento, partes[1], partes[2], sendBuff);

	} else if (strcmp(partes[0], "FICHA_MEDICA") == 0 && n >= 2) {
        capturar_salida((void(*)(char*))ver_ficha_medica, partes[1], sendBuff);

	} else if (strcmp(partes[0], "BUSCAR_MEDICOS") == 0 && n >= 3) {
        capturar_salida2((void(*)(char*,char*))buscar_medicos_especialidad, partes[1], partes[2], sendBuff);

   } else if (strcmp(partes[0], "RESERVAR_CITA") == 0 && n >= 5) {
	   int ok = comprobar_y_reservar(partes[1], atoi(partes[2]),
									 partes[3], partes[4]);
	   strcpy(sendBuff, ok ? "OK" : "ERROR;Cita no disponible");


   //Para el ADMINISTRADOR

   } else if (strcmp(partes[0], "ADMIN_ADD_CENTRO") == 0 && n >= 8) {
       // ADMIN_ADD_CENTRO;nombre;dir;cp;municipio;horario;tipo;telefono
       db_insertar_centro(partes[1], partes[2], partes[3], partes[4],
                          partes[5], partes[6], partes[7]);
       registrar_log("[ADMIN]", "Centro de salud añadido");
       strcpy(sendBuff, "OK;Centro añadido correctamente");

   } else if (strcmp(partes[0], "ADMIN_MOD_CENTRO") == 0 && n >= 3) {
       // ADMIN_MOD_CENTRO;id;nuevo_nombre
       db_modificar_centro(atoi(partes[1]), partes[2]);
       registrar_log("[ADMIN]", "Centro de salud modificado");
       strcpy(sendBuff, "OK;Centro modificado correctamente");

   } else if (strcmp(partes[0], "ADMIN_DEL_CENTRO") == 0 && n >= 2) {
       // ADMIN_DEL_CENTRO;id
       db_eliminar_centro(atoi(partes[1]));
       registrar_log("[ADMIN]", "Centro de salud eliminado");
       strcpy(sendBuff, "OK;Centro eliminado correctamente");

   } else if (strcmp(partes[0], "ADMIN_ADD_FARMACIA") == 0 && n >= 7) {
       // ADMIN_ADD_FARMACIA;nombre;dir;cp;municipio;telefono;guardia
       db_insertar_farmacia(partes[1], partes[2], partes[3],
                            partes[4], partes[5], atoi(partes[6]));
       registrar_log("[ADMIN]", "Farmacia añadida");
       strcpy(sendBuff, "OK;Farmacia añadida correctamente");

   } else if (strcmp(partes[0], "ADMIN_MOD_FARMACIA") == 0 && n >= 5) {
       // ADMIN_MOD_FARMACIA;id;nuevo_nombre;nueva_dir;nuevo_tel
       db_modificar_farmacia(atoi(partes[1]), partes[2], partes[3], partes[4]);
       registrar_log("[ADMIN]", "Farmacia modificada");
       strcpy(sendBuff, "OK;Farmacia modificada correctamente");

   } else if (strcmp(partes[0], "ADMIN_DEL_FARMACIA") == 0 && n >= 2) {
       // ADMIN_DEL_FARMACIA;id
       db_eliminar_farmacia(atoi(partes[1]));
       registrar_log("[ADMIN]", "Farmacia eliminada");
       strcpy(sendBuff, "OK;Farmacia eliminada correctamente");

   } else if (strcmp(partes[0], "ADMIN_DEL_USUARIO") == 0 && n >= 2) {
       // ADMIN_DEL_USUARIO;dni
       db_eliminar_usuario(partes[1]);
       registrar_log("[ADMIN]", "Usuario eliminado");
       strcpy(sendBuff, "OK;Usuario eliminado correctamente");

   } else if (strcmp(partes[0], "ADMIN_MOD_PASS") == 0 && n >= 3) {
       // ADMIN_MOD_PASS;dni;nueva_pass
       db_modificar_password_usuario(partes[1], partes[2]);
       registrar_log("[ADMIN]", "Contrasena de usuario modificada");
       strcpy(sendBuff, "OK;Contrasena modificada correctamente");

   } else if (strcmp(partes[0], "ADMIN_ADD_MEDICO") == 0 && n >= 4) {
       // ADMIN_ADD_MEDICO;nombre;especialidad;id_centro
       db_insertar_medico(partes[1], partes[2], atoi(partes[3]));
       registrar_log("[ADMIN]", "Medico añadido");
       strcpy(sendBuff, "OK;Medico añadido correctamente");

   } else if (strcmp(partes[0], "ADMIN_DEL_MEDICO") == 0 && n >= 2) {
       // ADMIN_DEL_MEDICO;id
       db_eliminar_medico(atoi(partes[1]));
       registrar_log("[ADMIN]", "Medico eliminado");
       strcpy(sendBuff, "OK;Medico eliminado correctamente");

   } else if(strcmp(partes[0], "VER_LOGS") == 0){
	   FILE* f = fopen(miConfig.ruta_logs, "r");
	   if(f == NULL){
		   strcpy(sendBuff, "ERROR;No se pudo abrir el archivo de logs.\n");

	   } else{
		   char linea[256];
		   sendBuff[0] = '\0';

		   while(fgets(linea, sizeof(linea), f)){
			   if(strlen(sendBuff) + strlen(linea) < 4000){
				   strcat(sendBuff, linea);
			   } else{
				   break;
			   }
		   }
		   fclose(f);
	   }

   }  else if(strcmp(partes[0], "VACIAR_LOGS") == 0){
	   FILE* f = fopen(miConfig.ruta_logs, "w");
	   if(f == NULL){
		   strcpy(sendBuff, "ERROR;No se pudo vaciar el archivo de logs.\n");

	   } else{
		   fprintf(f, "[%s] Logs vaciados correctamente\n", __DATE__);
		   fclose(f);
		   strcpy(sendBuff, "OK");

	   	}
    }  else if(strcmp(partes[0], "SET_CONFIG") == 0){

 	   if(strcmp(partes[1], "PUERTO") == 0){
 		   miConfig.puerto = atoi(partes[2]);
 		   strcpy(sendBuff, "OK;Puerto modificado. Aplica al reiniciar el servidor.");

 	   } else if(strcmp(partes[1], "DB") == 0){
 		   strcpy(miConfig.ruta_db, partes[2]);
 		   strcpy(sendBuff, "OK;Ruta de Base de Datos actualizada.");

 	   } else if (strcmp(partes[1], "LOGS") == 0){
 		   strcpy(miConfig.ruta_logs, partes[2]);
 		   strcpy(sendBuff, "OK;Ruta de Logs actualizada.");

 	   } else{
 		   strcpy(sendBuff, "ERROR;Parametro de configuracion desconocido");
 	   }

    }  else {
	   strcpy(sendBuff, "ERROR;Comando desconocido");
    	}
}
int main() {
    miConfig.puerto = SERVER_PORT;
    strcpy(miConfig.ruta_db, "../bionet.db");
    strcpy(miConfig.ruta_logs, "../log.txt");

    inicializar_db(miConfig.ruta_db);
    auto_carga_datos();

	SOCKET conn_socket, comm_socket;
	struct sockaddr_in server, client;
	char sendBuff[BUFF_SIZE], recvBuff[BUFF_SIZE];
	WSADATA wsaData;

	// Initialising Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed: %d\n", WSAGetLastError());
		return -1;
	}

	// SOCKET creation
	conn_socket = socket(AF_INET, SOCK_STREAM, 0);
	 if (conn_socket == INVALID_SOCKET) {
		printf("Error al crear socket: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Socket creado.\n");

	int opt = 1;
	setsockopt(conn_socket, SOL_SOCKET, SO_REUSEADDR,(char*)&opt, sizeof(opt));

	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family      = AF_INET;
	server.sin_port        = htons(SERVER_PORT);

	// BIND
	if (bind(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return -1;
	}
	printf("Bind hecho.\n");

	// LISTEN
	 if (listen(conn_socket, 5) == SOCKET_ERROR) {
		printf("Listen failed: %d\n", WSAGetLastError());
		closesocket(conn_socket);
		WSACleanup();
		return -1;
	}
	printf("Esperando conexiones en puerto %d...\n", SERVER_PORT);

	// Bucle principal — igual que el ejemplo de la profe
	do {
		int stsize = sizeof(struct sockaddr);
		comm_socket = accept(conn_socket, (struct sockaddr*)&client, &stsize);
		if (comm_socket == INVALID_SOCKET) { perror("accept"); continue; }
		printf("Cliente conectado: %s\n", inet_ntoa(client.sin_addr));


		memset(recvBuff, 0, BUFF_SIZE);
		int bytes = recv(comm_socket, recvBuff, BUFF_SIZE - 1, 0);
		if (bytes > 0) {
			printf("Recibido: %s\n", recvBuff);
			memset(sendBuff, 0, BUFF_SIZE);
			procesar_comando(recvBuff, sendBuff);
			send(comm_socket, sendBuff, strlen(sendBuff), 0);
			printf("Enviado: %s\n", sendBuff);
		}

		closesocket(comm_socket);

	} while (1);

	closesocket(conn_socket);
	WSACleanup();
	return 0;


}
