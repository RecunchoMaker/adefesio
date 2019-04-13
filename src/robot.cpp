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


typedef struct {
    volatile tipo_estado estado:3;
    volatile tipo_orientacion orientacion: 2;
    volatile bool hay_alguna_pared: 1;
    volatile bool giro_corregido: 1;
    volatile bool exploracion_completa: 1;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
    volatile int16_t ultima_diferencia_encoders = 0;
    volatile int16_t diferencia_pasos;
} tipo_robot;

volatile tipo_robot robot;

volatile bool mock_flag_siguiente_accion = false;

volatile uint8_t mejor_casilla; ///< Mejor casilla calculada desde la posición actual

volatile float desvio; ///< auxilar;


void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    leds_reset_distancias_minimas();
    motores_parar();

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    robot.estado = PARADO;
    robot.exploracion_completa = false;
} 


/**
 * @brief Comienza la exploracion o la solucion
 */
void robot_empieza() {
    if (robot.exploracion_completa) {
        Serial.println(F("EMPIEZA-RESUELVE"));
        robot_resuelve();
    }
    else {
        Serial.println(F("EMPIEZA-EXPLORA"));
        robot_explora();
    }
}


void robot_explora() {

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(), leds_pared_derecha());

    cli(); // evita el tratamiento solapado en robot_control
    robot.estado = FLOOD;
    robot_siguiente_accion();
    sei();
}

void robot_resuelve() {

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    
    camino_recalcula();
    Serial.print(F("SOLUCION: "));
    log_camino();
    robot.estado = FLOOD;
    cli(); // evita el tratamiento solapado en robot_control
    robot_siguiente_accion();
    sei();
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

    if (!laberinto_get_visitada(robot.casilla)) {
        laberinto_set_paredes_laterales(robot.casilla, 
                leds_pared_izquierda(),
                leds_pared_derecha()
                );
    }

    laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());

    laberinto_print();

    log_cambio_casilla();
}


void robot_siguiente_accion() {
    static int8_t paso;

    /*
    if (robot.estado == AVANZANDO) {
        _incrementa_casilla();
    }
    */

    leds_reset_distancias_minimas();
    robot.giro_corregido = false;

    if (robot.estado == AVANZANDO)
        _incrementa_casilla();
    paso = laberinto_get_paso(robot.casilla); // despues de incrementar casilla


    if (robot.estado == PARADO) {
        motores_parar();
    } else if (robot.estado == EMPIEZA) {
        Serial.print(F("E-EMPIEZA\n"));
        flood_init(robot.casilla == CASILLA_INICIAL ? CASILLA_SOLUCION: CASILLA_INICIAL);
        robot.estado = FLOOD;

        /*
        if (robot.casilla == CASILLA_INICIAL 
                and camino_get_ultima_casilla() == CASILLA_SOLUCION 
                and camino_get_todas_visitadas()) {
            robot.estado = ESPERANDO_SOLUCION;
        } else {


        if (robot.exploracion_completada)
        robot.estado = FLOOD;

        Serial.print(F("E-FIN\n"));
        robot.estado = PARADO;
        */
        accion_ejecuta(ESPERA);
    } else if (robot.estado == FLOOD) {
        Serial.print(F("E-FLOOD\n"));
        //while (flood_recalcula());
        if (!flood_recalcula()) {
            camino_recalcula();
            laberinto_print();
            log_camino();
            robot.estado = REORIENTA;
        } 
#ifdef MOCK
        else
            Serial.println("nextAction");
#endif
    } else if (robot.estado == REORIENTA) {
        Serial.print(F("E-REORIENTA..."));
        if (robot.orientacion == camino_get_orientacion_origen()) {
            Serial.print(F("OK\n"));
            robot.estado = ESPERANDO;
        } else {
            Serial.println();
            accion_ejecuta(GIRA_180);
            robot.orientacion--;
            robot.orientacion--;
            // continuo en el mismo estado
        }
    } else if (robot.estado == ESPERANDO) {
        accion_ejecuta(ESPERA);
        robot.estado = DECIDE;
    } else if (robot.estado == DECIDE) {
        if (robot.casilla == CASILLA_INICIAL 
                and camino_get_ultima_casilla() == CASILLA_SOLUCION 
                and camino_get_todas_visitadas()) {
            robot.estado = ESPERANDO_SENAL;
        } else {
            Serial.print(F("E-DECIDE\n"));
            robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2.0;
            switch (paso) {
                case PASO_RECTO:
                                 Serial.print(F("ARRANCA\n"));
                                 robot.estado = AVANZANDO;
                                 robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2.0;
                                 accion_ejecuta(ARRANCA);
                                 robot.casilla_offset = 0; // solo en el arranque
                                 break;
                case PASO_DER:   
                                 Serial.print(F("GIRA_DER\n"));
                                 if (!leds_pared_derecha()) {
                                     accion_ejecuta(GIRA_DER);
                                     robot.estado = ESPERANDO;
                                     laberinto_set_paso(robot.casilla, PASO_RECTO);
                                     robot.orientacion++;
                                 } else {
                                     robot.estado = FLOOD;
                                 }
                                 break;
                case PASO_IZQ:   
                                 Serial.print(F("GIRA_IZQ\n"));
                                     if (!leds_pared_izquierda()) {
                                     accion_ejecuta(GIRA_IZQ);
                                     robot.estado = ESPERANDO;
                                     laberinto_set_paso(robot.casilla, PASO_RECTO);
                                     robot.orientacion--;
                                 } else {
                                     robot.estado = FLOOD;
                                 }
                                 break;
                case PASO_STOP:  
                                 Serial.print(F("GIRA_180\n"));
                                 accion_ejecuta(GIRA_180);
                                 robot.estado = FLOOD;
                                 robot.orientacion--;
                                 robot.orientacion--;
                                 break;
            }
        }
    } else if (robot.estado == AVANZANDO) {
        Serial.print(F("E-AVANZANDO\n"));

        if (paso == PASO_RECTO and !leds_pared_enfrente()) {
            accion_ejecuta(AVANZA);
            Serial.print(F("AVANZA\n"));
            robot.estado = AVANZANDO;
        } else {
            accion_ejecuta(PARA);
            Serial.print(F("PARA\n"));
            robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2.0;

            log_leds();

            if (robot.casilla == CASILLA_SOLUCION or robot.casilla == CASILLA_INICIAL) {
                robot.estado = EMPIEZA;
            } else {
                robot.estado = FLOOD;
            }
        }
    }
}


int32_t robot_get_pasos_recorridos() {
    return pasos_recorridos;
}

int16_t robot_get_ultima_diferencia_encoders() {
    return robot.ultima_diferencia_encoders;
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

void robot_set_estado(tipo_estado estado) {;
    robot.estado = estado;
}

float robot_get_casilla_offset() {
    return robot.casilla_offset;
}

bool robot_es_valido_led_izquierdo() { // solo valido en rectas
    uint8_t esta_pared = laberinto_hay_pared_izquierda(robot.casilla);
    uint8_t siguiente_pared = laberinto_hay_pared_izquierda(robot.casilla + incremento[robot.orientacion]);

    if (esta_pared) {
        if (siguiente_pared) {
            return true; // siempre hay paredes
        } else {
            return leds_get_distancia(LED_IZQ)<0.03 or pasos_recorridos + robot.casilla_offset / LONGITUD_PASO_ENCODER < 200;
        }
    } else {
        if (siguiente_pared) {
            return pasos_recorridos > 400;
        }
    }
    return false;
}


bool robot_es_valido_led_derecho() { // solo valido en rectas
    uint8_t esta_pared = laberinto_hay_pared_derecha(robot.casilla);
    uint8_t siguiente_pared = laberinto_hay_pared_derecha(robot.casilla + incremento[robot.orientacion]);

    if (esta_pared) {
        if (siguiente_pared) {
            return true; // siempre hay paredes
        } else {
            return leds_get_distancia(LED_DER)<0.03 or pasos_recorridos + robot.casilla_offset / LONGITUD_PASO_ENCODER < 200;
        }
    } else {
        if (siguiente_pared) {
            return pasos_recorridos > 400;
        }
    }
    return false;
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
float robot_get_angulo_desvio() {

    // Al principio de un movimiento en recta, se utilizan las paredes disponibles
    // solo si estamos al principio de la casilla
    // if (robot.estado != 99) {
    desvio = 0.0;
    if (accion_get_accion_actual() == ARRANCA or accion_get_accion_actual() == AVANZA or accion_get_accion_actual() == PARA) {
        //if (leds_get_distancia_kalman(LED_IZQ) < 0.08) {
        if (robot_es_valido_led_izquierdo()) {
            desvio += motores_get_kp_pasillo1() * (-leds_get_distancia_kalman(LED_IZQ) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0));
            desvio -= motores_get_kp_pasillo2() * (leds_get_distancia_d(LED_IZQ) * motores_get_velocidad_lineal_objetivo());
        }
        if (robot_es_valido_led_derecho()) {
        //if (leds_get_distancia_kalman(LED_DER) < 0.08) {
            desvio -= motores_get_kp_pasillo1() * (-leds_get_distancia_kalman(LED_DER) - (ANCHURA_ROBOT / 2.0) + (LABERINTO_LONGITUD_CASILLA/2.0));
            desvio += motores_get_kp_pasillo2() * (leds_get_distancia_d(LED_DER) * motores_get_velocidad_lineal_objetivo());
        }

    }
    return desvio;
}

void robot_control() {

#ifdef MOCK

//            if (Serial.available()) {
//                char c = Serial.read();
//                if (c == '.')
//                    // Serial.println(F("*accion"));
//                    robot_siguiente_accion();
//            }
//            return;
//
    if (robot.estado == ESPERANDO or robot.estado == REORIENTA)
        robot_siguiente_accion();
    else if (mock_flag_siguiente_accion) {
        mock_flag_siguiente_accion = false;
        robot_siguiente_accion();
    }
    return;

#endif
    if (accion_get_accion_actual() == AVANZA) {
        if (robot_get_angulo_desvio() != 0)
            motores_set_radio(1.0 / robot_get_angulo_desvio());
        else
            motores_set_radio(8888);
    }

    if ((leds_get_distancia(LED_FDER) + leds_get_distancia(LED_FIZQ)) / 2.0 < 0.01 and accion_get_radio() == RADIO_INFINITO) {
        Serial.print(F("chocamos! leds:"));
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

        if (accion_get_accion_actual() == PARA and leds_pared_enfrente()) {
            motores_set_velocidad_lineal_objetivo( 
                    min(
                    ACCION_V0 + ((leds_get_distancia(LED_FDER)+leds_get_distancia(LED_FIZQ)/2.0) - 0.028) * (ACCION_VE/(ACCION_VE-ACCION_V0)),
                    ACCION_VE));
            accion_set_pasos_objetivo(pasos_recorridos + (( (leds_get_distancia(LED_FIZQ) + leds_get_distancia(LED_FDER)) / 2.0)-0.028) / LONGITUD_PASO_ENCODER);
        }

        if (pasos_recorridos >= accion_get_pasos_objetivo() or motores_get_velocidad_lineal_objetivo() == 0) {
#ifdef MOCK
            if (!Serial.available()) {
                Serial.print(F("Esperooooo"));
                return;
            }

#endif
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

void mock_siguiente_accion() {
    mock_flag_siguiente_accion = true;
}
