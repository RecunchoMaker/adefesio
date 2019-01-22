#ifndef __LOG_H
#define __LOG_H

#define LOG(x) Serial.print(x); Serial.print(" ");
#define LOGF(x,d) Serial.print(x,d); Serial.print(" ");
#define LOGN(x) Serial.println(x)
#define LOGFN(x,d) Serial.print(x,d); Serial.println();

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
#include <robot.h>

void robot_log_estado_cabecera() {
    Serial.println("x y orientacion angulo");
}

void robot_log_estado() {
    LOGF(robot_get_posicion_x(), 5);
    LOGF(robot_get_posicion_y(), 5);
    LOG(robot_get_orientacion());
    LOGFN(motores_get_angulo_actual() * 360 / (2*PI),2);
}
#endif

#endif /* ifndef LOG_H

 */

