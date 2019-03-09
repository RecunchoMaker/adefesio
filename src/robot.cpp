#include <Arduino.h>
#include <settings.h>
#include <robot.h>
#include <motores.h>
#include <encoders.h>
#include <leds.h>
#include <timer1.h>
#include <laberinto.h>
#include <accion.h>

volatile int32_t pasos_recorridos = 0;

volatile bool sinc_pared = false;

typedef struct {
    volatile tipo_estado estado:3;
    volatile tipo_orientacion orientacion: 2;
    volatile uint8_t casilla = 0;
    volatile float casilla_offset = 0;
} tipo_robot;

volatile tipo_robot robot;



void robot_init() {
    
    encoders_reset_posicion_total();
    encoders_reset_posicion();
    motores_parar();

    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;

} 

void _incrementa_casilla() {
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
    sinc_pared = false;
}

void robot_siguiente_accion() {

    if (accion_cambio_casilla()) {
        _incrementa_casilla();
        Serial.print("cambio a casilla ");
        Serial.println(robot.casilla);
        laberinto_print();
    }

    // TODO: el offset tampoco es lineal en los giros
    if (accion_get_radio() != 0) robot.casilla_offset += ( pasos_recorridos * LONGITUD_PASO_ENCODER);

    switch (robot.estado) {
        case PARADO:
            motores_parar();
            encoders_reset_posicion_total();
            break;
        case EXPLORANDO_INICIAL:
            accion_ejecuta(ARRANCAR);
            sinc_pared = false;
            robot.casilla_offset = LABERINTO_LONGITUD_CASILLA / 2;
            robot.estado = EXPLORANDO;
            break;
        case EXPLORANDO:
            Serial.print("continuo - leds enfrente: ");
            Serial.print(leds_get_valor(LED_FDER));
            Serial.print("/");
            Serial.println(leds_get_valor(LED_FIZQ));

            if (!leds_pared_derecha()) {
                 Serial.println("pp giro derecha");

                 accion_ejecuta(PARAR);
                 sinc_pared = false;

                 robot.estado = PAUSA_PRE_GIRO;
                 robot.casilla_offset = 0;
            } else if (!leds_pared_enfrente()) {
                 accion_ejecuta(RECTO);
                 sinc_pared = false;


            } else if (!leds_pared_izquierda()) {
                 Serial.println("paro para giro izquierda");
                 accion_ejecuta(PARAR);

                 sinc_pared = false;
                 robot.estado = PAUSA_PRE_GIRO;
                 robot.casilla_offset = 0;
            } else {
                 Serial.println("callejon sin salida - paro para giro 180");
                 accion_ejecuta(PARAR);

                 sinc_pared = false;
                 robot.estado = PAUSA_PRE_GIRO;
                 robot.casilla_offset = 0;
            }
            break;

        case PAUSA_INICIAL:
            Serial.println("pausa inicial");
            accion_ejecuta(ESPERA);
            sinc_pared = false;
            robot.estado = EXPLORANDO_INICIAL;
            laberinto_print();
            break;

        case PAUSA_PRE_GIRO:
            Serial.println("pausa pregiro");
            accion_ejecuta(ESPERA);
            sinc_pared = false; 
            laberinto_set_pared_frontal(robot.casilla, leds_pared_enfrente());
            robot.estado = PARADO_PARA_GIRO;
            break;

        case PARADO_PARA_GIRO:
            if (!leds_pared_derecha()) {
                Serial.println("giro derecha");
                accion_ejecuta(GIRO_DERECHA);
                sinc_pared = false;
                robot.orientacion++;
                robot.casilla_offset = 0;
                robot.estado = PAUSA_INICIAL;
            } else if (!leds_pared_izquierda()) {
                Serial.println("giro izquierda");
                accion_ejecuta(GIRO_IZQUIERDA);
                sinc_pared = false;
                robot.orientacion--;
                robot.casilla_offset = 0;
                robot.estado = PAUSA_INICIAL;
            } else {
                Serial.println("giro 180");
                accion_ejecuta(GIRO_180);
                sinc_pared = false;
                robot.orientacion+=2;
                robot.casilla_offset = 0;
                robot.estado = PAUSA_INICIAL;
            }
            break;

    }


    pasos_recorridos = 0;
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

    robot.estado = EXPLORANDO_INICIAL;
    robot.casilla = CASILLA_INICIAL;
    robot.orientacion = ORIENTACION_INICIAL;
    
    laberinto_set_paredes_laterales(robot.casilla, 
            leds_pared_izquierda(), leds_pared_derecha());
    robot_siguiente_accion();
}

float robot_get_casilla_offset() {
    return robot.casilla_offset;
}

void robot_control() {

    // control de posicion
    if (accion_get_radio()!=GIRO_DERECHA_TODO and accion_get_radio()!=GIRO_IZQUIERDA_TODO and robot.casilla_offset + pasos_recorridos * LONGITUD_PASO_ENCODER > LABERINTO_LONGITUD_CASILLA) {
        Serial.println("Retrocedo offset");
        robot.casilla_offset -= LABERINTO_LONGITUD_CASILLA;
    }

    // Sincronizacion con paredes
    if (!leds_pared_derecha() and laberinto_hay_pared_derecha(robot.casilla) and accion_get_radio() == RADIO_INFINITO and accion_get_distancia() == LABERINTO_LONGITUD_CASILLA and !sinc_pared) {
        Serial.println("sinc pared derecha");
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        accion_set_pasos_objetivo(pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER);
    }

    if (!leds_pared_izquierda() and laberinto_hay_pared_izquierda(robot.casilla) and accion_get_radio() == RADIO_INFINITO and accion_get_distancia() == LABERINTO_LONGITUD_CASILLA and !sinc_pared) {
        Serial.println("sinc pared derecha");
        sinc_pared = true;
        // TODO: quedan 4 cm para llegar al final de la casilla
        accion_set_pasos_objetivo(pasos_recorridos + 0.04 / LONGITUD_PASO_ENCODER);
    }

    if ((leds_get_valor(LED_FDER) + leds_get_valor(LED_FIZQ) > 600) and accion_get_radio() == RADIO_INFINITO) {
        Serial.println("chocamos! leds:");
        Serial.print(leds_get_valor(LED_FDER));
        Serial.print("/");
        Serial.print(leds_get_valor(LED_FIZQ));
        Serial.print(", pasos objetivo =");
        Serial.print(accion_get_pasos_objetivo());
        Serial.print(" recorridos =");
        Serial.println(pasos_recorridos);
        accion_set_pasos_objetivo(pasos_recorridos);
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
