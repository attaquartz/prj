#ifndef __USER_MOTOR_H__
#define __USER_MOTOR_H__

#include "user_common.h"

#define MOTOR_7_FREQ 2000
#define MOTOR_7_RPM_MAX 12000
#define MOTOR_7_RPM(rpm) (100 - ((rpm) * 100 / MOTOR_7_RPM_MAX))
#define MOTOR_7_RPM_TIMEOUT 5

extern volatile uint32_t motor_7_fg_last_count;
extern volatile uint32_t motor_7_watchdog;

void motor_1_handler(void);
void motor_2_handler(void);
void motor_3_handler(void);
void motor_4_handler(void);
void motor_5_handler(void);
void motor_6_handler(void);
void motor_7_reset(void);
void motor_7_handler(void);
void motor_7_set(uint32_t state);
void motor_8_scheduler_init(void);
void motor_8_handler(void);
void motor_9_handler(void);
void actuator_1_handler(void);

#endif