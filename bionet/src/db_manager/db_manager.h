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
int buscar_medicos_especialidad(char *especialidad, char *localidad);
int comprobar_y_reservar(char *dni, int id_medico, char *fecha, char *hora);
void db_insertar_centro(char* nom, char* dir, char* cp, char* mun, char* hor,
						char* tipo, char* tel);
void db_modificar_centro(int id, char* nuevo_nombre);
void db_eliminar_centro(int id);
void db_insertar_medico(char* nombre, char* especialidad, int id_centro);
void db_eliminar_medico(int id);
void db_eliminar_usuario(char* dni);
void db_modificar_password_usuario(char* dni, char* nueva_pass);
void db_insertar_farmacia(char* nom, char* dir, char* cp, char* mun, char* tel, int guardia);
void db_modificar_farmacia(int id, char* nuevo_nom, char* nueva_dir, char* nuevo_tel);
void db_modificar_guardia(int id, int estado);
void db_eliminar_farmacia(int id);
void registrar_log(const char* categoria, const char* detalle);
int validar_paciente(char* dni, char* pass);
void registrar_usuario(char* dni, char* nombre, char* email, char* municipio, char* pass, char* historial);

#endif /* DB_MANAGER_H_ */
