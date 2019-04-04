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
    volatile bool resolviendo: 1;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
    volatile float ultima_distancia_lateral = LABERINTO_LONGITUD_CASILLA / 2.0;
    volatile int16_t ultima_diferencia_encoders = 0;
    volatile int16_t diferencia_pasos;
} tipo_robot;

volatile tipo_robot robot;

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

} 


void robot_inicia_exploracion() {

    robot.resolviendo = false;
    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(), leds_pared_derecha());
    laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());

    laberinto_print();
    cli(); // evita el tratamiento solapado en robot_control
    robot.estado = FLOOD;

    robot_siguiente_accion();
    sei();
}

void robot_resuelve() {

    robot.resolviendo = true;
    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    
    camino_recalcula();
    laberinto_print();
    Serial.print(F("SOLUCION: "));
    log_camino();
    robot.estado = DECIDE;
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

    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(),
            leds_pared_derecha()
            );

    // robot.hay_alguna_pared indica si la casilla actual no tiene paredes y la anterior
    // tampoco tenía
    if (!robot.hay_alguna_pared and !leds_pared_izquierda() and !leds_pared_enfrente()) {
        robot.hay_alguna_pared = false;
        Serial.println("No hay paredes");
    }
    else
        robot.hay_alguna_pared = true;
    laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());

    laberinto_print();

    log_cambio_casilla();
}


void robot_siguiente_accion() {
    static int8_t paso;
    /// @todo control del incremento de casilla... double check esto
    /*
    if (accion_cambio_casilla()) {
        _incrementa_casilla();
    }
    */
    if (robot.estado == AVANZANDO) {
        _incrementa_casilla();
    }
    robot.diferencia_pasos = leds_get_diferencia_pasos_der_izq() - LED_DESFASE_LATERAL;
    leds_reset_distancias_minimas();
    robot.giro_corregido = false;

    paso = laberinto_get_paso(robot.casilla);

    if (robot.estado == PARADO) {
        motores_parar();
    } else if (robot.estado == FLOOD) {
        Serial.print("encoders aux: ");
        Serial.print(encoders_get_posicion_aux_left());
        Serial.print(":");
        Serial.println(encoders_get_posicion_aux_right());
        robot.ultima_diferencia_encoders = encoders_get_posicion_aux_left() - encoders_get_posicion_aux_right();
        Serial.println(F("E-FLOOD"));
        if (!flood_recalcula()) {
            camino_recalcula();
            laberinto_print();
            log_camino();
            robot.estado = REORIENTA;
        } 
    } else if (robot.estado == ESPERANDO) {

        Serial.println(F("E-ESPERANDO"));
        //robot.ultima_distancia_lateral = (leds_pared_derecha()? leds_get_distancia(LED_DER):leds_get_distancia(LED_IZQ));
        Serial.print("Ultima distancia lateral: ");
        Serial.println(robot.ultima_distancia_lateral, 9);
        Serial.print("  ");
        Serial.print(leds_get_distancia(LED_IZQ));
        Serial.print("-");
        Serial.print(leds_get_distancia(LED_DER));
        accion_ejecuta(ESPERA);

        
        robot.estado = DECIDE;

    } else if (robot.estado == REORIENTA) {
        Serial.println(F("E-REORIENTA"));
        if (robot.orientacion == camino_get_orientacion_origen()) {
            robot.estado = ESPERANDO;
        } else {
            accion_ejecuta(GIRA_180);
            robot.orientacion--;
            robot.orientacion--;

        }

    } else if (robot.estado == DECIDE) {

        if (robot.casilla == CASILLA_INICIAL) {
            Serial.println("CASILLA INICIAL");
            Serial.println(camino_get_ultima_casilla());
            Serial.print(robot.resolviendo);
            Serial.println(camino_get_todas_visitadas());
        }
        
        if (robot.casilla == CASILLA_INICIAL 
                and camino_get_ultima_casilla() == CASILLA_SOLUCION 
                and !robot.resolviendo
                and camino_get_todas_visitadas()) {
            robot.estado = ESPERANDO_RESOLUCION;
        }

        Serial.println(F("E-DECIDE"));
        Serial.print("  casilla ");
        Serial.print(robot.casilla);
        Serial.print("  paso = ");
        Serial.println(paso);
        laberinto_print(); 
        switch (paso) {
            case PASO_RECTO:
                             Serial.println(F("ARRANCA"));
                             robot.estado = AVANZANDO;
                             robot.casilla_offset = robot.ultima_distancia_lateral;
                             accion_ejecuta(ARRANCA);
                             Serial.print("offset nuevo = ");
                             Serial.println(robot.casilla_offset,5);
                             break;
            case PASO_DER:   
                             if (leds_pared_izquierda())
                                 robot.ultima_distancia_lateral = leds_get_distancia(LED_IZQ) + (ANCHURA_ROBOT/2.0);
                             else
                                 robot.ultima_distancia_lateral = LABERINTO_LONGITUD_CASILLA * 0.5;
                             Serial.println(F("GIRA_DER"));
                             accion_ejecuta(GIRA_DER);
                             robot.estado = ESPERANDO;
                             robot.orientacion++;
                             laberinto_set_paso(robot.casilla, PASO_RECTO);
                             break;
            case PASO_IZQ:   
                             if (leds_pared_derecha())
                                 robot.ultima_distancia_lateral = leds_get_distancia(LED_DER) + (ANCHURA_ROBOT/2.0);
                             else
                                 robot.ultima_distancia_lateral = LABERINTO_LONGITUD_CASILLA * 0.5;
                             Serial.println(F("GIRA_IZQ"));
                             accion_ejecuta(GIRA_IZQ);
                             robot.estado = ESPERANDO;
                             robot.orientacion--;
                             laberinto_set_paso(robot.casilla, PASO_RECTO);
                             break;
            case PASO_STOP:  
                             Serial.println(F("GIRA_180"));
                             accion_ejecuta(GIRA_180);
                             robot.estado = FLOOD;
                             //robot.estado = PARADO;
                             robot.orientacion--;
                             robot.orientacion--;
                             break;
        }
    } else if (robot.estado == AVANZANDO) {
        robot.casilla_offset = 0;
        Serial.println(F("E-AVANZANDO"));
        if (paso == PASO_RECTO and !leds_pared_enfrente()) {
            accion_ejecuta(AVANZA);
            Serial.println(F("AVANZA"));
            robot.estado = AVANZANDO;
        } else {
            accion_ejecuta(PARA);
            Serial.println(F("PARA"));

            // robot.estado = DECIDE;
            if (robot.casilla == CASILLA_SOLUCION)
                flood_init(CASILLA_INICIAL);
            if (robot.casilla == CASILLA_INICIAL)
                flood_init(CASILLA_SOLUCION);

            robot.estado = robot.resolviendo ? REORIENTA : FLOOD;
            laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());
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
        if (!robot.hay_alguna_pared) {
            //desvio += robot.diferencia_pasos * 0.0017;
        }
        else {
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

    }
    return desvio;
}

void robot_control() {

    static int16_t aux;
    //if (robot.estado == PARADO) return;
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

    // Control giros
    if (accion_get_radio() == GIRO_DERECHA_TODO and !robot.giro_corregido and pasos_recorridos > 120 and leds_get_pasos_minima_lectura_frontal() <100 ) {
        if (laberinto_hay_pared_izquierda(robot.casilla)) { // ya se cambio la variable orientacion
            aux = leds_get_pasos_distancia_minima(LED_DER) - 80;
        } else {
            aux = leds_get_pasos_minima_lectura_frontal() - 76; // La mitad de un giro 90
            /*
            Serial.print(leds_get_pasos_distancia_minima(LED_FIZQ));
            Serial.print(" ");
            Serial.print(leds_get_pasos_distancia_minima(LED_FDER));
            Serial.print(" ");
            Serial.print(leds_get_pasos_minima_lectura_frontal());
            Serial.print(" ");
            Serial.println(accion_get_pasos_objetivo());
            */
        }

        accion_set_pasos_objetivo(accion_get_pasos_objetivo() + aux * 0.0002 / LONGITUD_PASO_ENCODER);

        robot.giro_corregido = true;
    }
    if (accion_get_radio() == GIRO_IZQUIERDA_TODO and !robot.giro_corregido and pasos_recorridos > 120 and leds_get_pasos_minima_lectura_frontal() <100 ) {
        if (laberinto_hay_pared_derecha(robot.casilla)) { // ya se cambio la variable orientacion
            aux = leds_get_pasos_distancia_minima(LED_IZQ) - 80;
        } else {
            aux = 76 - leds_get_pasos_minima_lectura_frontal(); // La mitad de un giro 90
        }
        accion_set_pasos_objetivo(accion_get_pasos_objetivo() + aux * 0.0002 / LONGITUD_PASO_ENCODER);

        robot.giro_corregido = true;
    }


    if (accion_get_accion_actual() == AVANZA) {
        if (robot_get_angulo_desvio() != 0)
            motores_set_radio(1.0 / robot_get_angulo_desvio());
        else
            motores_set_radio(8888);
    }

    if ((leds_get_distancia(LED_FDER) + leds_get_distancia(LED_FIZQ)) / 2.0 < 0.01 and accion_get_radio() == RADIO_INFINITO) {
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

        if (accion_get_accion_actual() == PARA and leds_pared_enfrente()) {
            motores_set_velocidad_lineal_objetivo( 
                    min(
                    ACCION_V0 + ((leds_get_distancia(LED_FDER)+leds_get_distancia(LED_FIZQ)/2.0) - 0.028) * (ACCION_VE/(ACCION_VE-ACCION_V0)),
                    ACCION_VE));
            accion_set_pasos_objetivo(pasos_recorridos + (( (leds_get_distancia(LED_FIZQ) + leds_get_distancia(LED_FDER)) / 2.0)-0.028) / LONGITUD_PASO_ENCODER);
            /*
            Serial.print("parando: ");
            Serial.print(motores_get_velocidad_lineal_objetivo(),9);
            Serial.print(" ");
            Serial.print((leds_get_distancia(LED_FIZQ) + leds_get_distancia(LED_FDER) / 2.0),8);
            Serial.print(" ");
            Serial.print(pasos_recorridos);
            Serial.print(" ");
            Serial.println(accion_get_pasos_objetivo());
            */
        }

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
