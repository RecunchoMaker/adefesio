#ifndef __LOG_H
#define __LOG_H

#define LOG(x) Serial.print(x); Serial.print(" ");
#define LOGF(x,d) Serial.print(x,d); Serial.print(" ");
#define LOGN(x) Serial.println(x)
#define LOGFN(x,d) Serial.print(x,d); Serial.println();

#include <leds.h>
void log_leds();

#ifdef ENCODERS_LOG_ESTADO
#include <encoders.h>
#include <motores.h>

void encoders_log_estado_cabecera() {
    Serial.println("velocidadLeft velocidadRight pwmLeft pwmRight posLeft posRight velocidadObjetivo anguloActual anguloCalculado ticksSinAct");
}

void encoders_log_estado() {
    LOGF(encoders_get_velocidad_left(),5);
    LOGF(encoders_get_velocidad_right(),5);
    LOG(motores_get_pwm_left());
    LOG(motores_get_pwm_right());
    LOG(encoders_get_posicion_total_left());
    LOG(encoders_get_posicion_total_right());
    LOGF(motores_get_velocidad_lineal_objetivo(),5);
    LOGF(motores_get_angulo_actual() * 360 / (2*PI),5);
    LOGF(motores_get_angulo_actual_calculado() * 360 / (2*PI),5);
    LOGN(encoders_get_ticks_sin_actualizar_right());
}
#endif

#ifdef ROBOT_LOG_ESTADO

void robot_log_estado_cabecera();
void robot_log_estado();
#endif


#ifdef MOTORES_LOG_PID

void log_insert( float _ultima_velocidad, float _velocidad_lineal_objetivo, float _error_lineal_left,
       float _error_acumulado_left, float _aux1, float _aux2, float _aux3, 
       float _potencia, uint8_t _ticks);
void log_print();
void log_start();

#endif /* MOTORES_LOG_PID */


#ifdef MOTORES_LOG_PID_RESUMEN
void log_cabecera();
void log_linea();
#endif /* MOTORES_LOG_PID_RESUMEN */

#endif /* ifndef LOG_H

 */

