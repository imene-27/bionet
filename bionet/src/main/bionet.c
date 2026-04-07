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
void menu_paciente(char* dni_sesion);

void login_paciente();
void login_admin();
void registro_nuevo_paciente();
void menu_gestion_centros();
void menu_gestion_personal();
void menu_gestion_usuarios();
void menu_gestion_farmacias();
void mostrar_logs_sistema();
void menu_sincronizacion_csv();
void guardar_config(Config c);
void cargar_config(Config *c);
void menu_configuracion();

Config miConfig;

int main(){
	setvbuf(stdout, NULL, _IONBF, 0);

	//Cargar configuración
	cargar_config(&miConfig);

	inicializar_db(miConfig.ruta_db);

	//Cargamos los datos (si es necesario)
	auto_carga_datos();

	int opcion_inicial = -1;
	while(opcion_inicial != 0){

		printf("==================================\n");
		printf("       BIENVENIDO A BIONET        \n");
		printf("==================================\n");
		printf("[1] Login Paciente \n");
		printf("[2] Login Administrador \n");
		printf("[3] Registrarse (Nuevo Paciente) \n");
		printf("[0] Salir \n");
		printf("Seleccione: ");
		scanf("%d", &opcion_inicial);

		getchar();

		switch(opcion_inicial){
			case 1:
				login_paciente();
				break;

			case 2:
				login_admin();
				break;

			case 3:
				registro_nuevo_paciente();
				break;

			case 0:
				registrar_log("[SISTEMA]", "Aplicación cerrada por el usuario.");
				printf("[Saliendo....] Applicación apagada.\n");
				break;

			default:
				printf("Opción no válida. \n");
		}

	}

	return 0;
}

void login_paciente(){
	char dni[MAX_DNI], pass[MAX_PASS];

	printf("\n=====================================\n");
	printf("           ACCESO PACIENTES             \n");
	printf("======================================\n");

	printf("DNI: ");
	scanf("%s", dni);
	getchar();

	printf("Contraseña: ");
	scanf("%s", pass);
	getchar();

	//Llamamos a la funcion de la DB para validar el Usuario
	if(validar_paciente(dni, pass)){
		printf("\n[OK] Bienvenido al sistema BioNet. \n");
		menu_paciente(dni);

	} else{
		printf("\n[!] DNI o contraeña incorrectos. \n");
		printf("¿No tienes cuenta? Selecciona 'Registrarse' en el menu principal. \n");
	}

}

void login_admin(){
	char user[20], pass[MAX_PASS];
	printf("\n=====================================\n");
	printf("          ACCESO ADMINISTRADOR          \n");
	printf("=======================================\n");

		printf("Usuario: ");
		scanf("%s", user);
		getchar();

		printf("Contraseña: ");
		scanf("%s", pass);
		getchar();

		if(strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0){
			registrar_log("[LOGIN]", "ADMIN ha entrado al panel de control");
			menu_administrador();

		}else{
			printf("[!] Datos de administrador incorrectos. \n");
		}

}

void registro_nuevo_paciente(){
	char dni[MAX_DNI], nombre[MAX_NOMBRE], email[MAX_NOMBRE], pass[MAX_PASS], mun[MAX_MUNICIPIO], historial[200];

	printf("\n====================================\n");
	printf("      REGISTRO DE NUEVO PACIENTE      \n");
	printf("======================================\n");


	printf("DNI: ");
	scanf("%s", dni);
	getchar();

	printf("Nombre Completo: ");
	fgets(nombre, MAX_NOMBRE, stdin);
	nombre[strcspn(nombre, "\n")] = 0;

	printf("Email: ");
	fgets(email, MAX_NOMBRE, stdin);
	email[strcspn(email, "\n")] = 0;

	printf("Municipio: ");
	fgets(mun, MAX_MUNICIPIO, stdin);
	mun[strcspn(mun, "\n")] = 0;

	printf("Contraseña: ");
	scanf("%s", pass);
	getchar();

	printf("Historial médico breve (alergias, enfermedades....): ");
	fgets(historial, 200, stdin);
	historial[strcspn(historial, "\n")] = 0;


	//Llamamos a la DB para guardarlo
	registrar_usuario(dni, nombre, email, mun, pass, historial);

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
		printf("[6] Sincronización de Datos (CSV)\n");
		printf("[7] Configuración\n");
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
//				printf("\n---IMPORTANDO DATOS DESDE CSV---\n");
//				// Llamamos a las funciones
//				importar_farmacias("datos/farmacias.csv");
//				importar_centros_salud("datos/centros.csv");
//				importar_medicos("datos/medicos.csv");
//				importar_stock("datos/medicamento.csv");
//				printf("[OK] ¡Todos los datos han sido importados a la Base de Datos!\n");
				menu_sincronizacion_csv();
				break;

			case 7:
				menu_configuracion();
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
		printf("\n   BIONET: GESTIÓN DE PERSONAL MÉDICO  \n");
		printf("[1] Dar de alta a un médico\n");
		printf("[2] Dar de baja a un médico\n");
		printf("[0] Volver al menu anterior\n");
		printf("--------------------------------------------\n");
		printf("Seleccione una opción: ");
		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("Nombre completo del médico: ");
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
				printf("Introduzca el ID del médico para dar de baja: ");
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
		printf("\n=================================\n");
		printf("   BIONET: LOGS DEL SISTEMA       \n");
		printf("==================================\n");

		//Mostramos el contenido del archivo
		f = fopen(miConfig.ruta_logs, "r");
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
					f = fopen(miConfig.ruta_logs, "w");
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

void menu_sincronizacion_csv(){
	int sub_opcion = -1;
	while(sub_opcion != 0){
		printf("\n=================================\n");
		printf("     SINCRONIZACION DE DATOS CSV    \n");
		printf("===================================\n");
		printf("[1] Sincronizar Personal Médico\n");
		printf("[2] Sincronizar Centros de Salud\n");
		printf("[3] Sincronizar Farmacias y Stock\n");
		printf("[4] Volver a importar todos los archivos\n");
		printf("[0] Volver al panel de administración\n");
		printf("--------------------------------------\n");
		printf("Seleccione una opción: ");

		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("[SISTEMA] Actualizando médicos...\n");
				importar_medicos("datos/medicos.csv");
				registrar_log("[ADMIN]", "Sincronización manual de médicos realizada");
				printf("[OK] Médicos actualizados");
				break;

			case 2:
				printf("[SISTEMA] Actualizando centros....\n");
				importar_centros_salud("datos/centros.csv");
				registrar_log("[ADMIN]", "Sincronización manual de centros de salud realizada");
				printf("[OK] Centros de salud actualizados.\n");
				break;

			case 3:
				printf("[SISTEMA] Actualizando farmacias y stock....\n");
				importar_farmacias("datos/farmacias.csv");
				importar_stock("datos/stock.csv");
				registrar_log("[ADMIN]", "Sincronización manual de farmacia y stock realizada");
				printf("[OK] Farmacias y stock actualizados.\n");
				break;

			case 4:
				printf("[!] Iniciando sincronización masiva del sistema....\n");
				importar_medicos("datos/medicos.csv");
				importar_centros_salud("datos/centros.csv");
				importar_farmacias("datos/farmacias.csv");
				importar_stock("datos/stock.csv");
				registrar_log("[ADMIN]", "Sincronizacion total de CSV completada.");
				printf("[OK] Todos los datos han sido refrescados");
				break;

			case 0:
				printf("Volviendo al panel anterior....\n");
				break;

			default:
				printf("Opcion no válida.\n");
				break;

		}
	}
}

//Función para guardar la config en el archivo
void guardar_config(Config c){
	FILE *f = fopen("config.txt", "w");
	if(f == NULL){
		printf("[!] Error al abrir el archivo para escribir. \n");
		return;
	}

	fprintf(f, "puerto=%d\n", c.puerto);
	fprintf(f, "ruta_db=%s\n", c.ruta_db);
	fprintf(f, "ruta_logs=%s\n", c.ruta_logs);
	fclose(f);
	printf("[OK] ¡Configuración guardada con éxito!\n");

}

//Función para cargar la config
void cargar_config(Config *c){
	FILE *f = fopen("config.txt", "r");
	if(f == NULL){
		//Valores por defecto si el archivo no existe
		c->puerto = 8080;
		strcpy(c->ruta_db, "./database/bionet.db");
		strcpy(c->ruta_logs, "./logs/errores.log");
		return;
	}
	fscanf(f, "puerto=%d\n", &c->puerto);
	fscanf(f, "ruta_db=%s\n", c->ruta_db);
	fscanf(f, "ruta_logs=%s\n", c->ruta_logs);
	fclose(f);
}

void menu_configuracion(){
	int sub_opcion = -1;
	char mensaje_log[MAX_INFO_LOG];

	while(sub_opcion != 0){
		cargar_config(&miConfig);

		printf("\n==========================\n");
		printf("        CONFIGURACION        \n");
		printf("=============================\n");
		printf("[1] Cambiar puerto de escucha\n");
		printf("[2] Cambiar ruta Base de Datos\n");
		printf("[3] Cambiar ruta de logs\n");
		printf("[0] Volver al panel del administrador\n");
		printf("Seleccione una opcion: ");

		scanf("%d", &sub_opcion);
		getchar();

		switch(sub_opcion){
			case 1:
				printf("Nuevo puerto: ");
				scanf("%d", &miConfig.puerto);
				guardar_config(miConfig);

				//Registro en el LOG
				sprintf(mensaje_log, "ADMIN: Cambio de puerto a %d", miConfig.puerto);
				registrar_log("[CONFIGURACION]", mensaje_log);
				break;

			case 2:
				printf("Nueva ruta de Base de Datos: ");
				scanf("%s", miConfig.ruta_db);
				guardar_config(miConfig);

				sprintf(mensaje_log, "ADMIN: Nueva ruta DB establecida %s", miConfig.ruta_db);
				registrar_log("[CONFIGURACION]", mensaje_log);
				break;

			case 3:
				printf("Nueva ruta de Logs: ");
				scanf("%s", miConfig.ruta_logs);
				guardar_config(miConfig);

				sprintf(mensaje_log, "ADMIN: El sistema de logs se ha movido a esa ruta");
				registrar_log("[CONFIGURACION]", mensaje_log);
				break;

			case 0:
				printf("[Saliendo...] Volviendo al panel del administrador.\n");
				break;

			default:
				printf("Opcion no válida.\n");

		}

	}
}

// Menu para el paciente:
void menu_paciente(char* dni_sesion) {
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
		printf("[1] Buscar Farmacias por CP o municipio\n");
		printf("[2] Buscar Centros de Salud por CP o municipio\n");
		printf("[3] Consultar stock medicamento\n");
		printf("[4] Ver mi ficha medica\n");
		printf("[5] Reservar cita medica\n");
		printf("[0] Salir\n");
		printf("----------------------------------\n");
		printf("Seleccione una opcion: ");
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
				input1[strcspn(input1, "\r")] = 0;
				printf("Introduce CP o Municipio para buscar cerca: ");
				fgets(input2, 50, stdin);
				input2[strcspn(input2, "\n")] = 0;
				input2[strcspn(input2, "\r")] = 0;
				buscar_medicamento(input1, input2);
				break;


			case 4:
				ver_ficha_medica(dni_sesion);
				break;

			case 5: {
				char especialidad_elegida[50], localidad[MAX_MUNICIPIO], fecha[15], hora[10];
				int id_medico, opcion_esp, exito = 0;

				const char *lista_especialidades[] = {
						"Medicina General",  // Opción 1
						"Pediatria",         // Opción 2
						"Cardiologia",       // Opción 3
						"Traumatologia",     // Opción 4
						"Dermatologia",      // Opción 5
						"Oftalmologia",      // Opción 6
						"Ginecologia",       // Opción 7
						"Fisioterapia"       // Opción 8
				};

				int total_especialidades = 8;

				printf("\n--- RESERVAR CITA MÉDICA ---\n");

				printf("Especialidades disponibles:\n ");

				for (int i = 0; i < total_especialidades; i++) {
					printf("[%d] %s\n", i+1, lista_especialidades[i]);
				}
				printf("Selecciones el número de la especialidad: ");
				scanf("%d", &opcion_esp);
				getchar();

				if (opcion_esp < 1 || opcion_esp > total_especialidades){
					printf("Opción no válida. Volviendo al menú...\n");
					break;
				}
				strcpy(especialidad_elegida, lista_especialidades[opcion_esp -1]);

				printf("Introduce CP o Municipio: ");
				fgets(localidad, MAX_MUNICIPIO, stdin);
				localidad[strcspn(localidad, "\n")] = 0;
				localidad[strcspn(localidad, "\r")] = 0;

				//Guardamos cuántos médicos se han encontrado
				int encontrados = buscar_medicos_especialidad(especialidad_elegida, localidad);

				// Solo si hay médicos, pedimos lo datos de la cita
				if(encontrados > 0){
					printf("\nIntroduzca el ID del médico elegido: ");
					scanf("%d", &id_medico);
					getchar();

					// Hacemos un bucle hasta encontrar una fecha y hora disponible
					while (!exito) {
						printf("\n--- FECHA Y HORA DE LA CITA ---\n");
						printf("Fecha (AAAA-MM-DD): ");
						scanf("%s", fecha);

						printf("Hora (HH:MM): ");
						scanf("%s", hora);
						getchar();

						if (comprobar_y_reservar(dni_sesion, id_medico, fecha, hora)) {
							printf("\n[OK] ¡Cita reservada con exito!\n");
							exito = 1;
						} else {
							printf("\n[!] El Dr. ya tiene una cita a esa hora. Intente con otra.\n");
							printf("¿Desea probar otra hora? (S/N): ");

							char respuesta;
							scanf(" %c", &respuesta);
							getchar();

							if(respuesta == 'n' || respuesta == 'N'){
								printf("Reserva cancelada. Volviendo al menú principal...\n");
								break;
							}
						}
					}

				} else {
					printf("No se encontraron médicis de esa especialidad en la zona.\n");
					printf("Volviendo al menú principal...\n");
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

