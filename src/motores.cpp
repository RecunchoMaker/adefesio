/**
 * @file motores.cpp
 *
 * @brief Control de motores
 */
#include <Arduino.h>
#include <log.h>
#include <settings.h>
#include <motores.h>
#include <leds.h>
#include <robot.h>
#include <timer1.h>

/// Distancia entre las ruedas, en metros
volatile float distancia_entre_ruedas = DISTANCIA_ENTRE_RUEDAS;

volatile float kp_lineal = KP_LINEAL; ///< Parámetro P para pid de velocidad lineal
volatile float kd_lineal = KD_LINEAL; ///< Parámetro D para pid de velocidad lineal
volatile float ki_lineal = KI_LINEAL; ///< Parámetro I para pid de velocidad lineal

volatile float kp_pasillo = KP_PASILLO; ///< Parametro P para corrección entre pasillos
volatile float ki_pasillo = KI_PASILLO; ///< Parametro I para corrección entre pasillos
//@{
/// @name Variables para control de pwm

volatile float potencia_left = 0;  ///< Valor entre 0 y 1 proporcinal a la potencia del motor izquierdo
volatile float potencia_right = 0; ///< Valor entre 0 y 1 proporcinal a la potencia del motor derecho

volatile int16_t pwm_left;         ///< Valor pwm proporcionado al motor izquierdo
volatile int16_t pwm_right;        ///< Valor pwm proporcionado al motor derecho
volatile float maximo_pwm = 0;     ///< Máximo pwm aplicable a los motores
//@}

//@{
/// @name Velocidades y aceleraciones 
volatile float velocidad_lineal_objetivo = 0;
volatile float velocidad_lineal_objetivo_left = 0;
volatile float velocidad_lineal_objetivo_right = 0;

volatile float velocidad_lineal_actual = 0;
volatile float velocidad_lineal_actual_left = 0;
volatile float velocidad_lineal_actual_right = 0;

volatile float aceleracion_lineal = 0.0;
volatile float velocidad_angular_objetivo = 0;
//@}

/// Rado de giro
volatile float radio = 99999; // evita division por 0 en el init


//@{
/// @name Variables para pid
volatile float error_lineal_left = 0;
volatile float error_lineal_right = 0;

volatile float error_acumulado_left = 0;
volatile float error_acumulado_right = 0;
//@}


//@{
/// @name setters y getters
float motores_get_kp_lineal()  { return kp_lineal; }
void motores_set_kp_lineal(float kp) { kp_lineal = kp; }

float motores_get_ki_lineal()  { return ki_lineal; }
void motores_set_ki_lineal(float ki) { ki_lineal = ki; }

float motores_get_kd_lineal()  { return kd_lineal; }
void motores_set_kd_lineal(float kd) { kd_lineal = kd; }

float motores_get_kp_pasillo()  { return kp_pasillo; }
void motores_set_kp_pasillo(float kp) { kp_pasillo = kp; }

float motores_get_ki_pasillo()  { return ki_pasillo; }
void motores_set_ki_pasillo(float kp) { ki_pasillo = kp; }

float motores_get_velocidad_lineal_objetivo() {
    return velocidad_lineal_objetivo;
}
void motores_set_velocidad_lineal_objetivo(float velocidad) {
    velocidad_lineal_objetivo = velocidad;
}
float motores_get_velocidad_lineal_objetivo_left() {
    return velocidad_lineal_objetivo_left;
}
float motores_get_velocidad_lineal_objetivo_right() {
    return velocidad_lineal_objetivo_right;
}

int16_t motores_get_pwm_left() { return pwm_left; }
int16_t motores_get_pwm_right() { return pwm_right; }

void motores_set_maximo_pwm(int16_t pwm) {
    if (pwm > 255) pwm = 255;
    maximo_pwm = pwm;
}

float motores_get_velocidad_angular_objetivo() {
    return velocidad_angular_objetivo;
}

float motores_get_distancia_entre_ruedas() {
    return distancia_entre_ruedas;
}

void motores_set_distancia_entre_ruedas(float dr) {
    distancia_entre_ruedas = dr;
}

float motores_get_aceleracion_lineal() {
    return aceleracion_lineal;
}
void motores_set_aceleracion_lineal(float aceleracion) {
    aceleracion_lineal = aceleracion;
}

void motores_set_radio(float r) {
    radio = r;
}
//@}

/**
 * @brief Inicializa pines para el control de motor
 *
 * @param voltaje Voltaje proporcionado por la batería
 *
 * Define los pines digitales IN1 e IN2 para cada motor, que controlan la dirección
 * en la que giran, y los pines PWM y se establece el máximo pwm que se puede
 * aplicar a los motores en función de la carga de la batería.
 *
 * @see https://recunchomaker.github.io/adefesio//motores-y-puente-h/
 * @see https://recunchomaker.github.io/adefesio/normalizar-pwm-en-funcion-bateria/
 */
void motores_init(float voltaje) {

    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_LEFT_PWM, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_PWM, OUTPUT);

    motores_set_maximo_pwm((int16_t) (255.0 * 6.0 / voltaje));

    motores_set_potencia(0,0);
}


/**
 * @brief Establece un pwm en cada motor con valores normalizados entre 0 y 1
 *
 * @param left Potencia a entregar al motor izquierdo
 * @param right Potencia a entregar al motor derecho
 */

void motores_set_potencia(float left, float right) {
    if (left > 1) left = 1;
    if (right > 1) right = 1;
    if (left < -1) left = -1;
    if (right < -1) right = -1;

    motores_set_pwm_left((int16_t) (left * maximo_pwm));
    motores_set_pwm_right((int16_t) (right * maximo_pwm));
    potencia_left = left;
    potencia_right = right;
    encoders_set_direccion(potencia_left > 0, potencia_right > 0);
}


/**
 * @brief Estabece el pwm del motor derecho
 *
 * @param right Valor de pwm entre -255 y 255
 *
 * Se configuran los pines IN1 e IN2 en función del sentido de giro
 * y se escribe el valor absoluto del parámetro en el pin de pwm
 */
void motores_set_pwm_right(int16_t right) {
    if (right > 0) {
        if (right > maximo_pwm) right = maximo_pwm;
        digitalWrite(MOTOR_RIGHT_IN1, HIGH);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
        analogWrite(MOTOR_RIGHT_PWM, right);
        pwm_right = right;
    } else {
        if (right < -maximo_pwm) right = -maximo_pwm;
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, HIGH);
        analogWrite(MOTOR_RIGHT_PWM, -right);
        pwm_right = right;
    }
}


/**
 * @brief Estabece el pwm del motor izquierdo 
 *
 * @param right Valor de pwm entre -255 y 255
 *
 * Se configuran los pines IN1 e IN2 en función del sentido de giro
 * y se escribe el valor absoluto del parámetro en el pin de pwm
 */
void motores_set_pwm_left(int16_t left) {

    if (left > 0) {
        if (left > maximo_pwm) left = maximo_pwm;
        digitalWrite(MOTOR_LEFT_IN1, HIGH);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
        analogWrite(MOTOR_LEFT_PWM, left);
        pwm_left = left;
    } else {
        if (left < -maximo_pwm) left = -maximo_pwm;
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        digitalWrite(MOTOR_LEFT_IN2, HIGH);
        analogWrite(MOTOR_LEFT_PWM, -left);
        pwm_left = left;
    }
}


/**
 * @brief Resetea velocidades y aceleraciones y para los motores
 */
void motores_parar() {
    aceleracion_lineal = 0;
    velocidad_lineal_objetivo = 0;
    motores_set_potencia(0,0);
    encoders_reset_posicion_total();
}


/**
 * @brief Establece la potencia en cada motor en función de varios parámetros
 *
 * Se recalculan las velocidades de cada motor en función de:
 *
 * - velocidad_objetivo
 * - aceleracion_lineal
 * - radio
 * - distancia entre ruedas
 * - información de sensores
 * - información de encoders
 *
 * Se establece un PID que se alimenta de la información de los encoders para obtener
 * la diferencia de velocidad actual con la objetivo, y se corrige con información
 * de los sensores.
 *
 * @todo Sustituir el tratamiento de radio por velocidad angular
 */
void motores_actualiza_velocidad() {

    if (velocidad_lineal_objetivo != 0 or aceleracion_lineal != 0) {

        // Actualizamos la velocidad lineal
        velocidad_lineal_objetivo += (aceleracion_lineal * PERIODO_CICLO);

        /// @todo Corregir la necesidad de comprobar si la velocidad objetivo es menor que 0
        if (velocidad_lineal_objetivo < 0) // fix this
            velocidad_lineal_objetivo = 0;

        if (radio == GIRO_IZQUIERDA_TODO ) {
            // El radio es una constante especial para indicar giro sobre sí mismo
            velocidad_lineal_objetivo_left = -velocidad_lineal_objetivo;
            velocidad_lineal_objetivo_right = velocidad_lineal_objetivo;
        } else if (radio == GIRO_DERECHA_TODO) {
            // El radio es una constante especial para indicar giro sobre sí mismo
            velocidad_lineal_objetivo_left = velocidad_lineal_objetivo;
            velocidad_lineal_objetivo_right = -velocidad_lineal_objetivo;
        } else if (radio < 1) { 
            velocidad_angular_objetivo = velocidad_lineal_objetivo / radio;
            velocidad_lineal_objetivo_left = velocidad_angular_objetivo * (radio + distancia_entre_ruedas/2);
            velocidad_lineal_objetivo_right = velocidad_angular_objetivo * (radio - distancia_entre_ruedas/2);
        } else {
            // suponemos que un radio a 1m es siempre una recta
            velocidad_lineal_objetivo_left = velocidad_lineal_objetivo;
            velocidad_lineal_objetivo_right = velocidad_lineal_objetivo;

            // Suponemos que estamos en un pasillo
            
            potencia_left += kp_pasillo * robot_get_desvio_centro();
            potencia_right -= kp_pasillo * robot_get_desvio_centro();


        }

        error_lineal_left = encoders_get_ultima_velocidad_left() - velocidad_lineal_objetivo_left;
        error_lineal_right = encoders_get_ultima_velocidad_right() - velocidad_lineal_objetivo_right;

        // Cálculos de pid
        potencia_left += kp_lineal * error_lineal_left; 
        potencia_left += kd_lineal * ((error_lineal_left - error_acumulado_left) / PERIODO_CICLO);
        potencia_left += ki_lineal * error_acumulado_left;

        potencia_right += kp_lineal * error_lineal_right;
        potencia_right += kd_lineal * ((error_lineal_right - error_acumulado_right) / PERIODO_CICLO);
        potencia_right += ki_lineal * error_acumulado_right;

        error_acumulado_left = error_lineal_left;
        error_acumulado_right = error_lineal_right;

        motores_set_potencia(potencia_left, potencia_right);

    } else {
       motores_set_potencia(0, 0);
    }
}

