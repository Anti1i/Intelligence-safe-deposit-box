#ifndef __DC_MOTOR_H
#define __DC_MOTOR_H

#include <stdint.h>

#define MOTOR_STOP      0
#define MOTOR_FORWARD   1
#define MOTOR_BACKWARD  2

void DC_Motor_Control(uint8_t state);

#endif
