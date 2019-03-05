#include <Arduino.h>
#include <log.h>
#include <settings.h>
#include <motores.h>
#include <leds.h>
#include <timer1.h>

#define KA (2.0)

volatile float distancia_entre_ruedas = DISTANCIA_ENTRE_RUEDAS;

volatile float kp_lineal = KP_LINEAL;
volatile float kd_lineal = KD_LINEAL;
volatile float ki_lineal = KI_LINEAL;

volatile float kp_angular = -0.000;
volatile float error_angulo;

volatile float aceleracion_lineal = 0.0;

volatile float radio = 9999; // evita division por 0 en el init

volatile float potencia_left = 0;
volatile float potencia_right = 0;

volatile int16_t pwm_left;
volatile int16_t pwm_right;

volatile float velocidad_lineal_objetivo = 0;
volatile float velocidad_lineal_objetivo_left = 0;
volatile float velocidad_lineal_objetivo_right = 0;
volatile float velocidad_lineal_actual_left = 0;
volatile float velocidad_lineal_actual_right = 0;
volatile float velocidad_lineal_actual = 0;

volatile float velocidad_angular_objetivo = 0;

volatile double angulo_actual = 0;
volatile double angulo_actual_calculado = 0;

volatile float error_lineal_left = 0;
volatile float error_lineal_right = 0;

volatile float error_acumulado_left = 0;
volatile float error_acumulado_right = 0;

volatile double aux_e1 = 0;
volatile double aux_e2 = 0;

volatile float maximo_pwm = 0;

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

void motores_set_kp_lineal(float kp) {
    kp_lineal = kp;
}

void motores_set_kp_angular(float kp) {
    kp_angular = kp;
}

void motores_set_ki_lineal(float ki) {
    ki_lineal = ki;
}

void motores_set_kd_lineal(float kd) {
    kd_lineal = kd;
}

float motores_get_kp_lineal()  { return kp_lineal; }
float motores_get_kp_angular() { return kp_angular; }
float motores_get_ki_lineal()  { return ki_lineal; }
float motores_get_kd_lineal()  { return kd_lineal; }

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

void motores_set_maximo_pwm(int16_t pwm) {
    if (pwm > 255) pwm = 255;
    maximo_pwm = pwm;
}

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

int16_t motores_get_pwm_left() {
    return pwm_left;
}

int16_t motores_get_pwm_right() {
    return pwm_right;
}

void motores_set_velocidad_lineal_objetivo(float velocidad) {
    velocidad_lineal_objetivo = velocidad;
}

float motores_get_velocidad_lineal_objetivo() {
    return velocidad_lineal_objetivo;
}

float motores_get_velocidad_angular_objetivo() {
    return velocidad_angular_objetivo;
}

float motores_get_velocidad_lineal_objetivo_left() {
    return velocidad_lineal_objetivo_left;
}

float motores_get_velocidad_lineal_objetivo_right() {
    return velocidad_lineal_objetivo_right;
}

void motores_set_distancia_entre_ruedas(float dr) {
    distancia_entre_ruedas = dr;
}

float motores_get_distancia_entre_ruedas() {
    return distancia_entre_ruedas;
}

void motores_parar() {
    aceleracion_lineal = 0;
    velocidad_lineal_objetivo = 0;
    motores_set_potencia(0,0);
}

void motores_actualiza_velocidad() {

    if (velocidad_lineal_objetivo != 0 or aceleracion_lineal != 0) {

        velocidad_lineal_objetivo += (aceleracion_lineal * PERIODO_CICLO);
        if (velocidad_lineal_objetivo < 0)
            velocidad_lineal_objetivo = 0; // TODO fix cuando no da tiempo a decelerar

        if (radio == 0) { // caso especial. giro sobre si mismo
            velocidad_lineal_objetivo_left = -velocidad_lineal_objetivo;
            velocidad_lineal_objetivo_right = velocidad_lineal_objetivo;

            /*
            error_angulo = encoders_get_posicion_total_right() + encoders_get_posicion_total_left();

            velocidad_lineal_objetivo_left -= error_angulo * kp_angular;
            velocidad_lineal_objetivo_right += error_angulo * kp_angular;
            */

        } else if (radio < 1) { // suponemos que un radio superior a 1m es siempre una recta
            velocidad_angular_objetivo = velocidad_lineal_objetivo / radio;
            velocidad_lineal_objetivo_left = velocidad_angular_objetivo * (radio + distancia_entre_ruedas/2);
            velocidad_lineal_objetivo_right = velocidad_angular_objetivo * (radio - distancia_entre_ruedas/2);
        } else {

            velocidad_lineal_objetivo_left = velocidad_lineal_objetivo;
            velocidad_lineal_objetivo_right = velocidad_lineal_objetivo;

            // Suponemos que estamos en un pasillo
            potencia_left += 0.0001 * leds_get_desvio_centro();
            potencia_right -= 0.0001 * leds_get_desvio_centro();
        

            /*
            error_angulo = encoders_get_posicion_total_left() - encoders_get_posicion_total_right();

            velocidad_lineal_objetivo_left += error_angulo * kp_angular;
            velocidad_lineal_objetivo_right -= error_angulo * kp_angular;
            */
        }

        error_lineal_left = encoders_get_ultima_velocidad_left() - velocidad_lineal_objetivo_left;
        error_lineal_right = encoders_get_ultima_velocidad_right() - velocidad_lineal_objetivo_right;

        potencia_left += kp_lineal * error_lineal_left; 
        potencia_left += kd_lineal * ((error_lineal_left - error_acumulado_left) / PERIODO_CICLO);
        potencia_left += ki_lineal * error_acumulado_left;

        potencia_right += kp_lineal * error_lineal_right;
        potencia_right += kd_lineal * ((error_lineal_right - error_acumulado_right) / PERIODO_CICLO);
        potencia_right += ki_lineal * error_acumulado_right;


        error_acumulado_left = error_lineal_left;
        error_acumulado_right = error_lineal_right;

#ifdef MOTORES_LOG_PID
        // if (1) {
        if (timer1_get_cuenta() % 5 == 1) {
        // if (timer1_get_cuenta() > 0.5 * (1.0/PERIODO_CICLO)) { // esperamos 1 segundo
        log_insert(
                encoders_get_ultima_velocidad_left(),
                // velocidad_lineal_objetivo + (velocidad_angular_objetivo * PI * distancia_entre_ruedas / 2  ),
                velocidad_lineal_objetivo + (velocidad_angular_objetivo * PI * distancia_entre_ruedas / 2  ),
           //     velocidad_lineal_objetivo,
           //
                // error_lineal_left,
                encoders_get_ultima_velocidad_right(),

                error_acumulado_left,
                kp_lineal * error_lineal_left,
                kd_lineal * ((error_lineal_right - error_acumulado_right) / PERIODO_CICLO),
                ki_lineal * error_acumulado_left,
                //pwm_left,
                potencia_left,
                //encoders_get_ticks_sin_actualizar_left()
                encoders_get_posicion_left()
                );
        }
#endif

        motores_set_potencia(potencia_left, potencia_right);

    } else {
       motores_set_potencia(0, 0);
    }
}

void motores_set_aceleracion_lineal(float aceleracion) {
    aceleracion_lineal = aceleracion;
}

float motores_get_aceleracion_lineal() {
    return aceleracion_lineal;
}


void motores_set_radio(float r) {
    radio = r;
}

double motores_get_angulo_actual() {
    return angulo_actual;
}

void motores_set_angulo_actual(double angulo) {
    angulo_actual = angulo;
}

double motores_get_angulo_actual_calculado() {
    return angulo_actual_calculado;
}

void motores_actualiza_angulo() {

    /*
     * Forma 1 de calcular el angulo, presuponiendo que actualiza_velicidad()
     * funciona perfectamente
     *
    angulo_actual_calculado += velocidad_angular_objetivo * PERIODO_CICLO;
    */

    /*
     * Forma 2 de calcular el angulo
     *
    aux_e1 = encoders_get_ultima_velocidad_left() * PERIODO_CICLO;
    aux_e2 = encoders_get_ultima_velocidad_right() * PERIODO_CICLO;
        angulo_actual_calculado += (aux_e1 - aux_e2) / distancia_entre_ruedas;
    */

    if (encoders_get_posicion_left() != encoders_get_posicion_right()) {
        angulo_actual+= (LONGITUD_PASO_ENCODER * encoders_get_posicion_left() -
                         LONGITUD_PASO_ENCODER * encoders_get_posicion_right())
                         / 
                         distancia_entre_ruedas;
        if (angulo_actual < 0) angulo_actual+=2*PI;
        else if (angulo_actual>2*PI) angulo_actual -= 2*PI;
    }

}
