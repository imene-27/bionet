/*
 * db_manager.h
 *
 *  Created on: 28 mar 2026
 *      Author: nahia.epelde
 */

#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_

void inicializar_db();
void buscar_farmacias_por_cp(int cp_buscado);
void buscar_medicos_por_centro(int id_centro);
void ver_stock_farmacia(int id_farmacia);

#endif /* DB_MANAGER_H_ */
