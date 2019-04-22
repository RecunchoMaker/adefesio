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
#include <leds.h>
#include <flood.h>
#include <control.h>

/// Acumula los pasos recorridos de encoder desde el último reset
volatile int32_t pasos_recorridos = 0;


typedef struct {
    volatile tipo_estado estado:4;
    volatile tipo_orientacion orientacion: 2;
    volatile bool hay_alguna_pared: 1;
    volatile bool giro_corregido: 1;
    volatile bool exploracion_completa: 1;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
    volatile int16_t ultima_diferencia_encoders = 0;
    volatile int16_t diferencia_pasos;
} tipo_robot;

volatile void (*control_funcion)(void)=NULL;

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
        accion_ejecuta(ESPERA);
        control_funcion = control_espera;

    } else if (robot.estado == FLOOD) {
        control_funcion = NULL;
        Serial.print(F("E-FLOOD\n"));
        if (!flood_recalcula()) {
            camino_recalcula();
            laberinto_print();
            log_camino();
            robot.estado = REORIENTA;
        } 
        control_funcion = control_continua;
#ifdef MOCK
        else
            Serial.println("nextAction");
#endif
    } else if (robot.estado == REORIENTA) {
        Serial.print(F("E-REORIENTA..."));
        if (robot.orientacion == camino_get_orientacion_origen()) {
            Serial.print(F("OK\n"));
            accion_ejecuta(ESPERA);
            control_funcion = control_espera;
            robot.estado = DECIDE;
        } else {
            Serial.println();
            accion_ejecuta(GIRA_180);
            robot.orientacion--;
            robot.orientacion--;
            control_funcion = NULL;
            // continuo en el mismo estado
        }
    } else if (robot.estado == ESPERANDO) { //@todo quitar este estado
        accion_ejecuta(ESPERA);
        control_funcion = control_espera;
        robot.estado = DECIDE;
    } else if (robot.estado == DECIDE) {
        control_funcion = NULL;
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
            robot.casilla_offset = 0;
            accion_ejecuta(AVANZA);
            Serial.print(F("AVANZA\n"));
            robot.estado = AVANZANDO;
            control_funcion = control_avance;
        } else {
            accion_ejecuta(PARA);
            Serial.print(F("PARA\n"));
            robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2.0;
            control_funcion = control_parada;

            log_leds();

            if (robot.casilla == CASILLA_SOLUCION or robot.casilla == CASILLA_INICIAL) {
                robot.estado = EMPIEZA;
            } else {
                robot.estado = EMPIEZA; //@todo
            }
        }
    } else if (robot.estado == CALIBRANDO) {
        Serial.print(F("E-CALIBRANDO"));
        accion_ejecuta(CALIBRA_ATRAS);
        robot.estado = CALIBRANDO_ATRAS;
    } else if (robot.estado == CALIBRANDO_ATRAS) {
        Serial.println("ejecuto camibra alante");
        accion_ejecuta(CALIBRA_ADELANTE);
        robot.estado = CALIBRANDO_ADELANTE;
    } else if (robot.estado == CALIBRANDO_ADELANTE) {
        Serial.println("y paro");
        accion_ejecuta(ESPERA);
        robot.estado = PARADO;
    }


}


int32_t robot_get_pasos_recorridos() {
    pasos_recorridos = accion_get_radio() == GIRO_IZQUIERDA_TODO or accion_get_radio() == GIRO_DERECHA_TODO ? abs (encoders_get_posicion_total_right() 
        - encoders_get_posicion_total_left()) / 2
        : encoders_get_posicion_total();
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
            return encoders_get_posicion_total() + robot.casilla_offset / LONGITUD_PASO_ENCODER < 250;
        }
    } else {
        if (siguiente_pared) {
            return encoders_get_posicion_total() > 400;
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
            return encoders_get_posicion_total() + robot.casilla_offset / LONGITUD_PASO_ENCODER < 250;
        }
    } else {
        if (siguiente_pared) {
            return encoders_get_posicion_total() > 400;
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
    if (accion_get_accion_actual() == ARRANCA or accion_get_accion_actual() == AVANZA) {
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

    } else if (accion_get_accion_actual() == PARA and leds_pared_enfrente()) {
        desvio = 1300.0 * (leds_get_distancia(LED_FDER) - leds_get_distancia(LED_FIZQ));
    }
    return desvio;
}

void robot_control() {

#ifdef MOCK
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

    /*
    if (motores_get_velocidad_lineal_objetivo() > 0 and (leds_get_distancia(LED_FDER) + leds_get_distancia(LED_FIZQ)) / 2.0 < 0.01 and accion_get_radio() == RADIO_INFINITO) {
        Serial.print(F("chocamos! leds:"));
        log_leds();
        log_pasos();
        accion_interrumpe(pasos_recorridos);
        if (pasos_recorridos < 100) {
            ///@todo parada de emergencia si pongo las manos delante
            robot.estado = PARADO;
        }
    }
    */


    ///@todo
    //if (accion_get_velocidad_maxima() <= -9990.0) {
    
    if (control_funcion != NULL) {
        control_funcion();
    } else {

        pasos_recorridos = accion_get_radio() == GIRO_IZQUIERDA_TODO or accion_get_radio() == GIRO_DERECHA_TODO ? abs (encoders_get_posicion_total_right() 
            - encoders_get_posicion_total_left()) / 2
            : encoders_get_posicion_total();

        if ( (pasos_recorridos >= accion_get_pasos_objetivo() and motores_get_velocidad_lineal_objetivo() > 0)
             or ((pasos_recorridos <= accion_get_pasos_objetivo() and motores_get_velocidad_lineal_objetivo() < 0))
             or motores_get_velocidad_lineal_objetivo() == 0) {

            robot_siguiente_accion();
        } else if (
                (motores_get_velocidad_lineal_objetivo() > 0 and pasos_recorridos >= accion_get_pasos_hasta_decelerar())
                or 
                (motores_get_velocidad_lineal_objetivo() < 0 and pasos_recorridos <= accion_get_pasos_hasta_decelerar())
                )
                {
            motores_set_aceleracion_lineal(-accion_get_deceleracion());
        } else if (accion_get_accion_actual() == PARA and leds_pared_enfrente()) {
            accion_set_pasos_objetivo((leds_get_distancia(LED_FDER) - 0.02) / LONGITUD_PASO_ENCODER);
        }
        else if ((motores_get_velocidad_lineal_objetivo() > 0 and motores_get_velocidad_lineal_objetivo() >= accion_get_velocidad_maxima())
                or
                (motores_get_velocidad_lineal_objetivo() < 0 and motores_get_velocidad_lineal_objetivo() <= accion_get_velocidad_maxima())
                )
        {
            motores_set_velocidad_lineal_objetivo(accion_get_velocidad_maxima());
        }


    }


}

void mock_siguiente_accion() {
    mock_flag_siguiente_accion = true;
}


void robot_calibracion_frontal() {

    //if (leds_pared_enfrente() and leds_pared_derecha() and leds_pared_izquierda()) {
    delay(1000);
    leds_calibracion_lateral();
    delay(1000);

    if (leds_get_distancia(LED_FIZQ) < 0.02) {

        bool fin_calibracion = false;
        Serial.println(F("Comienzo calibracion"));

        while (!fin_calibracion) {
            int8_t indice_i = 15;
            int16_t array_i[16];
            int8_t indice_d = 15;
            int16_t array_d[16];

            encoders_reset_posicion_aux_total();

            while((leds_get_valor(LED_FIZQ) >> 6) < indice_i)
                array_i[indice_i--] = 0;
            while((leds_get_valor(LED_FDER) >> 6) < indice_d)
                array_d[indice_d--] = 0;

            robot.estado = CALIBRANDO;

            while (encoders_get_posicion_aux() > -300 and (indice_i >= 0 or indice_d >=0 )) {
                /*
                Serial.print(leds_get_valor(LED_FDER));
                Serial.print(";");
                Serial.print(leds_get_valor(LED_FDER)>>6, BIN);
                Serial.print(";");
                Serial.println(encoders_get_posicion_aux());
                Serial.print(encoders_get_posicion_aux());
                Serial.print(",");
                Serial.print(leds_get_valor(LED_FDER));
                Serial.print(":0x");
                Serial.print(leds_get_valor(LED_FDER),BIN);
                Serial.print(" <<");
                Serial.println(leds_get_valor(LED_FDER) >> 6);
                */
                    
                if (leds_get_valor(LED_FIZQ) >> 6 <= indice_i and indice_i>=0) {
                    /*
                    Serial.print("(mov) array _i");
                    Serial.print(indice_i);
                    Serial.print(" = ");
                    Serial.println(encoders_get_posicion_aux());
                    */
                    array_i[indice_i--] = max(encoders_get_posicion_aux(),-255);
                }
                if (leds_get_valor(LED_FDER) >> 6 <= indice_d and indice_d>=0) {
                    Serial.print("(mov) array _d ");
                    Serial.print(indice_d);
                    Serial.print(" = ");
                    Serial.println(encoders_get_posicion_aux());
                    array_d[indice_d--] = max(encoders_get_posicion_aux(),-255);
                }
            }

            /*
            for (int i=0; i<=15; i++) {

                Serial.print("array ");
                Serial.print(i);
                Serial.print(" = ");
                Serial.print(array_i[i]);
                Serial.print(", ");
                Serial.print(array_d[i]);
                Serial.print(": ");
                Serial.print((int) (1000.0 * -array_i[i] * LONGITUD_PASO_ENCODER));
                Serial.print(": ");
                Serial.print(led_segmentos[LED_FIZQ-A0][i]);
                Serial.print(", ");
                Serial.print(led_segmentos[LED_FDER-A0][i]);

                Serial.println();

            }
            */
            Serial.print("indice i ");
            Serial.println(indice_i);
            Serial.print("indice d ");
            Serial.println(indice_d);

            while (indice_i >= 0)
                array_i[indice_i--] = -300;
            while (indice_d >= 0)
                array_d[indice_d--] = -300;


            leds_set_segmento(LED_FIZQ, array_i);
            leds_set_segmento(LED_FDER, array_d);

            leds_set_segmento(LED_DER, array_i);
            leds_set_segmento(LED_IZQ, array_d);
                
            Serial.println(F("Fin calibra frontal "));
            accion_set_pasos_objetivo(pasos_recorridos);
            while (leds_get_distancia(LED_FDER) > 0.02);
           
            accion_set_pasos_objetivo(pasos_recorridos);
            Serial.println(F("Fin calibra frontal 2"));

            leds_graba_segmentos_a_eeprom();
            fin_calibracion = true;



        }
    } else {
        leds_lee_segmentos_de_eeprom();
    }
}
