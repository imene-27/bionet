/*
 * db_manager.h
 *
 *  Created on: 28 mar 2026
 *      Author: nahia.epelde
 */

#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_

void inicializar_db();
void buscar_farmacias(char *busqueda);
void buscar_centros(char *busqueda);
void buscar_medicamento(char *nombre_med, char *localidad);
void ver_ficha_medica(char *dni_usuario);
void buscar_medicos_especialidad(char *especialidad, char *localidad);
int comprobar_y_reservar(char *dni, int id_medico, char *fecha, char *hora);

#endif /* DB_MANAGER_H_ */
