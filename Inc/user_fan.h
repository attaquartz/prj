#ifndef __USER_FAN_H__
#define __USER_FAN_H__

#include "user_common.h"

void fan_1_reset(void);
void fan_1_handler(void);
void fan_2_reset(void);
void fan_2_handler(void);
void fan_3_reset(void);
void fan_3_handler(void);
void ptc_fan_reset(void);
void ptc_fan_handler(void);

#endif