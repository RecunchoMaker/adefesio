/**
 * @file robot.h
 *
 * @brief Cabecera de robot.cpp
 */

#ifndef __ROBOT_H
#define __ROBOT_H

enum tipo_orientacion {NORTE, ESTE, SUR, OESTE};
enum tipo_estado {PARADO, FLOOD, REORIENTA, ESPERANDO, DECIDE, AVANZANDO, FIN, ESPERANDO_RESOLUCION};


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
void robot_resuelve();

uint8_t robot_get_casilla();
float robot_get_casilla_offset();
int16_t robot_get_ultima_diferencia_encoders();
tipo_estado robot_get_estado();
tipo_orientacion robot_get_orientacion();
int32_t robot_get_pasos_recorridos();
float robot_get_angulo_desvio();
bool robot_es_valido_led_izquierdo();
bool robot_es_valido_led_derecho();

float _distancia_para_decelerar(float velocidad, float aceleracion);

#endif /* ifndef ROBOT_H

 */
