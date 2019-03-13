
#ifndef __ROBOT_H
#define __ROBOT_H

enum tipo_orientacion {NORTE, ESTE, SUR, OESTE};
enum tipo_estado {PARADO, PARADO_PARA_GIRO, PAUSA_PRE_GIRO, EXPLORANDO_INICIAL, PAUSA_INICIAL, EXPLORANDO};


void robot_set_amax(float aceleracion_maxima);
float robot_get_amax();
void robot_set_acur(float aceleracion_entrada_en_curva);
float robot_get_acur();
void robot_set_afin(float aceleracion_frenada_final);
float robot_get_afin();
void robot_set_vr(float velocidad_en_recta);
float robot_get_vr();
void robot_set_vc(float velocidad_en_curva);
float robot_get_vc();

void robot_init();
void robot_siguiente_accion();
void robot_control();
void robot_inicia_exploracion();

uint8_t robot_get_casilla();
float robot_get_casilla_offset();
tipo_estado robot_get_estado();
tipo_orientacion robot_get_orientacion();
int32_t robot_get_pasos_recorridos();

float _distancia_para_decelerar(float velocidad, float aceleracion);

#endif /* ifndef ROBOT_H

 */
