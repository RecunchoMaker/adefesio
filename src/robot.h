#ifndef __ROBOT_H
#define __ROBOT_H

void robot_init();
void robot_siguiente_accion();
void robot_control();
uint8_t robot_get_accion();



float _distancia_para_decelerar(float velocidad, float aceleracion);

#endif /* ifndef ROBOT_H

 */
