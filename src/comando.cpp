#include <Arduino.h>
#include <comando.h>
#include <motores.h>
#include <robot.h>

#define COMANDO_MAX_TAMANO 16
#define COMANDO_DELIMITADOR ' '
#define COMANDO_FIN 13

#define COMANDO_PROMPT "A> "

static char buffer[COMANDO_MAX_TAMANO];

static uint8_t buffer_idx;
char caracter;
int8_t aux_i;
bool go = false;

char const *comando_kp = "kp";       // set KP
char const *comando_kd = "kd";       // set KD
char const *comando_ki = "ki";       // set KI
char const *comando_kpa = "kpa";     // set KP angular
char const *comando_amax = "amax";   // set aceleracion maxima
char const *comando_acur = "acur";   // set aceleracion para entrada en curbas
char const *comando_afin = "afin";   // set deceleracion final
char const *comando_vc = "vc";       // set velocidad en curva
char const *comando_vr = "vr";       // set velocidad en recta
char const *comando_dr = "dr";       // set distancia entre ruedas

void comando_prompt() {
    go = false;
    Serial.print(COMANDO_PROMPT);
}

void comando_init() {
    for (aux_i=0; aux_i<COMANDO_MAX_TAMANO; aux_i++)
        buffer[aux_i] = '-';
    buffer_idx = 0;
}

void comando_lee_serial() {
    while (Serial.available()) {
        caracter = Serial.read();
        // turn around - me salto el caracter 10 que acompana a 13
        if (caracter == 10) continue;

        buffer[buffer_idx] = caracter;

        if (caracter == COMANDO_FIN) {
            if (buffer_idx > 0) {
                buffer[buffer_idx] = 0;
                _procesa_buffer();
                comando_init();
            }
        } else {
            buffer_idx += (buffer_idx > COMANDO_MAX_TAMANO-1?0:1);
        }
    }
}

void _procesa_buffer() {

    for (aux_i = 0;
         buffer[aux_i] != COMANDO_DELIMITADOR and aux_i<COMANDO_MAX_TAMANO; 
         aux_i++);

    if (buffer[aux_i] == COMANDO_DELIMITADOR) {
        // Comando con token + valor
        buffer[aux_i] = 0;
        _procesa_comando(buffer, buffer + aux_i + 1 );
    } else {
        // Comando con un unico token
        _procesa_comando(buffer);
    }
    if (!go)
        comando_prompt();
}

void _procesa_setter(char const *comando, char *token, char *parametro, 
        void (*setter)(float), float (*getter)()) {

    if (!strcmp(token, comando)) {
        (*setter) (atof(parametro));
        Serial.print(token);
        Serial.print(" = ");
        Serial.println((*getter)(),5);
    }
}

void _procesa_comando(char *token, char *parametro) {

    _procesa_setter(comando_kp, token, parametro, motores_set_kp_lineal, motores_get_kp_lineal);
    _procesa_setter(comando_kd, token, parametro, motores_set_kd_lineal, motores_get_kd_lineal);
    _procesa_setter(comando_ki, token, parametro, motores_set_ki_lineal, motores_get_ki_lineal);
    _procesa_setter(comando_kpa, token, parametro, motores_set_kp_angular, motores_get_kp_angular);
    _procesa_setter(comando_amax, token, parametro, robot_set_amax, robot_get_amax);
    _procesa_setter(comando_acur, token, parametro, robot_set_acur, robot_get_acur);
    _procesa_setter(comando_afin, token, parametro, robot_set_afin, robot_get_afin);
    _procesa_setter(comando_vc, token, parametro, robot_set_vc, robot_get_vc);
    _procesa_setter(comando_vr, token, parametro, robot_set_vr, robot_get_vr);
    _procesa_setter(comando_vr, token, parametro, robot_set_vr, robot_get_vr);
    _procesa_setter(comando_dr, token, parametro, motores_set_distancia_entre_ruedas, motores_get_distancia_entre_ruedas);

}

void _procesa_comando(char *token) {
    if (!strcmp(token, "go")) {
        go = true;
    }
}

bool comando_go() {
    return go;
}
