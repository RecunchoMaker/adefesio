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
    }
    if (!strcmp(token, "kpa")) {
        motores_set_kp_angular(atof(parametro));
        Serial.print("kpa=");
        Serial.println(motores_get_kp_angular(),5);
    }
    else if (!strcmp(token, "kd")) {
        motores_set_kd_lineal(atof(parametro));
        Serial.print("kd=");
        Serial.println(motores_get_kd_lineal());
    }
    else if (!strcmp(token, "ki")) {
        motores_set_ki_lineal(atof(parametro));
        Serial.print("ki=");
        Serial.println(motores_get_ki_lineal());
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
