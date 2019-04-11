/**
 * @file comando.cpp
 *
 * @brief Un prompt simple por el puerto serie
 *
 * El prompt admite la introducción de comandos/instrucciones por el puerto serie, con la forma
 * <tt>comando parametro</tt>. Se admite únicamente 1 o ningún parámetro.
 *
 * Uso: se llama una única vez a <tt>comando_init()</tt> y se llama en el bucle
 * de control a <tt>comando_lee_serial()</tt> (llamada no bloqueante). 
 *
 * Para crear un nuevo comando, se crea una <tt>constant char</tt> con el nombre del comando,
 * y se añaden en la funcion <tt>_procesa_comando()</tt> que está
 * sobrecargada para considerar los casos de 1 parámetro (getters) o ninguno.
 *
 * @code
 * char const *comando_nuevo = "restart";
 * void _procesa_comando(char *token) {
 *
 *   (...)
 *   _procesa_comando_simple(comando_nuevo, token, funcion_restart);
 *
 * }
 * @endcode
 *
 * El comando especial @p go se puede utilizar en el caso de que se necesite esperar por la
 * introducción de varios comandos antes de continuar con el proceso, por ejemplo:
 * @code
 *
 * // espero la introducción de varios comandos
 * while (!comando_get_go()) {
 *     comando_lee_serial();
 * }
 *
 * // continuo proceso
 * @endcode
 *
 * @see https://recunchomaker.github.io/adefesio/interprete-de-comandos-sencillo/
 */

#include <Arduino.h>
#include <comando.h>
#include <accion.h>
#include <motores.h>
#include <robot.h>
#include <leds.h>
#include <camino.h>
#include <log.h>

/// Máxima longitud permitida para un comando
#define COMANDO_MAX_TAMANO 16

/// Caracter separador de comando/argumento
#define COMANDO_DELIMITADOR ' '

/// Caracter interpretado como fin de comando
#define COMANDO_FIN 13

/// Prompt visible cuando el intérprete está disponible
#define COMANDO_PROMPT "A> "

/// Almacena los caracteres introducidos
static char buffer[COMANDO_MAX_TAMANO];

/// Posicion en el buffer del caracter que se va a introducir
static uint8_t buffer_idx;

/// Alamacena el último caracter introducido por el puerto serie
char caracter;

/// Auxiliar utilizado en varios bucles que recorren el biffer
int8_t aux_i;

/// Flag de control del comando go
bool go = false;

//@{
/**
 * @name Constantes de comandos
 */
char const *comando_go = "go";   ///< comando especial para permitir la espera de un comando completo en el bucle principal

char const *comando_kp = "kp";       ///< constante P del pid para velocidad lineal
char const *comando_kd = "kd";       ///< D
char const *comando_ki = "ki";       ///< I
char const *comando_kp1 = "kp1";     ///< constante P del pid para correcion en pasillo en funcion del desvio lateral
char const *comando_kp2 = "kp2";     ///< constante P del pid para correcion en pasillo en funcion del incremento del desvio
char const *comando_amax = "amax";   ///< aceleracion maxima
char const *comando_acur = "acur";   ///< aceleracion para entrada en curvas
char const *comando_afin = "afin";   ///< deceleracion final
char const *comando_vc = "vc";       ///< velocidad maxima en curva
char const *comando_vr = "vr";       ///< velocidad maxima en recta
char const *comando_ve = "ve";       ///< velocidad maxima en exploracion
char const *comando_dr = "dr";       ///< distancia entre ruedas

char const *comando_l1 = "l1";       ///< activar leds
char const *comando_l0 = "l0";       ///< desactivar leds

char const *comando_pr = "pr";       ///< anadir PASO_RECTO a camino
char const *comando_pd = "pd";       ///< anadir PASO_DER a camino
char const *comando_pi = "pi";       ///< anadir PASO_IZQ a camino

/// comandos para MOCK
char const *comando_ld = "ld";       ///< setear distancia del led derecho
char const *comando_li = "li";       ///< setear distancia del led izquierdo
char const *comando_lf = "lf";       ///< setear distancia del led frontal
char const *comando_sa = "sa";       ///< lanza siguiente accion

//@}


/**
 * @brief Inicializa el intérprete 
 *
 * Se vacía el bufer contenedor de la instrucción y se resetea
 * el índice
 */
void comando_init() {
    for (aux_i=0; aux_i<COMANDO_MAX_TAMANO; aux_i++)
        buffer[aux_i] = '-';
    buffer_idx = 0;
}


/**
 * @brief Muestra el prompt del intérprete
 */
void comando_prompt() {
    go = false;
    Serial.print(COMANDO_PROMPT);
}


/**
 * @brief Ejecuta el setter especificado si @p token coincide con @p comando
 *
 * La mayoría de los comandos de 1 parámetro son setters, y esta función
 * ejecuta dicho setter y muestra luego el resultado a efectos de comprobación
 *
 * @param comando Constante con la que comparar el token recibido
 * @param token Token del comando
 * @param parametro Valor del parámetro que se va a enviar al setter
 * @param setter puntero a la función setter
 * @param getter puntero a la funcion getter para mostrar el resultado
 */
void _procesa_setter(char const *comando, char *token, char *parametro, 
        void (*setter)(float), float (*getter)()) {

    if (!strcmp(token, comando)) {
        (*setter) (atof(parametro));
        if (getter) {
            Serial.print(token);
            Serial.print(" = ");
            Serial.println((*getter)(),5);
        }
    }
}


/**
 * @brief Ejecuta el comando especificado si @p token coincide con @p comando
 *
 * @param comando Constante con la que comparar el token recibido
 * @param token Token del comando
 * @param funcion Puntero a la función que ejecuta el comando
 */
void _procesa_comando_simple(char const *comando, char *token, void (*funcion)(void)) {
    if (!strcmp(token, comando)) {
        (*funcion)();
    }
}


/**
 * @brief Lanza la funcion correspondiente a token
 *
 * @param token Token del comando
 */
void _procesa_comando(char *token) {

    _procesa_comando_simple(comando_l0, token, leds_desactiva);
    _procesa_comando_simple(comando_l1, token, leds_activa);
    _procesa_comando_simple(comando_go, token, comando_set_go);
    _procesa_comando_simple(comando_pr, token, camino_anadir_paso_recto);
    _procesa_comando_simple(comando_pd, token, camino_anadir_paso_der);
    _procesa_comando_simple(comando_pi, token, camino_anadir_paso_izq);
    _procesa_comando_simple(comando_sa, token, mock_siguiente_accion);
}


/**
 * @brief Lanza la funcion correspondiente a token con un parametro
 *
 * @param token Token del comando
 * @param parametro Valor del parámetro que se va a enviar a la funcion
 *
 * @note Actualmente todas las funciones de un parámetro se corresponden con setters
 */
void _procesa_comando(char *token, char *parametro) {

    _procesa_setter(comando_kp, token, parametro, motores_set_kp_lineal, motores_get_kp_lineal);
    _procesa_setter(comando_kd, token, parametro, motores_set_kd_lineal, motores_get_kd_lineal);
    _procesa_setter(comando_ki, token, parametro, motores_set_ki_lineal, motores_get_ki_lineal);
    _procesa_setter(comando_kp1, token, parametro, motores_set_kp_pasillo1, motores_get_kp_pasillo1);
    _procesa_setter(comando_kp2, token, parametro, motores_set_kp_pasillo2, motores_get_kp_pasillo2);
    _procesa_setter(comando_amax, token, parametro, accion_set_amax, accion_get_amax);
    _procesa_setter(comando_acur, token, parametro, accion_set_acur, accion_get_acur);
    _procesa_setter(comando_afin, token, parametro, accion_set_afin, accion_get_afin);
    _procesa_setter(comando_vc, token, parametro, accion_set_vc, accion_get_vc);
    _procesa_setter(comando_vr, token, parametro, accion_set_vr, accion_get_vr);
    _procesa_setter(comando_ve, token, parametro, accion_set_ve, accion_get_ve);
    _procesa_setter(comando_dr, token, parametro, motores_set_distancia_entre_ruedas, motores_get_distancia_entre_ruedas);
    _procesa_setter(comando_ld, token, parametro, leds_mock_led_d, nullptr);
    _procesa_setter(comando_li, token, parametro, leds_mock_led_i, nullptr);
    _procesa_setter(comando_lf, token, parametro, leds_mock_led_f, nullptr);

}


/**
 * @brief Lanza el comando que está en el buffer
 *
 * Se comprueba si se ha introducido un comando con parámetro o sin el,
 * y se lanza el procesameinto correspondiente
 */
void _procesa_buffer() {

    for (aux_i = 0;
         buffer[aux_i] != COMANDO_DELIMITADOR and aux_i<COMANDO_MAX_TAMANO; 
         aux_i++);

    if (buffer[aux_i] == COMANDO_DELIMITADOR) {
        // Comando con token + valor
        buffer[aux_i] = 0;
        Serial.print("buffer = ");
        Serial.println(buffer + aux_i + 1);
        _procesa_comando(buffer, buffer + aux_i + 1 );
    } else {
        // Comando con un unico token
        _procesa_comando(buffer);
    }
    if (!go)
        comando_prompt();
}


/**
 * @brief Lee el puerto serie y lanza el procesamiento al recibir un fin de comando
 *
 * La funcion no es bloqueante (se puede llamar en un loop)
 *
 * @note Siempre se salta el caracter 10, que aconpaña al 13 en el modo CRLF
 */
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


/**
 * @brief Setter del flag @p go
 *
 */
void comando_set_go() {
    go = true;
}


/**
 * @brief Getter del flag @p go
 */
bool comando_get_go() {
    return go;
}
