#include <Arduino.h>
#include <log.h>

#include <leds.h>
void log_leds() {
    Serial.print("leds:");
    Serial.print(leds_get_valor(LED_IZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FIZQ));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_FDER));
    Serial.print("\t");
    Serial.print(leds_get_valor(LED_DER));
    Serial.print("\t");
}



#ifdef ROBOT_LOG_ESTADO
#include <robot.h>
#include <motores.h>

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

#ifdef MOTORES_LOG_PID_RESUMEN
#include <encoders.h>
#include <motores.h>
void log_cabecera() {
    Serial.println("vlineal vlinealLeft vlinealRight vEncoderLeft vEncoderRight pwmLeft pwmRight posLeft posRight");
}

void log_linea() {
    LOGF(motores_get_velocidad_lineal_objetivo(),5)
    LOGF(motores_get_velocidad_lineal_objetivo_left(),5)
    LOGF(motores_get_velocidad_lineal_objetivo_right(),5)
    LOGF(encoders_get_ultima_velocidad_left(), 5);
    LOGF(encoders_get_ultima_velocidad_right(), 5);
    LOG(motores_get_pwm_left());
    LOG(motores_get_pwm_right());
    LOG(encoders_get_posicion_total_left());
    LOGN(encoders_get_posicion_total_right());
}
#endif

#ifdef MOTORES_LOG_PID
#include <encoders.h>

#define MAX_LOG_MEM 35
volatile uint8_t index = MAX_LOG_MEM + 1;
volatile float a_ultima_velocidad[MAX_LOG_MEM];
volatile float a_velocidad_lineal_objetivo[MAX_LOG_MEM];
volatile float a_error_lineal_left[MAX_LOG_MEM];
volatile float a_error_acumulado_left[MAX_LOG_MEM];
volatile float a_aux1[MAX_LOG_MEM];
volatile float a_aux2[MAX_LOG_MEM];
volatile float a_aux3[MAX_LOG_MEM];
volatile float a_potencia[MAX_LOG_MEM];
volatile uint8_t a_ticks[MAX_LOG_MEM];

void log_insert(
        float _ultima_velocidad,
        float _velocidad_lineal_objetivo,
        float _error_lineal_left,
        float _error_acumulado_left,
        float _aux1,
        float _aux2,
        float _aux3,
        float _potencia,
        uint8_t _ticks
        )
{
    if (index < MAX_LOG_MEM) {

        a_ultima_velocidad[index] = _ultima_velocidad;
        a_velocidad_lineal_objetivo[index] = _velocidad_lineal_objetivo;
        a_error_lineal_left[index] = _error_lineal_left;
        a_error_acumulado_left[index] = _error_acumulado_left;
        a_aux1[index] = _aux1;
        a_aux2[index] = _aux2;
        a_aux3[index] = _aux3;
        a_ticks[index] = _ticks;
        a_potencia[index] = _potencia;
        index++;
    }
}

void log_print() {
    if (index == MAX_LOG_MEM) {
        Serial.println("ultimaVel velObjetivo errorLeft errorAcumulado kp kd ki potencia encoderPos");
        for (uint8_t i = 0; i < MAX_LOG_MEM; i++) {
           LOGF(a_ultima_velocidad[i],5);
           LOGF(a_velocidad_lineal_objetivo[i],5);
           LOGF(a_error_lineal_left[i],5);
           LOGF(a_error_acumulado_left[i],5);
           if (a_aux1[i]>1) a_aux1[i] = 1.5;
           LOGF(a_aux1[i],5);
           if (a_aux1[2]>1) a_aux1[i] = 1.5;
           LOGF(a_aux2[i],5);
           if (a_aux1[3]>1) a_aux1[i] = 1.5;
           LOGF(a_aux3[i],5);
           if (a_potencia[i] > 1) a_potencia[i] = 1;
           LOGF(a_potencia[i],5);
           LOGN(a_ticks[i]);
        }
        index++; // para parar de logear
    }
}

void log_start() {
    index = 0;
}

#endif /* MOTORES_LOG_PID */
        
