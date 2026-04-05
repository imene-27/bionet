/*
 ============================================================================
 Name        : bionet.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constantes.h"
#include "../db_manager/entidades.h"
#include "../db_manager/db_manager.h"
#include "../importador/importador.h"

void menu_administrador();
void menu_usuario(char* dni_sesion);

void menu_gestion_centros();
void menu_gestion_personal();
void menu_gestion_usuarios();
void menu_gestion_farmacias();
void mostrar_logs_sistema();

int main(){
	setvbuf(stdout, NULL, _IONBF, 0);

	char user[MAX_NOMBRE], pass[MAX_PASS];

	inicializar_db();

	printf("==================================\n");
	printf("       BIENVENIDO A BIONET        \n");
	printf("==================================\n");
	printf("Usuario/DNI: ");
	scanf("%s", user);
	printf("Contraseña: ");
	scanf("%s", pass);

	// Login
	if(strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0){
		menu_administrador();
	} else {
		menu_usuario(user);
	}

	return 0;
}

// MENÚ PARA EL ADMINISTRADOR
void menu_administrador(){
	int opcion = -1;
	while(opcion != 0){
		printf("=========================================\n");
		printf("    BIONET - PANEL DE ADMINISTRACION     \n");
		printf("=========================================\n");
		printf("[1] Gestion de Centros de Salud\n");
		printf("[2] Gestion de Personal Medico\n");
		printf("[3] Gestion de usuarios\n");
		printf("[4] Gestion de Farmacias\n");
		printf("[5] Ver logs del sistema\n");
		printf("[6] Configuración\n");
		printf("[0] Salir y apagar servidor\n");
		printf("-------------------------------\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &opcion);
		getchar();

		switch(opcion){
			case 1:
				menu_gestion_centros();
				break;

			case 2:
				menu_gestion_personal();
				break;

			case 3:
				menu_gestion_usuarios();
				break;

			case 4:
				menu_gestion_farmacias();
				break;

			case 5:
				mostrar_logs_sistema();
				break;

			case 6:
				printf("\n---IMPORTANDO DATOS DESDE CSV---\n");
				// Llamamos a las funciones
				importar_farmacias("datos/farmacias.csv");
				importar_centros_salud("datos/centros.csv");
				importar_medicos("datos/medicos.csv");
				importar_stock("datos/stock.csv");
				printf("[OK] ¡Todos los datos han sido importados a la Base de Datos!\n");
				break;

			case 0:
				printf("Saliendo...Apagando el panel.\n");
				break;

			default:
				printf("Opcion no valida. \n");

		}

	}

}

void menu_gestion_centros(){
	int sub_opcion = -1;
	char id[MAX_ID], nombre[MAX_NOMBRE], dir[MAX_DIR], cp[MAX_CP],
		mun[MAX_MUNICIPIO], hor[20], tipo[20], tel[MAX_DNI];

	while(sub_opcion != 0){
		printf("\n   BIONET: GESTION DE CENTROS DE SALUD  \n");
		printf("[1] Añadir Centros\n");
		printf("[2] Modificar Centros\n");
		printf("[3] Eliminar Centros \n");
		printf("[0] Volver al menu anterior\n");
		printf("-----------------------------------------\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &sub_opcion);

		getchar();

		switch(sub_opcion){
			case 1:
				printf("Nombre: "); fgets(nombre, MAX_NOMBRE, stdin); nombre[strcspn(nombre, "\n")] = 0;
				printf("Direccion: "); fgets(dir, MAX_DIR, stdin); dir[strcspn(dir, "\n")] = 0;
				printf("CP: "); scanf("%s", cp); getchar();
				printf("Municipio: "); fgets(mun, MAX_MUNICIPIO, stdin); mun[strcspn(mun, "\n")] = 0;
				printf("Horario: "); fgets(hor, 20, stdin); hor[strcspn(hor, "\n")] = 0;
				printf("Tipo Centro: "); fgets(tipo, 20, stdin); tipo[strcspn(tipo, "\n")] = 0;
				printf("Telefono: "); scanf("%s", tel); getchar();

				//Llamada a la DB
				db_insertar_centro(nombre, dir, cp, mun, hor, tipo, tel);
				break;

			case 2:
				printf("Introduzca el ID del centro a modificar: ");
				scanf("%s", id);
				getchar();

				printf("Introduzca el nuevo nombre para el centro: ");
				fgets(nombre, MAX_NOMBRE, stdin);
				nombre[strcspn(nombre, "\n")] = 0;

				//Modificamos los datos en la base de datos
				db_modificar_centro(atoi(id), nombre);
				break;

			case 3:
				printf("Introduzca el ID del centro a eliminar: ");
				scanf("%s", id);
				getchar();

				db_eliminar_centro(atoi(id));
				break;

			case 0:
				break;


			default:
				printf("Opcion no valida. \n");
		}
	}

}

void menu_gestion_personal(){
	int sub_opcion = -1;
	char id[MAX_ID], nombre[MAX_NOMBRE], especialidad[15], id_centro[MAX_ID];

	while(sub_opcion != 0){
		printf("\n   BIONET: GESTION DE PERSONAL MEDICO  \n");
		printf("[1] Dar de alta a un medico\n");
		printf("[2] Dar de baja a un medico\n");
		printf("[0] Volver al menu anterior\n");
		printf("--------------------------------------------\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("Nombre completo del medico: ");
				fgets(nombre, MAX_NOMBRE, stdin);
				nombre[strcspn(nombre, "\n")] = 0;

				printf("Especialidad: ");
				fgets(especialidad, 50, stdin);
				especialidad[strcspn(especialidad, "\n")] = 0;

				printf("ID del centro de Salud donde trabaja: ");
				scanf("%s", id_centro);
				getchar();

				db_insertar_medico(nombre, especialidad, atoi(id_centro));
				break;

			case 2:
				printf("Introduzca el ID del medico para dar de baja: ");
				scanf("%s", id);
				getchar();

				db_eliminar_medico(atoi(id));
				break;

			case 0:
				printf("[Saliendo....] Volviendo al Panel de Administracion.\n");
				break;

			default:
				printf("Opcion no valida.\n");


		}
	}
}

void menu_gestion_usuarios(){
	int sub_opcion = -1;
	char dni[MAX_DNI], new_pass[MAX_PASS];

	while(sub_opcion != 0){
		printf("\n   BIONET: GESTION DE USUARIOS \n");
		printf("[1] Eliminar un usuario\n");
		printf("[2] Modificar contraseña de usuario\n");
		printf("[0] Volver al menu anterior\n");
		printf("--------------------------------------------\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("Introduzca el DNI del usuario a eliminar: ");
				scanf("%s", dni);
				getchar();

				db_eliminar_usuario(dni);
				break;

			case 2:
				printf("Introduzca el DNI del usuario: ");
				scanf("%s", dni);
				getchar();

				printf("Introduzca la nueva contraseña: ");
				scanf("%s", new_pass);
				getchar();

				db_modificar_password_usuario(dni, new_pass);
				break;

			case 0:
				printf("[Saliendo....] Volviendo al Panel de Administracion.\n");
				break;

			default:
				printf("Opcion no valida.\n");
		}

	}
}

void menu_gestion_farmacias(){
	int sub_opcion = -1;
	char id[MAX_ID], nombre[MAX_NOMBRE], dir[MAX_DIR], cp[MAX_CP], mun[MAX_MUNICIPIO], tel[MAX_DNI];
	int guardia;

	while(sub_opcion != 0){
		printf("\n   BIONET: GESTION DE FARMACIAS \n");
		printf("[1] Añadir Farmacia\n");
		printf("[2] Modificar datos de Farmacia\n");
		printf("[3] Eliminar Farmacia\n");
		printf("[4] Cambiar estado de Guardia\n");
		printf("[0] Volver al menu anterior\n");
		printf("--------------------------------------------\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("Nombre: "); fgets(nombre, MAX_NOMBRE, stdin); nombre[strcspn(nombre, "\n")] = 0;
				printf("Direccion: "); fgets(dir, MAX_DIR, stdin); dir[strcspn(dir, "\n")] = 0;
				printf("CP: "); scanf("%s", cp); getchar();
				printf("Municipio: "); fgets(mun, MAX_MUNICIPIO, stdin); mun[strcspn(mun, "\n")] = 0;
				printf("Telefono: "), scanf("%s", tel); getchar();
				printf("¿Esta de guardia? (1: Si / 0: No): "); scanf("%d", &guardia); getchar();

				db_insertar_farmacia(nombre, dir, cp, mun, tel, guardia);
				break;

			case 2:
				printf("ID de la farmcia a modificar: ");
				scanf("%s", id);
				getchar();
				printf("Nuevo Nombre: ");
				fgets(nombre, MAX_NOMBRE, stdin); nombre[strcspn(nombre, "\n")] = 0;
				printf("Nueva direccion: ");
				fgets(dir, MAX_DIR, stdin); dir[strcspn(dir, "\n")] = 0;
				printf("Nuevo telefono: ");
				scanf("%s", tel);
				getchar();

				db_modificar_farmacia(atoi(id), nombre, dir, tel);
				break;



			case 3:
				printf("ID de la farmacia a eliminar: ");
				scanf("%s", id);
				getchar();

				db_eliminar_farmacia(atoi(id));
				break;


			case 4:
				printf("ID de la farmacia: ");
				scanf("%s", id);
				getchar();

				printf("Nuevo estado (1: Guardia / 0: Normal): ");
				scanf("%d", &guardia);
				getchar();

				db_modificar_guardia(atoi(id), guardia);
				break;

			case 0:
				printf("[Saliendo....] Volviendo al Panel de Administracion.\n");
				break;

			default:
				printf("Opcion no valida.\n");
				break;

		}


	}
}

void mostrar_logs_sistema(){
	int sub_opcion = -1;
	FILE *f;
	char linea[MAX_LINEA];

	while(sub_opcion != 0){
		printf("=================================\n");
		printf("   BIONET: LOGS DEL SISTEMA       \n");
		printf("==================================\n");

		//Mostramos el contenido del archivo
		f = fopen("log.txt", "r");
		if(f == NULL){
			printf("[!] No hay logs registrados o el archivo no existe.\n");

		} else{
			printf("     CONTENIDO ACTUAL      \n");
			while(fgets(linea, MAX_LINEA, f)){
				printf("%s", linea);

			}
			fclose(f);
		}

		printf("------------------------------------------\n");
		printf("[1] Actualizar vista\n");
		printf("[2] Eliminar todos los logs \n");
		printf("[0] Volver al menu anterior\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("\n[INFO] Vista actualizada. \n");
				break;

			case 2:
				printf("¿Seguro que desea vaciar el historial? (S/N): ");
				char conf;
				scanf("%c", &conf);
				getchar();

				if(conf == 'S' || conf == 's'){
					f = fopen("log.txt", "w");
					if(f != NULL){
						fprintf(f, "[%s] Logs vaciados correctamente\n", __DATE__);
						fclose(f);
						printf("[OK] Historial eliminado. \n");
					}
				}
				break;

			case 0:
				printf("[Saliendo....] Volviendo al Panel de Administracion.\n");
				break;

			default:
				printf("Opcion no valida.\n");
				break;
		}

	}

}


// Menu para el usuario:
void menu_usuario(char* dni_sesion) {
	int opcion = -1;
	char input1[50], input2[50];

//	//Inicializamos la base de datos
//	inicializar_db();
//
//	//Importamos todos los csvs
//	importar_farmacias("farmacias.csv");
//	importar_centros_salud("centros.csv");
//	importar_medicos("medicos.csv");
//	importar_stock("stock.csv");

	//Menu
	while(opcion !=0){
		printf("==================================\n");
		printf(" 	BIONET: GESTIÓN DE SALUD		\n");
		printf("==================================\n");
		printf("1. Buscar Farmacias por CP o municipio\n");
		printf("2. Buscar Centros de Salud por CP o municipio\n");
		printf("3. Consultar stock medicamento\n");
		printf("4. Ver mi ficha medica\n");
		printf("5. Reservar cita medica\n");
		printf("0. Salir\n");
		printf("----------------------------------\n");
		printf("Seleccione una opcion. ");
		scanf("%d", &opcion);
		//Limpiamos el buffer del salto de linea
		getchar();

		switch(opcion){
			case 1:
				printf("Introduce CP o Municipio: ");
				fgets(input1, 50, stdin);
				//Eliminamos el salto de linea del input
				input1[strcspn(input1, "\n")] = 0;
				input1[strcspn(input1, "\r")] = 0;
				buscar_farmacias(input1);
				break;

			case 2:
				printf("Introduce CP o Municipio: ");
				fgets(input1, 50, stdin);
				input1[strcspn(input1, "\n")] = 0;
				input1[strcspn(input1, "\r")] = 0;
				buscar_centros(input1);
				break;

			case 3:
				printf("Nombre del medicamento: ");
				fgets(input1, 50, stdin);
				input1[strcspn(input1,  "\n")] = 0;
				printf("Introduce CP o Municipio para buscar cerca: ");
				fgets(input2, 50, stdin);
				input2[strcspn(input2, "\n")] = 0;
				buscar_medicamento(input1, input2);
				break;

			case 4:
				printf("Introduzca su DNI: ");
				scanf("%s", dni_sesion);
				ver_ficha_medica(dni_sesion);
				break;

			case 5: {
				char especialidad[50], localidad[50], fecha[15], hora[10];
				int id_medico, exito = 0;

				printf("Especialidad deseada: ");
				fgets(especialidad, 50, stdin);
				especialidad[strcspn(especialidad, "\n")] = 0;;

				printf("introduce CP o Municipio: ");
				fgets(localidad, 50, stdin);
				localidad[strcspn(localidad, "\n")] = 0;

				buscar_medicos_especialidad(especialidad, localidad);
				printf("\nIntroduzca el ID del medico elegido: ");
				scanf("%d", &id_medico);
				printf("Introduzca su DNI para la reserva: ");
				scanf("%s", dni_sesion);

				// Hacemos un bucle hasta encontrar una fecha y hora disponible
				while (!exito) {
					printf("\n--- COMPROBAR DISPONIBILIDAD ---\n");
					printf("Fecha (AAAA-MM-DD): ");
					scanf("%s", fecha);
					printf("Hora (HH:MM): ");
					scanf("%s", hora);

					if (comprobar_y_reservar(dni_sesion, id_medico, fecha, hora)) {
						printf("\n[OK] ¡Cita reservada con exito!\n");
						exito = 1;
					} else {
						printf("\n[!] El Dr. ya tiene una cita a esa hora. Intente con otra.\n");
					}
				}
				break;


			case 0:
				printf("Saliendo del sistema...\n");
				break;

			default:
				printf("Opcion no valida\n");
				break;
			}

		}
	}
}

