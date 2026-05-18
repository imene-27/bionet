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

void procesar_comando(char *recvBuff, char *sendBuff) {

    char *partes[10];
    int n = 0;

    char copia[BUFF_SIZE];
    strncpy(copia, recvBuff, BUFF_SIZE);
    char *tok = strtok(copia, ";");
    while (tok && n < 10) {
    	partes[n++] = tok; tok = strtok(NULL, ";");
    }

    if (n == 0) {
    	strcpy(sendBuff, "ERROR;Mensaje vacio");
    	return;
    }

    if (strcmp(partes[0], "LOGIN") == 0 && n >= 3) {
        int ok = validar_paciente(partes[1], partes[2]);
        strcpy(sendBuff, ok ? "OK" : "ERROR;Credenciales incorrectas");

    } else if (strcmp(partes[0], "REGISTRO") == 0 && n >= 7) {
		registrar_usuario(partes[1], partes[2], partes[3],
						  partes[4], partes[5], partes[6]);
		strcpy(sendBuff, "OK");

    } else if (strcmp(partes[0], "BUSCAR_FARMACIA") == 0 && n >= 2) {
		buscar_farmacias(partes[1]);
		strcpy(sendBuff, "OK");             // TODO: capturar salida y mandarla

    } else if (strcmp(partes[0], "BUSCAR_CENTRO") == 0 && n >= 2) {
		buscar_centros(partes[1]);
		strcpy(sendBuff, "OK");

	} else if (strcmp(partes[0], "BUSCAR_MED") == 0 && n >= 3) {
		buscar_medicamento(partes[1], partes[2]);
		strcpy(sendBuff, "OK");

	} else if (strcmp(partes[0], "FICHA_MEDICA") == 0 && n >= 2) {
		ver_ficha_medica(partes[1]);
		strcpy(sendBuff, "OK");
	} else if (strcmp(partes[0], "BUSCAR_MEDICOS") == 0 && n >= 3) {
		   buscar_medicos_especialidad(partes[1], partes[2]);
		   strcpy(sendBuff, "OK");

   } else if (strcmp(partes[0], "RESERVAR_CITA") == 0 && n >= 5) {
	   int ok = comprobar_y_reservar(partes[1], atoi(partes[2]),
									 partes[3], partes[4]);
	   strcpy(sendBuff, ok ? "OK" : "ERROR;Cita no disponible");

   } else {
	   strcpy(sendBuff, "ERROR;Comando desconocido");
   }
}
int main() {
	inicializar_db("bionet.db");

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
