#include <Arduino.h>
#include <log.h>
#include <settings.h>
#include <motores.h>
#include <timer1.h>

#define KA (2.0)

volatile float kp_lineal = -0.6;
volatile float kd_lineal = -0.0;
volatile float ki_lineal = -0.0;


volatile float maxima_velocidad_lineal = MAX_VELOCIDAD_LINEAL;
volatile float maxima_aceleracion_lineal = MAX_ACELERACION_LINEAL;
volatile float maxima_velocidad_angular = MAX_VELOCIDAD_ANGULAR;
volatile float maxima_aceleracion_angular = MAX_ACELERACION_ANGULAR;

volatile float aceleracion_lineal = 0.0;
volatile float aceleracion_angular = 0.0;

volatile float potencia_left = 0;
volatile float potencia_right = 0;

volatile int16_t pwm_left;
volatile int16_t pwm_right;

volatile float velocidad_lineal_objetivo = 0;
volatile float velocidad_lineal_objetivo_temp = 0;
volatile float velocidad_lineal_actual_left = 0;
volatile float velocidad_lineal_actual_right = 0;
volatile float velocidad_lineal_actual = 0;

volatile float velocidad_angular_objetivo_temp = 0;

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
    motores_set_maxima_velocidad_lineal(MAX_VELOCIDAD_LINEAL);
    motores_set_maxima_aceleracion_lineal(MAX_ACELERACION_LINEAL);

    motores_set_potencia(0,0);
}

void motores_set_kp_lineal(float kp) {
    kp_lineal = kp;
}

void motores_set_ki_lineal(float ki) {
    ki_lineal = ki;
}

void motores_set_kd_lineal(float kd) {
    kd_lineal = kd;
}

float motores_get_kp_lineal() { return kp_lineal; }
float motores_get_ki_lineal() { return ki_lineal; }
float motores_get_kd_lineal() { return kd_lineal; }

void motores_set_potencia(float left, float right) {
    if (left > 1) left = 1;
    if (right > 1) right = 1;
    if (left < -1) left = -1;
    if (right < -1) right = -1;
    motores_set_pwm_left((int16_t) (left * maximo_pwm));
    motores_set_pwm_right((int16_t) (right * maximo_pwm));
    potencia_left = left;
    potencia_right = right;
}

void motores_set_maximo_pwm(int16_t pwm) {
    if (pwm > 255) pwm = 255;
    maximo_pwm = pwm;
}

void motores_set_pwm_right(int16_t right) {
    if (right > 0) {
        if (right > MAX_PWM) right = MAX_PWM;
        digitalWrite(MOTOR_RIGHT_IN1, HIGH);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
        analogWrite(MOTOR_RIGHT_PWM, right);
        pwm_right = right;
    } else {
        if (right < -MAX_PWM) right = -MAX_PWM;
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, HIGH);
        analogWrite(MOTOR_RIGHT_PWM, -right);
        pwm_right = right;
    }
}

void motores_set_pwm_left(int16_t left) {

    if (left > 0) {
        if (left > MAX_PWM) left = MAX_PWM;
        digitalWrite(MOTOR_LEFT_IN1, HIGH);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
        analogWrite(MOTOR_LEFT_PWM, left);
        pwm_left = left;
    } else {
        if (left < -MAX_PWM) left = -MAX_PWM;
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

float motores_get_velocidad_lineal_objetivo_temp() {
    return velocidad_lineal_objetivo_temp;
}

float motores_get_velocidad_angular_objetivo_temp() {
    return velocidad_angular_objetivo_temp;
}

void motores_actualiza_velocidad() {

    if (velocidad_lineal_objetivo_temp != 0 or aceleracion_lineal != 0 or velocidad_angular_objetivo_temp != 0 or aceleracion_angular != 0) {
        velocidad_lineal_objetivo_temp += (aceleracion_lineal * PERIODO_TIMER);
        velocidad_angular_objetivo_temp += (aceleracion_angular * PERIODO_TIMER);

        // TODO: Este control ya no haria falta. Lo hace el planificador
        /*
        if (velocidad_lineal_objetivo_temp > velocidad_lineal_objetivo)
            velocidad_lineal_objetivo_temp = velocidad_lineal_objetivo;
        */

        error_lineal_left = encoders_get_ultima_velocidad_left() - 
           (velocidad_lineal_objetivo_temp + (velocidad_angular_objetivo_temp * PI * DISTANCIA_ENTRE_RUEDAS / 2  ));
        error_lineal_right = encoders_get_ultima_velocidad_right() -
           (velocidad_lineal_objetivo_temp - (velocidad_angular_objetivo_temp * PI * DISTANCIA_ENTRE_RUEDAS / 2  ));

        /* no acumulamos todos los errores, sino que guardamos el último de ellos (mas abajo)
        error_acumulado_left += error_lineal_left;
        error_acumulado_right += error_lineal_right;
        */

        /* no utilizamos KA
        potencia_left = KA * (velocidad_lineal_objetivo_temp + (velocidad_angular_objetivo_temp * DISTANCIA_ENTRE_RUEDAS / 2));
        */
        potencia_left += kp_lineal * error_lineal_left; 
        potencia_left += kd_lineal * ((error_lineal_left - error_acumulado_left) / PERIODO_TIMER);
        potencia_left += ki_lineal * error_acumulado_left;

        //potencia_right = KA * (velocidad_lineal_objetivo_temp - (velocidad_angular_objetivo_temp * DISTANCIA_ENTRE_RUEDAS / 2));
        potencia_right += kp_lineal * error_lineal_right;
        potencia_right += kd_lineal * ((error_lineal_right - error_acumulado_right) / PERIODO_TIMER);
        potencia_right += ki_lineal * error_acumulado_right;

        encoders_set_direccion(potencia_left > 0, potencia_right > 0);

#ifdef MOTORES_LOG_PID
        // if (1) {
        if (timer1_get_cuenta() % 5 == 1) {
        // if (timer1_get_cuenta() > 0.5 * (1.0/PERIODO_TIMER)) { // esperamos 1 segundo
        log_insert(
                encoders_get_ultima_velocidad_left(),
                // velocidad_lineal_objetivo_temp + (velocidad_angular_objetivo_temp * PI * DISTANCIA_ENTRE_RUEDAS / 2  ),
                velocidad_lineal_objetivo_temp + (velocidad_angular_objetivo_temp * PI * DISTANCIA_ENTRE_RUEDAS / 2  ),
           //     velocidad_lineal_objetivo_temp,
           //
                // error_lineal_left,
                encoders_get_ultima_velocidad_right(),

                error_acumulado_left,
                kp_lineal * error_lineal_left,
                kd_lineal * ((error_lineal_right - error_acumulado_right) / PERIODO_TIMER),
                ki_lineal * error_acumulado_left,
                //pwm_left,
                potencia_left,
                //encoders_get_ticks_sin_actualizar_left()
                encoders_get_posicion_left()
                );
        }
#endif

        error_acumulado_left = error_lineal_left;
        error_acumulado_right = error_lineal_right;

        motores_set_potencia(potencia_left, potencia_right);

    } else {
        motores_set_potencia(0, 0);
    }
}

void motores_set_maxima_velocidad_lineal(float velocidad) {
    maxima_velocidad_lineal = velocidad;
}

float motores_get_maxima_velocidad_lineal() {
    return maxima_velocidad_lineal;
}

void motores_set_maxima_aceleracion_lineal(float aceleracion) {
    maxima_aceleracion_lineal = aceleracion;
}

float motores_get_maxima_aceleracion_lineal() {
    return maxima_aceleracion_lineal;
}

void motores_set_aceleracion_lineal(float aceleracion) {
    aceleracion_lineal = aceleracion;
}

float motores_get_aceleracion_lineal() {
    return aceleracion_lineal;
}


void motores_set_maxima_velocidad_angular(float velocidad) {
    maxima_velocidad_angular = velocidad;
}

float motores_get_maxima_velocidad_angular() {
    return maxima_velocidad_angular;
}

void motores_set_maxima_aceleracion_angular(float aceleracion) {
    maxima_aceleracion_angular = aceleracion;
}

float motores_get_maxima_aceleracion_angular() {
    return maxima_aceleracion_angular;
}

void motores_set_aceleracion_angular(float aceleracion) {
    aceleracion_angular = aceleracion;
}

float motores_get_aceleracion_angular() {
    return aceleracion_angular;
}

void motores_set_velocidad(float velocidad_lineal, float velocidad_angular) {

    if (velocidad_lineal > motores_get_maxima_velocidad_lineal())
        velocidad_lineal_objetivo = motores_get_maxima_velocidad_lineal();
    else {
        // velocidad_lineal_objetivo contiene la ultima velocidad seteada
        // en esta función
        velocidad_lineal_objetivo_temp = velocidad_lineal_objetivo;
        velocidad_lineal_objetivo = velocidad_lineal;
    }

    velocidad_angular_objetivo_temp = velocidad_angular;

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
    angulo_actual_calculado += velocidad_angular_objetivo_temp * PERIODO_TIMER;
    */

    /*
     * Forma 2 de calcular el angulo
     *
    aux_e1 = encoders_get_ultima_velocidad_left() * PERIODO_TIMER;
    aux_e2 = encoders_get_ultima_velocidad_right() * PERIODO_TIMER;
        angulo_actual_calculado += (aux_e1 - aux_e2) / DISTANCIA_ENTRE_RUEDAS;
    */

    if (encoders_get_posicion_left() != encoders_get_posicion_right()) {
        angulo_actual+= (LONGITUD_PASO_ENCODER * encoders_get_posicion_left() -
                         LONGITUD_PASO_ENCODER * encoders_get_posicion_right())
                         / 
                         DISTANCIA_ENTRE_RUEDAS;
        if (angulo_actual < 0) angulo_actual+=2*PI;
        else if (angulo_actual>2*PI) angulo_actual -= 2*PI;
    }

}
