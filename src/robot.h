
#ifndef __ROBOT_H
#define __ROBOT_H

struct tipo_accion {
    volatile float distancia;
    volatile int32_t pasos_objetivo;
    volatile int32_t pasos_hasta_decelerar;
    volatile float aceleracion;
    volatile float deceleracion;
    volatile float velocidad_maxima;
    volatile float velocidad_final;
    volatile float radio;
};

enum tipo_orientacion {NORTE, ESTE, SUR, OESTE};
enum tipo_estado {PARADO, PARADO_PARA_GIRO, PAUSA_PRE_GIRO, EXPLORANDO_INICIAL, PAUSA_INICIAL, EXPLORANDO, VUELTA, TEST};


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

// TODO: esta funcion es privada
void _crea_accion(float distancia,
                  float aceleracion, float deceleracion,
                  float velocidad_maxima, float velocidad_final,
                  float radio); 

void robot_init();
void robot_siguiente_accion();
void robot_control();
void robot_inicia_exploracion();
tipo_accion robot_get_accion();

uint8_t robot_get_casilla();
float robot_get_casilla_offset();
tipo_estado robot_get_estado();
tipo_orientacion robot_get_orientacion();

float _distancia_para_decelerar(float velocidad, float aceleracion);

#endif /* ifndef ROBOT_H

 */
