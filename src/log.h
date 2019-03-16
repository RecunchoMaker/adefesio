/**
 * @file log.h
 *
 * @brief Cabecera de log.cpp
 */

#ifndef __LOG_H
#define __LOG_H

//@{
/**
 * @name Helpers para imprimir variables
 */
#define LOG(x) Serial.print(x); Serial.print(" ");
#define LOGF(x,d) Serial.print(x,d); Serial.print(" ");
#define LOGN(x) Serial.println(x)
#define LOGFN(x,d) Serial.print(x,d); Serial.println();
//@}

void log_leds();
void log_accion();
void log_motores();
void log_pasos();
void log_casilla_pasos_leds();
void log_cambio_casilla();

#endif /* ifndef LOG_H

 */

