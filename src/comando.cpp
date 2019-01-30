#include <Arduino.h>
#include <comando.h>
#include <motores.h>

#define COMANDO_MAX_TAMANO 16
#define COMANDO_DELIMITADOR ' '
#define COMANDO_FIN 13

#define COMANDO_PROMPT "A> "

static char buffer[COMANDO_MAX_TAMANO];

static uint8_t buffer_idx;
char caracter;
int8_t aux_i;
bool go = false;

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

void _procesa_comando(char *token, char *parametro) {
    if (!strcmp(token, "kp")) {
        motores_set_kp_lineal(atof(parametro));
        Serial.print("kp=");
        Serial.println(motores_get_kp_lineal());
    } else if (!strcmp(token, "kd")) {
        motores_set_kd_lineal(atof(parametro));
        Serial.print("kd=");
        Serial.println(motores_get_kd_lineal());
    }
    else if (!strcmp(token, "ki")) {
        motores_set_ki_lineal(atof(parametro));
        Serial.print("ki=");
        Serial.println(motores_get_ki_lineal());
    }
    else if (!strcmp(token, "vm")) {
        motores_set_maxima_velocidad_lineal(atof(parametro));
        Serial.print("v.max=");
        Serial.println(motores_get_maxima_velocidad_lineal());
    }
    else if (!strcmp(token, "am")) {
        motores_set_maxima_aceleracion_lineal(atof(parametro));
        Serial.print("a.max=");
        Serial.println(motores_get_maxima_aceleracion_lineal());
    }
    else if (!strcmp(token, "va")) {
        motores_set_maxima_velocidad_angular(atof(parametro));
        Serial.print("v.a.max=");
        Serial.println(motores_get_maxima_velocidad_angular());
    }
    else if (!strcmp(token, "aa")) {
        motores_set_maxima_aceleracion_angular(atof(parametro));
        Serial.print("a.a.max=");
        Serial.println(motores_get_maxima_aceleracion_angular());
    }
}

void _procesa_comando(char *token) {
    if (!strcmp(token, "go")) {
        go = true;
    }
}

bool comando_go() {
    return go;
}
