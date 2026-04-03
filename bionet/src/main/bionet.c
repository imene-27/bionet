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
#include "../db_manager/entidades.h"
#include "../db_manager/db_manager.h"
#include "../importador/importador.h"


int main() {
	int opcion = -1;
	char input1[50], input2[50], dni_sesion[15];

	//Inicializamos la base de datos
	inicializar_db();

	//Importamos todos los csvs
	importar_farmacias("farmacias.csv");
	importar_centros_salud("centros.csv");
	importar_medicos("medicos.csv");
	importar_stock("stock.csv");

	//Menu
	while(opcion !=0){
		printf("==================================\n");
		printf(" 	BIONET: GESTIÓN DE SALUD		\n");
		printf("==================================\n");
		printf("1. Buscar Farmacias por CP o municipio\n");
		printf("2. Buscar Centros de Salud por CP o municipio\n");
		printf("3. Consultar stock medicamento");
		printf("4. Ver mi ficha medica");
		printf("5. Reservar cita medica");
		printf("0. Salir\n");
		printf("----------------------------------\n");
		printf("Seleccione una opcion. ");
		scanf("%d", opcion);
		//Limpiamos el buffer del salto de linea
		getchar();

		switch(opcion){
			case 1:
				printf("Introduce CP o Municipio: ");
				fgets(input1, 50, stdin);
				//Eliminamos el salto de linea del input
				input1[strcspn(input1, "\n")] = 0;
				buscar_farmacias(input1);
				break;

			case 2:
				printf("Introduce CP o Municipio: ");
				fgets(input1, 50, stdin);
				input1[strcspn(input1, "\n")] = 0;
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
	return 0;
}

