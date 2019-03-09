#include <accion.h>
#include <settings.h>
#include <motores.h>
#include <timer1.h>
#include <log.h>

// Aceleracion maxima, usada en las rectas
#define ACCION_AMAX 1.0

// Aceleracion de frenada, antes de entrar en una curba
#define ACCION_ACUR 1.0

// Aceleracion de la frenada final en la ultima casilla
#define ACCION_AFIN 0.8

// Velocidad maxima en recta
#define ACCION_VR 0.40

// Velocidad maxima en curva
#define ACCION_VC 0.30

// Velocidad maxima en exploracion
#define ACCION_VE 0.20

// Velocidad minima antes de parar
#define ACCION_V0 0.1

// Longitud del arco que se describe en una curba
#define ACCION_DISTG (2*PI*(LABERINTO_LONGITUD_CASILLA/2)/4)


volatile float distancia;
volatile int32_t pasos_objetivo;
volatile int32_t pasos_hasta_decelerar;
volatile float aceleracion;
volatile float deceleracion;
volatile float velocidad_maxima;
volatile float velocidad_final;
volatile float accion_radio;

volatile float amax = ACCION_AMAX;
volatile float acur = ACCION_ACUR;
volatile float afin = ACCION_AFIN;
volatile float vr = ACCION_VR;
volatile float vc = ACCION_VC;
volatile float ve = ACCION_VE;

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

void accion_set_pasos_objetivo(int32_t pasos) {
    pasos_objetivo = pasos;
}

int32_t accion_get_pasos_hasta_decelerar() {
    return pasos_hasta_decelerar;
}

float _distancia_para_decelerar(float vi, float vf, float aceleracion) {
    return ((0.5 * (vi - vf) * (vi - vf)) + (vf * (vi - vf))) / aceleracion;
}

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

    log_accion();
    timer1_reset_cuenta();

    // TODO: mirar mejor esto
    encoders_reset_posicion_total();

}


void accion_ejecuta(char accion) {
    if (accion == ARRANCAR) {
        //accion_set(LABERINTO_LONGITUD_CASILLA/2, amax, amax, ve, ve, INFINITO);
        accion_set(LABERINTO_LONGITUD_CASILLA/2, amax, amax, ve, ve, RADIO_INFINITO);
    } else if (accion ==  PARAR) {
        //accion_set(LABERINTO_LONGITUD_CASILLA/2, amax, afin, ve, ROBOT_V0, INFINITO)
        accion_set(LABERINTO_LONGITUD_CASILLA/2, amax, afin, ve, ACCION_V0, RADIO_INFINITO);
    } else if (accion == RECTO) {
        // accion_set(LABERINTO_LONGITUD_CASILLA, amax, amax, ve, ve, INFINITO);
        accion_set(LABERINTO_LONGITUD_CASILLA, amax, amax, ve, ve, RADIO_INFINITO);
    } else if (accion == ESPERA) {
        accion_set(0, 0, 0, 0, 0, 0.1); // espera 0.2 segundos
    } else if (accion == GIRO_DERECHA) {
        // accion_set(-PI*motores_get_distancia_entre_ruedas()/4.0, amax, amax , 0.2 , ROBOT_V0, GIRO_DERECHA_TODO); // gira 90
        accion_set(-PI*motores_get_distancia_entre_ruedas()/4.0, amax, amax, ve, ACCION_V0, GIRO_DERECHA_TODO); // gira 90
    } else if (accion == GIRO_IZQUIERDA) {
        accion_set(PI*motores_get_distancia_entre_ruedas()/4.0, amax, amax, ve , ACCION_V0, GIRO_IZQUIERDA_TODO); // gira 90
    } else if (accion == GIRO_180) {
        // accion_set(PI*motores_get_distancia_entre_ruedas()/2.0, amax, amax , 0.2, ROBOT_V0, GIRO_IZQUIERDA_TODO); // gira 180g
        accion_set(PI*motores_get_distancia_entre_ruedas()/2.0, amax, amax, ve, ACCION_V0, GIRO_IZQUIERDA_TODO); // gira 180g
    } else {
        Serial.println("No existe accion!");
    }
}

bool accion_cambio_casilla() {
    // TODO: valido sólo en exploración
    // Si vamos rectos y no vamos a frenar la acción implica un cambio de casilla
    return (accion_radio == RADIO_INFINITO and velocidad_final > ACCION_V0 );
}
