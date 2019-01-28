#ifndef __ROBOT_H
#define __ROBOT_H

#define RECTO 9999

#define MAX_VELOCIDAD_GIRO   (PI/1.4)

// orientaciones
#define ESTE 0
#define NORTE 1
#define OESTE 2
#define SUR 3

void robot_set_posicion(float x, float y);
void robot_set_orientacion(int8_t o);
float robot_get_posicion_x();
float robot_get_posicion_y();
int8_t robot_get_orientacion();
void robot_actualiza_posicion();
void robot_ir_a(float x, float y, float radio);
void robot_gira(int8_t orientacion);
void robot_parar();

#ifdef ENCODERS_LOG_ESTADO
void robot_log_estado_cabecera();
void robot_log_estado();
#endif

#endif /* ifndef ROBOT_H

 */
