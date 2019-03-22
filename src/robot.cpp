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

volatile int32_t pasos_recorridos = 0;

volatile bool sinc_pared = false;

typedef struct {
    volatile tipo_estado estado:3;
    volatile tipo_orientacion orientacion: 2;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
} tipo_robot;

volatile tipo_robot robot;

volatile uint8_t mejor_casilla; ///< Mejor casilla calculada desde la posición actual

void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;

} 

/**
 * @brief Incrementa la casilla actual en función de la orientacion
 *
 * Además, marca la casilla que se abandona como visitada, y pone en la nueva
 * el valor de las paredes en función de la información de sensores.
 */

void _incrementa_casilla() {

    laberinto_set_visitada(robot.casilla);

    switch (robot.orientacion) {
        case NORTE:
            robot.casilla += CASILLA_NORTE;
            break;
        case SUR:
            robot.casilla += CASILLA_SUR;
            break;
        case ESTE:
            robot.casilla += CASILLA_ESTE;
            break;
        case OESTE:
            robot.casilla += CASILLA_OESTE;
            break;
    }

    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(),
            leds_pared_derecha()
            );
    laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());
    sinc_pared = false;

    log_cambio_casilla();
}

void robot_siguiente_accion() {

    if (accion_cambio_casilla()) {
        _incrementa_casilla();

    }
    sinc_pared = false;
    robot.casilla_offset = 0;
    pasos_recorridos = 0;

    /// @todo el offset tampoco es lineal en los giros
    if (accion_get_radio() != 0) robot.casilla_offset += ( pasos_recorridos * LONGITUD_PASO_ENCODER);

    switch (robot.estado) {
        case PARADO:
            motores_parar();
            break;
        case EXPLORANDO_INICIAL:
            Serial.println(F("explorando inicial"));
            if (!flood_recalcula()) {
                accion_ejecuta(ARRANCAR);
                robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2;
                robot.estado = EXPLORANDO;
            }
            break;
        case EXPLORANDO:
            if (!leds_pared_derecha()) {
                 accion_ejecuta(PARAR);
                 robot.estado = PAUSA_PRE_GIRO;
            } else if (!leds_pared_enfrente()) {
                 accion_ejecuta(RECTO);
            } else if (!leds_pared_izquierda()) {
                 accion_ejecuta(PARAR);
                 robot.estado = PAUSA_PRE_GIRO;
            } else {
                 accion_ejecuta(PARAR);
                 robot.estado = PAUSA_PRE_GIRO;
            }
            break;
        case PAUSA_INICIAL:
            Serial.println(F("pausa inicial"));
            accion_ejecuta(ESPERA);
            laberinto_print();
            robot.estado = EXPLORANDO_INICIAL;
            break;

        case PAUSA_PRE_GIRO:
            Serial.println(F("pausa pregiro"));
            accion_ejecuta(ESPERA);
            laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());
            robot.estado = PARADO_PARA_GIRO;
            break;

        case PARADO_PARA_GIRO:
            if (!laberinto_hay_pared_derecha(robot.casilla)) {
                Serial.println(F("giro derecha"));
                accion_ejecuta(GIRO_DERECHA);
                robot.orientacion++;
                robot.estado = PAUSA_INICIAL;
            } else if (!laberinto_hay_pared_izquierda(robot.casilla)) {
                Serial.println(F("giro izquierda"));
                accion_ejecuta(GIRO_IZQUIERDA);
                robot.orientacion--;
                robot.estado = PAUSA_INICIAL;
            } else {
                Serial.println(F("giro 180"));
                accion_ejecuta(GIRO_180);
                robot.orientacion+=2;
                robot.estado = PAUSA_INICIAL;
            }
            break;

    }

    if (robot.casilla == CASILLA_INICIAL and robot.orientacion == ORIENTACION_INICIAL and robot.estado==ORIENTACION_INICIAL) {
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

void robot_inicia_exploracion() {

    robot.estado = PAUSA_INICIAL;
    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(), leds_pared_derecha());

    robot_siguiente_accion();
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
int16_t robot_get_desvio_centro() {
    int16_t desvio = 0;

    // Al principio de un movimiento en recta, se utilizan las paredes disponibles
    // solo si estamos al principio de la casilla
    if (robot.estado == EXPLORANDO) {
        if (laberinto_hay_pared_derecha(robot.casilla)) desvio += leds_get_desvio_derecho();
        if (laberinto_hay_pared_izquierda(robot.casilla)) desvio += leds_get_desvio_izquierdo();
    }

    return desvio;
}

void robot_control() {

    // control de posicion
    /** TODO: esto no se entiende
    if (accion_get_radio()!=GIRO_DERECHA_TODO and accion_get_radio()!=GIRO_IZQUIERDA_TODO and robot.casilla_offset + pasos_recorridos * LONGITUD_PASO_ENCODER > LABERINTO_LONGITUD_CASILLA) {
        Serial.println(F("Retrocedo offset"));
        robot.casilla_offset -= LABERINTO_LONGITUD_CASILLA;
    }
    */
    // log_pasos();
    

    // Sincronizacion con paredes
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

    if (!leds_pared_izquierda() and laberinto_hay_pared_izquierda(robot.casilla) and accion_get_radio() == RADIO_INFINITO and accion_get_distancia() == LABERINTO_LONGITUD_CASILLA and !sinc_pared) {
        Serial.print(F("sinc pared izquierda"));
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        accion_set_pasos_objetivo(pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER);
    }

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
            robot_siguiente_accion();
        }
    } else {

        pasos_recorridos = accion_get_radio() == GIRO_IZQUIERDA_TODO or accion_get_radio() == GIRO_DERECHA_TODO ? abs (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();

        if (pasos_recorridos >= accion_get_pasos_objetivo() or motores_get_velocidad_lineal_objetivo() == 0) {
            robot_siguiente_accion();
        }
        else if (pasos_recorridos >= accion_get_pasos_hasta_decelerar()) {
            motores_set_aceleracion_lineal(-accion_get_deceleracion());
        }
        else if (motores_get_velocidad_lineal_objetivo() >= accion_get_velocidad_maxima()) {
            motores_set_velocidad_lineal_objetivo(accion_get_velocidad_maxima());
        }
    }

}
