/**
 * @file robot.cpp
 *
 * @brief Control central del movimiento del robot
 *
 * @todo Mucho sin documentar
 */
#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <leds.h>
#include <timer1.h>
#include <laberinto.h>
#include <accion.h>
#include <log.h>
#include <flood.h>

/// Acumula los pasos recorridos de encoder desde el último reset
volatile int32_t pasos_recorridos = 0;

//@{
/// Variables utilizadas para la sincronizacion en giros
volatile int16_t robot_led_izq = 0;
volatile int16_t robot_led_der = 0;
//@}


typedef struct {
    volatile tipo_estado estado:3;
    volatile tipo_orientacion orientacion: 2;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
} tipo_robot;

volatile tipo_robot robot;

volatile uint8_t mejor_casilla; ///< Mejor casilla calculada desde la posición actual

volatile float desvio; ///< auxilar;


void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    robot.estado = PARADO;

} 


void robot_inicia_exploracion() {

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(), leds_pared_derecha());
    laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());

    laberinto_print();
    Serial.println("sa-0");

    robot.estado = DECIDE;
    robot_siguiente_accion();
}


/**
 * @brief Incrementa la casilla actual en función de la orientacion
 *
 * Además, marca la casilla que se abandona como visitada, y pone en la nueva
 * el valor de las paredes en función de la información de sensores.
 */

void _incrementa_casilla() {

    laberinto_set_visitada(robot.casilla);

    robot.casilla += incremento[robot.orientacion];

    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(),
            leds_pared_derecha()
            );
    laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());

    laberinto_print();

    log_cambio_casilla();
}


void robot_siguiente_accion() {
    /// @todo control del incremento de casilla... double check esto
    /*
    if (accion_cambio_casilla()) {
        _incrementa_casilla();
    }
    */
    if (robot.estado == AVANZANDO) {
        _incrementa_casilla();
    }

    int8_t paso = laberinto_get_paso(robot.casilla);

    if (robot.estado == PARADO) {
        motores_parar();
    } else if (robot.estado == ESPERANDO) {
        Serial.println(F("E-ESPERANDO"));
        accion_ejecuta(ESPERA);
        robot.estado = DECIDE;
    } else if (robot.estado == DECIDE) {
        Serial.println(F("E-DECIDE"));
        switch (paso) {
            case PASO_RECTO: accion_ejecuta(ARRANCA);
                             Serial.println(F("ARRANCA"));
                             robot.estado = AVANZANDO;
                             robot.casilla_offset = LABERINTO_LONGITUD_CASILLA * 0.5 / LONGITUD_PASO_ENCODER;
                             break;
            case PASO_DER:   accion_ejecuta(GIRA_DER);
                             Serial.println(F("GIRA_DER"));
                             robot.estado = ESPERANDO;
                             robot.orientacion++;
                             laberinto_set_paso(robot.casilla, PASO_RECTO);
                             break;
            case PASO_IZQ:   accion_ejecuta(GIRA_IZQ);
                             Serial.println(F("GIRA_IZQ"));
                             robot.estado = ESPERANDO;
                             robot.orientacion--;
                             laberinto_set_paso(robot.casilla, PASO_RECTO);
                             break;
            case PASO_STOP:  accion_ejecuta(GIRA_180);
                             Serial.println(F("GIRA_180"));
                             robot.estado = FIN;
                             robot.orientacion--;
                             robot.orientacion--;
                             robot_led_izq = leds_get_valor(LED_IZQ);
                             robot_led_der = leds_get_valor(LED_DER);
                             break;
        }
    } else if (robot.estado == AVANZANDO) {
        robot.casilla_offset = 0;
        Serial.println(F("E-AVANZANDO"));
        if (paso == PASO_RECTO) {
            accion_ejecuta(AVANZA);
            Serial.println(F("AVANZA"));
            robot.estado = AVANZANDO;
        } else {
            accion_ejecuta(PARA);
            Serial.println(F("PARA"));
            robot.estado = ESPERANDO;
        }
    } else if (robot.estado == FIN) {
        Serial.println(F("E-FIN"));
        accion_ejecuta(ESPERA);
        robot.estado = PARADO;
    }

}


int32_t robot_get_pasos_recorridos() {
    return pasos_recorridos;
}

uint8_t robot_get_casilla() {
    return robot.casilla;
}

tipo_orientacion robot_get_orientacion() {
    return robot.orientacion;
}

tipo_estado robot_get_estado() {
    return robot.estado;
}

float robot_get_casilla_offset() {
    return robot.casilla_offset;
}


/**
 * @brief Devuelve una estimacion del desvio al centro
 *
 * Usa los leds laterales, el progreso de la accion actual y las paredes
 * del laberinto para estimar una desviación al centro.
 *
 * Lo usa motores_actualiza_velocidad() para corregir la desviación
 *
 */
float robot_get_desvio_centro() {

    // Al principio de un movimiento en recta, se utilizan las paredes disponibles
    // solo si estamos al principio de la casilla
    if (robot.estado != 99) {
        if (pasos_recorridos + robot.casilla_offset > 999)  {
           desvio = 0;
        } else { 
            // alfa final
            desvio = 0;
            if (laberinto_hay_pared_izquierda(robot.casilla)) {
                desvio += ((LABERINTO_LONGITUD_CASILLA/2.0) - leds_get_distancia_kalman(LED_IZQ) - LEDS_DISTANCIA) / 0.2;
                desvio -= leds_get_distancia_d(LED_IZQ) / (PERIODO_CICLO * motores_get_velocidad_lineal_objetivo());
            }
            /*
            if (laberinto_hay_pared_derecha(robot.casilla)) {
                desvio += 0.2 / ((LABERINTO_LONGITUD_CASILLA/2.0) - leds_get_distancia_kalman(LED_DER) - LEDS_DISTANCIA);
                desvio -= leds_get_distancia_d(LED_DER) / (PERIODO_CICLO * motores_get_velocidad_lineal_objetivo());
            }
            */

            //float tan_angulo_der = laberinto_hay_pared_derecha(robot.casilla) ? 
            //    leds_get_distancia_d(LED_DER) / (motores_get_velocidad_lineal_objetivo() * PERIODO_CICLO)
            //    : 0;
            //Serial.println(leds_get_distancia_d(LED_IZQ),8);
            //Serial.println(tan_angulo_izq, 6);

            //desvio = tan_angulo_der - tan_angulo_izq;
        }
    }

    return 0.1 * desvio;
}

void robot_control() {

    if (robot.estado == PARADO) return;
    // control de posicion
    /** TODO: esto no se entiende
    if (accion_get_radio()!=GIRO_DERECHA_TODO and accion_get_radio()!=GIRO_IZQUIERDA_TODO and robot.casilla_offset + pasos_recorridos * LONGITUD_PASO_ENCODER > LABERINTO_LONGITUD_CASILLA) {
        Serial.println(F("Retrocedo offset"));
        robot.casilla_offset -= LABERINTO_LONGITUD_CASILLA;
    }
    */
    // log_pasos();
    

    // Sincronizacion con paredes
    /*
    if (!leds_pared_derecha() and laberinto_hay_pared_derecha(robot.casilla) and accion_get_radio() == RADIO_INFINITO and accion_get_distancia() == LABERINTO_LONGITUD_CASILLA and !sinc_pared) {
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        if (pasos_recorridos > DOS_TERCIOS_CASILLA) {
            Serial.print(F("sinc pared derecha"));
            accion_set_pasos_objetivo(pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER);
        } else if (pasos_recorridos < DOS_TERCIOS_CASILLA) {
            /// TODO: hacer esto mejor
            log_leds();
            laberinto_print();
            
            Serial.print(F("no detecte a tiempo la pared"));
        }
            
    }
    */

    // Prueba sincronizacion 180
    /*
    if (robot.estado == FIN and pasos_recorridos > 255) {
        int16_t diff = 0;
        if (leds_pared_derecha()) {
            diff = robot_led_der - leds_get_valor(LED_DER);
        } else if (leds_pared_izquierda()) {
            diff = leds_get_valor(LED_IZQ) - robot_led_izq ;
        }

        Serial.print("diff = ");
        Serial.print(diff);
        accion_set_pasos_objetivo(279 - diff/10);
    }
    */

    /*
    if (!leds_pared_izquierda() and laberinto_hay_pared_izquierda(robot.casilla) and accion_get_radio() == RADIO_INFINITO and accion_get_distancia() == LABERINTO_LONGITUD_CASILLA and !sinc_pared) {
        Serial.print(F("sinc pared izquierda"));
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        accion_set_pasos_objetivo(pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER);
    }
    */

    if ((leds_get_valor(LED_FDER) + leds_get_valor(LED_FIZQ) > 1000) and accion_get_radio() == RADIO_INFINITO) {
        Serial.println(F("chocamos! leds:"));
        log_leds();
        log_pasos();
        accion_interrumpe(pasos_recorridos);
        if (pasos_recorridos < 100) {
            ///@todo parada de emergencia si pongo las manos delante
            robot.estado = PARADO;
        }
    }


    if (accion_get_velocidad_maxima() <= 0.0) {
        motores_parar();
        if (timer1_get_cuenta() * PERIODO_TIMER > accion_get_radio()) {
            //Serial.println("sa-1");
            robot_siguiente_accion();
        }
    } else {

        pasos_recorridos = accion_get_radio() == GIRO_IZQUIERDA_TODO or accion_get_radio() == GIRO_DERECHA_TODO ? abs (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();

        if (pasos_recorridos >= accion_get_pasos_objetivo() or motores_get_velocidad_lineal_objetivo() == 0) {
            Serial.println("sa-2");
            robot_siguiente_accion();
        }
        else if (pasos_recorridos >= accion_get_pasos_hasta_decelerar()) {
            motores_set_aceleracion_lineal(-accion_get_deceleracion());
        }
        else if (motores_get_velocidad_lineal_objetivo() >= accion_get_velocidad_maxima()) {
            motores_set_velocidad_lineal_objetivo(accion_get_velocidad_maxima());
        }


    }

    ///@todo
    /*
    if (leds_get_distancia(LED_IZQ) > 0.1 or leds_get_distancia(LED_DER) > 0.1) {
        Serial.println("emergencia!");
        log_leds();
        Serial.println(leds_get_distancia(LED_IZQ));
        Serial.println(leds_get_distancia(LED_DER));
        robot.estado = PARADO;
        motores_parar();
    }
    */
    

}
