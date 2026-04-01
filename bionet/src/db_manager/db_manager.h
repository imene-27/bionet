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

#endif /* DB_MANAGER_H_ */
