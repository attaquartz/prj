#ifndef __USER_INPUT_H__
#define __USER_INPUT_H__

#include "user_common.h"

#define LEVEL_SENSOR(i) ((i) == 0 ? SENSOR_LEVEL_H_FILTER : (i) == 1 ? SENSOR_LEVEL_L_FILTER : (i) == 2 ? SENSOR_LEVEL_LL_FILTER : (i) == 3 ? SENSOR_LEVEL_L_REUSE : (i) == 4 ? SENSOR_LEVEL_L_EQ : (i) == 5 ? SENSOR_LEVEL_L_S2 : 0)
#define ALERT_SWITCH(i) ((i) == 0 ? SENSOR_TEMP_1 : (i) == 1 ? SENSOR_TEMP_2 : (i) == 2 ? SENSOR_TEMP_3 : 0)
#define USER_SWITCH(i) ((i) == 0 ? SW_AUTO_START : (i) == 1 ? SW_AUTO_STOP : (i) == 2 ? SW_FAUCET : (i) == 3 ? SW_EMERGENCY : 0)

#define MODBUS_SERVER_DETECT_MS 10000
#define ZERO_CROSSING_DETECT_MS 1000
#define LEVEL_SENSOR_DETECT_MS 10000
#define ALERT_SENSOR_DETECT_MS 200
#define USER_SWITCH_DETECT_MS 200

#define LEVEL_SENSOR_MAX 6
#define ALERT_SENSOR_MAX 5
#define USER_SWITCH_MAX 5
#define CT_SENSOR_MAX 5

typedef struct
{
	bool flag;
	uint8_t state;
	uint32_t tick;
} input_t;

typedef struct
{
	bool adc_done_flag;
	uint32_t adc[CT_SENSOR_MAX];
	uint8_t state[CT_SENSOR_MAX];
} ct_t;

extern input_t zero_crossing;
extern input_t level_sensor[LEVEL_SENSOR_MAX];
extern input_t alert_sensor[ALERT_SENSOR_MAX];
extern input_t user_switch[USER_SWITCH_MAX];
extern ct_t ct_sensor;

void zero_crossing_handler(void);
void level_sensor_handler(uint32_t lv);
void alert_sensor_handler(uint32_t alert);
void user_switch_handler(uint32_t sw);
void ct_sensor_handler(void);

#endif