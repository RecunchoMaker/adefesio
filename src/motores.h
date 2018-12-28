#ifndef __MOTORES_H
#define __MOTORES_H

// H-bridge pins
#define MOTOR_LEFT_PWM 5 
#define MOTOR_LEFT_IN1 9
#define MOTOR_LEFT_IN2 4

#define MOTOR_RIGHT_PWM 6 
#define MOTOR_RIGHT_IN1 7
#define MOTOR_RIGHT_IN2 10

#define MAX_PWM 200

void motores_init(void);
void motores_set_pwm(int16_t left, int16_t right);

#endif /* ifndef MOTORES_H

 */
