#ifndef __ROBOT_H
#define __ROBOT_H

void robot_init();
void robot_siguiente_accion();
void robot_control();

float _distancia_para_decelerar(float velocidad, float aceleracion);

#endif /* ifndef ROBOT_H

 */
