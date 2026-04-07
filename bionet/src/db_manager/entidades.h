 /*
 * entidades.h
 *
 *  Created on: 28 mar 2026
 *      Author: nahia.epelde
 */

#ifndef ENTIDADES_H_
#define ENTIDADES_H_

//Definimos la estructura para Farmacia
typedef struct {
	int id_farmacia;
	char nombre[50];
	char direccion[100];
	char horario[50];
	char telefono[15];
	int codigo_postal_fk;
	int es_guardia; //1 si esta de guardia o 0 si no lo esta
} Farmacia;


//Definimos la estructura para Usuario (tanto admin como ciudadano)
typedef struct {
	char dni[10];
	char nombre[50];
	char email[50];
	char contrasena[20];
	char tipo[15]; 	//Para indicar si es "admin" o "usuario"
} Usuario;


//Definimos la estructura para  Centros de Salud
typedef struct {
	int id_centro;
	char nombre[50];
	char direccion[100];
	char horario[50];
	int codigo_postal_fk;
	int tiene_urgencias; //1 para SI y 0 para NO
} CentroSalud;


//Definimos la estructura para Doctor
typedef struct {
	int id_medico;
	char nombre[50];
	char especialidad[50];
	int id_centro_fk; //ID del centro donde trabaja
} Doctor;


//Definimos las estructura para Medicamento
typedef struct {
	int id_medic;
	char nombre[50];
	int unidades;
} Medicamento;


//Definimos las estructura para Cita
typedef struct {
	int id_cita;
	char dni_usuario_fk[10]; //El usuario que solicita la cita
	int id_medico_fk; //El medico con el que solicita la cita
	char fecha[11]; //Las fechas tienen este formato: "2026-01-01"
	char hora[6]; //Las horas tienen este formato: "10:30"
} Cita;


//Definimos las estructura para Tratamiento
typedef struct {
	int id_tratamiento;
	int id_medicamento_fk;
	char dosis[50];
} Tratamiento;


//Definimos las estructura para Ficha Medica
typedef struct {
	char dni_fk[10]; //Usuario al que pertenece
	char enfermedades[200];
	char operaciones[200];
	int id_tratamiento_fk; //Tratamoento activo para este usuario
} FichaMedica;


//Definimos las estructura para Codigo postal
typedef struct {
	int codigo;
	char municipio[50];
} CodigoPostal;

//Estructura para manjear la configuración
typedef struct{
	int puerto;
	char ruta_db[MAX_PATH];
	char ruta_logs[MAX_PATH];
}Config;
extern Config miConfig;


#endif /* ENTIDADES_H_ */
