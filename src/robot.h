
#ifndef __ROBOT_H
#define __ROBOT_H

#define ROBOT_PARADO 0
#define ROBOT_EXPLORANDO 1

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
uint8_t robot_get_accion();

int8_t robot_get_estado();
void robot_set_estado(int8_t estado);

float _distancia_para_decelerar(float velocidad, float aceleracion);

#endif /* ifndef ROBOT_H

 */
