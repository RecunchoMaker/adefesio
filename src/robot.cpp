#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>

volatile float posicion_x;
volatile float posicion_y;

volatile float posicion_x_inicial;
volatile float posicion_y_inicial;

volatile int8_t orientacion;

void robot_set_posicion(float x, float y) {
    posicion_x = x;
    posicion_y = y;
    posicion_x_inicial = x;
    posicion_y_inicial = y;
    encoders_reset_posicion_total();
}

void robot_set_orientacion(int8_t o) {
    orientacion = o;
}

float robot_get_posicion_x() {
    return posicion_x;
}

float robot_get_posicion_y() {
    return posicion_y;
}

int8_t robot_get_orientacion() {
    return orientacion;
}

void robot_ir_a(float x, float y, float radio) {

    if (radio == RECTO) {

        // Estoy en la orientacion correcta?
        if (posicion_x == x) {
            if ((posicion_y < y and orientacion == NORTE)
                or
               (posicion_y > y and orientacion == SUR))
                posicion_x_inicial = posicion_x;
                posicion_y_inicial = posicion_y;
                encoders_reset_posicion_total();
                motores_set_velocidad(MAX_VELOCIDAD_LINEAL, 0);
        }

        if (posicion_y == y) {
            if ((posicion_x < x and orientacion == ESTE)
                or
               (posicion_x > x and orientacion == OESTE)) {
                posicion_x_inicial = posicion_x;
                posicion_y_inicial = posicion_y;
                encoders_reset_posicion_total();
                motores_set_velocidad(MAX_VELOCIDAD_LINEAL, 0);
            }
        }


    }
}

void robot_gira(int8_t o) {

    if (o == -1) {
        motores_set_velocidad(0.0, -MAX_VELOCIDAD_GIRO);
    } else {
        motores_set_velocidad(0.0, +MAX_VELOCIDAD_GIRO);
    }


}


void robot_actualiza_posicion() {
    if (motores_get_velocidad_angular_objetivo() == 0) {
        switch (orientacion) {
            case NORTE: posicion_y = posicion_y_inicial + LONGITUD_PASO_ENCODER * (encoders_get_posicion_total_left() + encoders_get_posicion_total_right()) / 2.0;
                        break;
            case SUR: posicion_y = posicion_y_inicial - LONGITUD_PASO_ENCODER * (encoders_get_posicion_total_left() + encoders_get_posicion_total_right()) / 2.0;
                        break;
            case ESTE: posicion_x = posicion_x_inicial + (LONGITUD_PASO_ENCODER * (encoders_get_posicion_total_left() + encoders_get_posicion_total_right()) / 2.0);
                        break;
            case OESTE: posicion_x = posicion_x_inicial - LONGITUD_PASO_ENCODER * (encoders_get_posicion_total_left() + encoders_get_posicion_total_right()) / 2.0;
                        break;
        }
    } else {
        if (motores_get_velocidad_angular_objetivo() < 0) {
            if (motores_get_angulo_actual() > 3*PI/2) orientacion = ESTE;
            else if (motores_get_angulo_actual() > PI) orientacion = SUR;
            else if (motores_get_angulo_actual() > PI/2) orientacion = OESTE;
            else if (motores_get_angulo_actual() > 0) orientacion = NORTE;
        } else {
            if (motores_get_angulo_actual() > 0) orientacion = OESTE;
            else if (motores_get_angulo_actual() > PI/2) orientacion = NORTE;
            else if (motores_get_angulo_actual() > PI) orientacion = ESTE;
            else if (motores_get_angulo_actual() > 3*PI/2) orientacion = SUR;
        }
    }
}

void robot_parar() {
    motores_set_velocidad(0.0, 0);
}
