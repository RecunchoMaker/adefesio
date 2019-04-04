/**
 * @file accion.cpp
 *
 * @brief Funciones para el control de acciones completas, que generalmente implican una casilla
 *
 * Una _acción_ se corresponde con una pequeña trayectoria en la que se establece una
 * _velocidad final_ y unas aceleraciones. Una acción suele completar (aunque no necesariamente)
 * una casilla. Por ejemplo:
 *
 * - En el arranque (con el robot situado en la mitad de la casilla), arrancar con una aceleración
 *   determinada hasta el final de dicha casilla
 * - Avanzar recto una casilla completa
 * - Hacer un giro en redondo, en el centro de una casilla
 *
 * Se pueden necesitar un conjunto de acciones en secuencia para entrar y salir en una casilla.
 * Por ejemplo: cuando el robot entra en una casilla con dos paredes laterales y una pared enfrente
 * (un callejon sin salida) es necesario:
 * - Ejecutar una acción que frene hasta detenerse en el medio de la casilla
 * - Ejecutar una acción que gire en redondo 180 grados
 * - Ejecutar una acción que arranque de nuevo hasta el final de la casilla
 *
 * Las funciones definidas aquí sólo sirven para definir las acciones y hacer los cálculos necesarios
 * (para no repetirlos constantemente en la interrupción de tiempo). Las acciones suelen necesitar un
 * cojunto de pasos de encoder para completarse, y sólo terminan cuando dichos pasos se completan, o
 * cuando un evento exterior da por terminada la acción.
 */

#include <accion.h>
#include <settings.h>
#include <motores.h>
#include <timer1.h>
#include <leds.h>
#include <log.h>
#include <robot.h>

volatile float distancia;               ///< Distancia en metros de la acción actual
volatile int32_t pasos_objetivo;        ///< Pasos de encoder necesarios para completar la acción actual
volatile int32_t pasos_hasta_decelerar; ///< Pasos a partir de los cuales hay que empezar a decelerar
volatile float aceleracion;             ///< Aceleración aplicada en caso de que la velocidad solicitada sea inferior a la actual
volatile float deceleracion;            ///< Deceleración aplicada a partir de que los pasos de encoder lleguen a pasos_objetivo
volatile float velocidad_maxima;        ///< Velocidad máxima de la acción
volatile float velocidad_final;         ///< Velocidad a la que debe terminar la acción
volatile float accion_radio;            ///< Radio de la acción (RADIO_INFINITO en caso de ser una recta)j

volatile float amax = ACCION_AMAX;      ///< Aceleración máxima, usada en las rectas
volatile float acur = ACCION_ACUR;      ///< Aceleración de frenada antes de entrar en una curva
volatile float afin = ACCION_AFIN;      ///< Aceleración de la frenada final en la última casilla
volatile float vr = ACCION_VR;          ///< Velocidad máxima en recta
volatile float vc = ACCION_VC;          ///< Velocidad máxima en curba
volatile float ve = ACCION_VE;          ///< Velocidad máxima en exploración
volatile float vg = ACCION_VG;          ///< Velocidad máxima giro en redondo

volatile tipo_accion accion_actual;

//@{
/**
 * @name Setters y getters
 */
tipo_accion accion_get_accion_actual() {
    return accion_actual;
}
float accion_get_distancia() {
    return distancia;
}
float accion_get_aceleracion() {
    return aceleracion;
}
float accion_get_deceleracion() {
    return deceleracion;
}
float accion_get_velocidad_final() {
    return velocidad_final;
}
float accion_get_velocidad_maxima() {
    return velocidad_maxima;
}
float accion_get_radio() {
    return accion_radio;
}
int32_t accion_get_pasos_objetivo() {
    return pasos_objetivo;
}
void accion_set_amax(float aceleracion_maxima) {
    amax = aceleracion_maxima;
}
float accion_get_amax() {
    return amax;
}
void accion_set_acur(float aceleracion_entrada_en_curva) {
    acur = aceleracion_entrada_en_curva;
}
float accion_get_acur() {
    return acur;
}
void accion_set_afin(float aceleracion_frenada_final) {
    afin = aceleracion_frenada_final;
}
float accion_get_afin() {
    return afin;
}
void accion_set_vr(float velocidad_en_recta) {
    vr = velocidad_en_recta;
}
float accion_get_vr() {
    return vr;
}
void accion_set_vc(float velocidad_en_curva) {
    vc = velocidad_en_curva;
}
float accion_get_vc() {
    return vc;
}
void accion_set_ve(float velocidad_en_exploracion) {
    ve = velocidad_en_exploracion;
}
float accion_get_ve() {
    return ve;
}
void accion_set_pasos_objetivo(int32_t pasos) {
    pasos_objetivo = pasos;
}
void accion_set_velocidad_final(int32_t pasos) {
    pasos_objetivo = pasos;
}
int32_t accion_get_pasos_hasta_decelerar() {
    return pasos_hasta_decelerar;
}
//@}


/**
 * @brief Calcula la distancia necesaria para decelerar a una velocidad final, dadas la inicial la aceleración
 */

float _distancia_para_decelerar(float vi, float vf, float aceleracion) {
    return ((0.5 * (vi - vf) * (vi - vf)) + (vf * (vi - vf))) / aceleracion;
}


/**
 * @brief Calcula las variables necesarias que definen una acción y la inicia
 *
 * @param dist distancia que se recorre en la acción
 * @param acel aceleración a aplicar si la velocidad actual es menor que la velocidad máxima de la acción
 * @param decel deceleración en caso de que la velocidad final sea menor que la máxima
 * @param vmax velocidad máxima de la acción
 * @param vfinal velocidad a la que finaliza la acción
 * @param radio radio en metros de la acción (en caso de rectas, utilizar RADIO_INFINITO)
 */
void accion_set(float dist,
                float acel, float dece,
                float vmax, float vfinal,
                float radio) {

    // la distancia puede ser negativa en los giros en redondo
    distancia = abs(dist);
    aceleracion = acel;

    pasos_objetivo = distancia / LONGITUD_PASO_ENCODER;
    deceleracion = dece;

    velocidad_maxima = vmax;
    velocidad_final = vfinal;
    pasos_hasta_decelerar = (
            (distancia -
             _distancia_para_decelerar (velocidad_maxima, velocidad_final, deceleracion)) /
              LONGITUD_PASO_ENCODER);
    accion_radio = radio;

    motores_set_radio(accion_radio);

    if (velocidad_maxima > motores_get_velocidad_lineal_objetivo()) {
        motores_set_aceleracion_lineal(aceleracion);
    }
    else
        motores_set_aceleracion_lineal(0);

    timer1_reset_cuenta();

    encoders_reset_posicion_total();
}


/**
 * @brief Inicia la acción especificada en el parámetro
 *
 * @param accion Tipo de acción a ejectuar
 */
void accion_ejecuta(tipo_accion accion) {

    accion_actual = accion;

    if (accion == ARRANCA) {
        Serial.print(F("* Arranca desde:"));
        Serial.println(robot_get_casilla_offset());
        encoders_reset_posicion_aux_total();
        accion_set(LABERINTO_LONGITUD_CASILLA-robot_get_casilla_offset(), amax, amax, ve, ve, RADIO_INFINITO);
    } else if (accion ==  PARA) {
        accion_set(LABERINTO_LONGITUD_CASILLA/2, amax, afin, ve, ACCION_V0, RADIO_INFINITO);

    } else if (accion == AVANZA) {
        Serial.println(F("* Continua recto"));
        accion_set(LABERINTO_LONGITUD_CASILLA, amax, amax, ve, ve, RADIO_INFINITO);
    } else if (accion == ESPERA) {
        Serial.println(F("* Pausa"));
        accion_set(0, 0, 0, 0, 0, 0.3); // en segundos
        Serial.println("leds: ");
        Serial.print(leds_get_distancia(LED_IZQ),9);
        Serial.print(" ");
        Serial.print(leds_get_distancia(LED_FIZQ),9);
        Serial.print(" ");
        Serial.print(leds_get_distancia(LED_FDER),9);
        Serial.print(" ");
        Serial.print(leds_get_distancia(LED_DER),9);
        Serial.println();

    } else if (accion == GIRA_DER) {
        Serial.println(F("* Gira derecha"));
        Serial.println(robot_get_ultima_diferencia_encoders());
        accion_set(-PI*motores_get_distancia_entre_ruedas()/4.0
               // + robot_get_ultima_diferencia_encoders() * LONGITUD_PASO_ENCODER
                ,amax, amax, vg, ACCION_V0, GIRO_DERECHA_TODO); // gira 90
    } else if (accion == GIRA_IZQ) {
        Serial.print(F("* Gira izquierda (comp: "));
        Serial.println(robot_get_ultima_diferencia_encoders());
        accion_set(PI*motores_get_distancia_entre_ruedas()/4.0
                //+ robot_get_ultima_diferencia_encoders() * LONGITUD_PASO_ENCODER
                ,amax, amax, vg , ACCION_V0, GIRO_IZQUIERDA_TODO); // gira 90
    } else if (accion == GIRA_180) {
        Serial.println(F("* Gira 180 grados"));
        accion_set(PI*motores_get_distancia_entre_ruedas()/2.0, amax, amax, vg, ACCION_V0, GIRO_IZQUIERDA_TODO); // gira 180g
    } else {
        Serial.println(F("No existe accion!"));
    }
}


/**
 * @brief Cancela una accion
 *
 * @param pasos Numero de pasos recorridos en el momento de la llamada
 *
 * Se llama a esta función cuando se detecta un evento que implica la cancelación
 * de la acción en curso. Por ejemplo, cuando se entra en una casilla y no se detecta
 * pared frontal, se ejecuta la acción correspondiente a avanzar una casilla completa.
 * Si en el medio de este avance se detecta la pared (debido a que los sensores no
 * consiguieron detectarla al inicio), se llama a esta función para
 * cancelar el avance y poder decidir la siguiente acción.
 *
 * @todo Es necesario realmente igualar los pasos objetivo?
 */
void accion_interrumpe(int32_t pasos) {
    pasos_objetivo = pasos;
    velocidad_final = 0;
}


/**
 * @brief Devuelve si la acción actual implica un cambio de casilla
 *
 */
bool accion_cambio_casilla() {
    /// @todo esto sólo parece válido sólo en exploración
    return (
            // Si vamos rectos y no vamos a frenar la acción implica un cambio de casilla
            (accion_radio == RADIO_INFINITO and velocidad_final > ACCION_V0)
           );

}
