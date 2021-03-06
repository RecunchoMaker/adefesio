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
#define LOG(x) Serial.print(x); Serial.print("\t");
#define LOGF(x,d) Serial.print(x,d); Serial.print("\t");
#define LOGN(x) Serial.println(x)
#define LOGFN(x,d) Serial.print(x,d); Serial.println();
//@}

void log_leds();
void log_accion();
void log_motores();               //#5
void log_pasos();
void log_casilla_pasos_leds();    //#1
void log_cambio_casilla();
void log_variables_trayectoria(); //#2
void log_camino();
void log_leds_distancias();       //#3
void log_correccion_pasillos();   //#4

#endif /* ifndef LOG_H

 */

