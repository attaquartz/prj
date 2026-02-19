#ifndef __USER_OPERATION_H__
#define __USER_OPERATION_H__

#include "user_common.h"

typedef struct
{
	volatile uint32_t systick;
	volatile uint32_t modbus_server_tick;
	volatile uint32_t ID_VALVE_1_ON_TIME;
	volatile uint32_t ID_PUMP_1_ON_DELAY;
	volatile uint32_t ID_UV_LAMP_OFF_DELAY;
	volatile uint32_t ID_AERATION_3_OFF_DELAY;
	volatile uint32_t ID_PUMP_3_ON_TIME;
	volatile uint32_t ID_WATER_PUMP_4_TIMEOUT;
	volatile uint32_t ID_WATER_PUMP_1_ON_TIME;
	volatile uint32_t ID_WATER_PUMP_2_ON_TIME;
	volatile uint32_t ID_VALVE_3_ON_TIME;
	volatile uint32_t ID_VALVE_3_OFF_DELAY;
	volatile uint32_t ID_VALVE_4_ON_TIME;
	volatile uint32_t ID_VALVE_4_OFF_DELAY;
	volatile uint32_t ID_VALVE_5_ON_TIME;
	volatile uint32_t ID_VALVE_5_OFF_DELAY;
	volatile uint32_t ID_VALVE_6_ON_TIME;
	volatile uint32_t ID_VALVE_6_OFF_DELAY;
	volatile uint32_t ID_MOTOR_8_OFF_DELAY;
	volatile uint32_t ID_WATER_PUMP_5_ON_TIME;
	volatile uint32_t ID_VALVE_2_ON_TIME;
	volatile uint32_t ID_VALVE_2_ON_DELAY;
	volatile uint32_t ID_MOTOR_1_OP_TIME;
	volatile uint32_t ID_MOTOR_1_DIR_TIME;
	volatile uint32_t ID_FAN_1_OP_TIME;
	volatile uint32_t ID_FAN_2_OP_TIME;
	volatile uint32_t ID_PTC_FAN_OP_TIME;
	volatile uint32_t ID_MOTOR_2_OP_TIME;
	volatile uint32_t ID_MOTOR_2_DIR_TIME;
	volatile uint32_t ID_ACTUATOR_1_OP_TIME;
	volatile uint32_t ID_MOTOR_3_OP_TIME;
	volatile uint32_t ID_HEATER_1_OP_TIME;
	volatile uint32_t ID_HEATER_1_TEMP_UP_LIMIT_TIME;
	volatile uint32_t ID_HEATER_1_TEMP_UP_LIMIT_TIME_2;
	volatile uint32_t ID_PUMP_2_OP_TIME;
	volatile uint32_t ID_MOTOR_4_OP_TIME;
	volatile uint32_t ID_MOTOR_4_DIR_TIME;
	volatile uint32_t ID_MOTOR_5_OP_TIME;
	volatile uint32_t ID_MOTOR_5_DIR_TIME;
	volatile uint32_t ID_MOTOR_6_OP_TIME;
	volatile uint32_t ID_MOTOR_6_DIR_TIME;
	volatile uint32_t ID_MOTOR_7_OP_TIME;
	volatile uint32_t ID_RADIATOR_DELAY_TIME;
} timer_tick_t;

typedef struct
{
	uint16_t ID_SEQUENCE;
	
	uint16_t ID_DEV_OPERATION;
	uint16_t ID_DEV_STATE;
	uint16_t ID_POWER_METER_V;
	uint16_t ID_POWER_METER_A;
	uint16_t ID_POWER_METER_W;
	uint32_t ID_POWER_METER_WH;
	uint16_t ID_ENVIRONMENT_IN;
	uint16_t ID_ENVIRONMENT_OUT;
	uint16_t ID_FLOW_METER;
	uint16_t ID_FILTER_GAUGE;
	uint16_t ID_RADIATOR;
	uint16_t ID_FAN_3;
	uint16_t ID_SIGNAL_INDICATOR;
	
	uint16_t ID_VALVE_1;
	uint16_t ID_VALVE_1_START_TIME_INDEX;
	uint16_t ID_PUMP_1;
	uint16_t ID_UV_LAMP;
	uint16_t ID_AERATION_3;
	uint16_t ID_PUMP_3;
	uint16_t ID_PUMP_3_START_TIME_INDEX;
	uint16_t ID_VALVE_7;
	uint16_t ID_VALVE_8;
	uint16_t ID_WATER_PUMP_4;
	uint16_t ID_AERATION_1;
	uint16_t ID_AERATION_2;
	uint16_t ID_WATER_PUMP_1;
	uint16_t ID_WATER_PUMP_1_INTERVAL_INDEX;
	uint16_t ID_WATER_PUMP_1_START_TIME_INDEX;
	uint16_t ID_WATER_PUMP_2;
	uint16_t ID_WATER_PUMP_2_INTERVAL_INDEX;
	uint16_t ID_WATER_PUMP_2_START_TIME_INDEX;
	uint16_t ID_WATER_PUMP_3;
	uint16_t ID_VALVE_AIR_LIFTER_INDEX;
	uint16_t ID_VALVE_3;
	uint16_t ID_VALVE_4;
	uint16_t ID_VALVE_5;
	uint16_t ID_VALVE_6;
	uint16_t ID_MOTOR_8;
	uint16_t ID_MOTOR_8_OFF_TIME_INDEX;
	uint16_t ID_MOTOR_9;
	uint16_t ID_WATER_PUMP_5;
	uint16_t ID_WATER_PUMP_5_START_TIME_INDEX;
	uint16_t ID_VALVE_2;
	uint16_t ID_VALVE_2_OP_INDEX;
	uint16_t ID_MOTOR_1;
	uint16_t ID_MOTOR_1_INDEX;
	uint16_t ID_MOTOR_1_DIR;
	uint16_t ID_MOTOR_1_DIR_INDEX;
	uint16_t ID_FAN_1;
	uint16_t ID_FAN_2;
	uint16_t ID_PTC_FAN;
	uint16_t ID_MOTOR_2;
	uint16_t ID_MOTOR_2_INDEX;
	uint16_t ID_MOTOR_2_DIR;
	uint16_t ID_MOTOR_2_DIR_INDEX;
	uint16_t ID_ACTUATOR_1;
	uint16_t ID_MOTOR_3;
	uint16_t ID_HEATER_1;
	uint16_t ID_PUMP_2;
	uint16_t ID_MOTOR_4;
	uint16_t ID_MOTOR_4_INDEX;
	uint16_t ID_MOTOR_4_DIR;
	uint16_t ID_MOTOR_5;
	uint16_t ID_MOTOR_5_INDEX;
	uint16_t ID_MOTOR_5_DIR;
	uint16_t ID_MOTOR_6;
	uint16_t ID_MOTOR_6_INDEX;
	uint16_t ID_MOTOR_6_DIR;
	uint16_t ID_MOTOR_7;
	uint16_t ID_MOTOR_7_FG;
	uint16_t ID_MOTOR_7_OUTPUT;

	uint16_t ID_LEVEL_SENSOR;
	uint16_t ID_SWITCH;
	uint16_t ID_SENSOR;

} operation_parameter_t;

typedef struct
{
	uint32_t ID_EEPROM_ID;
	uint32_t ID_VALVE_1_INTERVAL;
	uint32_t ID_VALVE_1_START_TIME_1;
	uint32_t ID_VALVE_1_START_TIME_2;
	uint32_t ID_VALVE_1_START_TIME_3;
	uint32_t ID_VALVE_1_START_TIME_4;
	uint32_t ID_VALVE_1_START_TIME_5;
	uint32_t ID_VALVE_1_START_TIME_6;
	uint32_t ID_VALVE_1_START_TIME_7;
	uint32_t ID_VALVE_1_START_TIME_8;
	uint32_t ID_VALVE_1_START_TIME_9;
	uint32_t ID_VALVE_1_START_TIME_10;
	uint32_t ID_VALVE_1_ON_TIME;
	
	uint32_t ID_PUMP_1_ON_DELAY;
	
	uint32_t ID_UV_LAMP_OFF_DELAY;
	
	uint32_t ID_AERATION_3_OFF_DELAY;
	
	uint32_t ID_PUMP_3_INTERVAL;
	uint32_t ID_PUMP_3_START_TIME_1;
	uint32_t ID_PUMP_3_START_TIME_2;
	uint32_t ID_PUMP_3_START_TIME_3;
	uint32_t ID_PUMP_3_START_TIME_4;
	uint32_t ID_PUMP_3_START_TIME_5;
	uint32_t ID_PUMP_3_ON_TIME;
	
	uint32_t ID_WATER_PUMP_4_TIMEOUT;
	
	uint32_t ID_SLUDGE_DISCHARGE_INTERVAL;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_1;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_2;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_3;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_4;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_5;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_6;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_7;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_8;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_9;
	uint32_t ID_SLUDGE_DISCHARGE_START_TIME_10;
	
	uint32_t ID_WATER_PUMP_1_ON_TIME;
	
	uint32_t ID_WATER_PUMP_2_ON_TIME;
	
	uint32_t ID_VALVE_3_ON_TIME;
	uint32_t ID_VALVE_3_OFF_DELAY;
	uint32_t ID_VALVE_4_ON_TIME;
	uint32_t ID_VALVE_4_OFF_DELAY;
	uint32_t ID_VALVE_5_ON_TIME;
	uint32_t ID_VALVE_5_OFF_DELAY;
	uint32_t ID_VALVE_6_ON_TIME;
	uint32_t ID_VALVE_6_OFF_DELAY;
	
	uint32_t ID_MOTOR_8_OFF_DELAY;
	
	uint32_t ID_WATER_PUMP_5_INTERVAL;
	uint32_t ID_WATER_PUMP_5_START_TIME_1;
	uint32_t ID_WATER_PUMP_5_START_TIME_2;
	uint32_t ID_WATER_PUMP_5_START_TIME_3;
	uint32_t ID_WATER_PUMP_5_START_TIME_4;
	uint32_t ID_WATER_PUMP_5_START_TIME_5;
	uint32_t ID_WATER_PUMP_5_START_TIME_6;
	uint32_t ID_WATER_PUMP_5_START_TIME_7;
	uint32_t ID_WATER_PUMP_5_START_TIME_8;
	uint32_t ID_WATER_PUMP_5_START_TIME_9;
	uint32_t ID_WATER_PUMP_5_START_TIME_10;
	uint32_t ID_WATER_PUMP_5_START_TIME_11;
	uint32_t ID_WATER_PUMP_5_START_TIME_12;
	uint32_t ID_WATER_PUMP_5_START_TIME_13;
	uint32_t ID_WATER_PUMP_5_START_TIME_14;
	uint32_t ID_WATER_PUMP_5_START_TIME_15;
	uint32_t ID_WATER_PUMP_5_START_TIME_16;
	uint32_t ID_WATER_PUMP_5_START_TIME_17;
	uint32_t ID_WATER_PUMP_5_START_TIME_18;
	uint32_t ID_WATER_PUMP_5_START_TIME_19;
	uint32_t ID_WATER_PUMP_5_START_TIME_20;
	uint32_t ID_WATER_PUMP_5_START_TIME_21;
	uint32_t ID_WATER_PUMP_5_START_TIME_22;
	uint32_t ID_WATER_PUMP_5_START_TIME_23;
	uint32_t ID_WATER_PUMP_5_START_TIME_24;
	uint32_t ID_WATER_PUMP_5_ON_TIME;
	
	uint32_t ID_VALVE_2_ON_DELAY_1;
	uint32_t ID_VALVE_2_ON_DELAY_2;
	uint32_t ID_VALVE_2_ON_DELAY_3;
	uint32_t ID_VALVE_2_ON_DELAY_4;
	uint32_t ID_VALVE_2_ON_DELAY_5;
	uint32_t ID_VALVE_2_ON_DELAY_6;
	uint32_t ID_VALVE_2_ON_DELAY_7;
	uint32_t ID_VALVE_2_ON_DELAY_8;
	uint32_t ID_VALVE_2_ON_DELAY_9;
	uint32_t ID_VALVE_2_ON_DELAY_10;
	uint32_t ID_VALVE_2_ON_TIME;
	
	uint32_t ID_MOTOR_1_OP_TIME;
	uint32_t ID_MOTOR_1_CW_TIME;
	uint32_t ID_MOTOR_1_CCW_TIME;
	uint32_t ID_MOTOR_1_STOP_TIME;
	
	uint32_t ID_FAN_1_OP_TIME;
	uint32_t ID_FAN_2_OP_TIME;
	uint32_t ID_PTC_FAN_OP_TIME;
	
	uint32_t ID_MOTOR_2_DRY_OP_TIME;
	uint32_t ID_MOTOR_2_DRY_CW_TIME;
	uint32_t ID_MOTOR_2_DRY_CCW_TIME;
	uint32_t ID_MOTOR_2_DRY_STOP_TIME;
	uint32_t ID_MOTOR_2_CRUSH_OP_TIME;
	uint32_t ID_MOTOR_2_CRUSH_CW_TIME;
	uint32_t ID_MOTOR_2_CRUSH_CCW_TIME;
	uint32_t ID_MOTOR_2_CRUSH_STOP_TIME;
	uint32_t ID_MOTOR_2_OUT_OP_TIME;
	uint32_t ID_MOTOR_2_OUT_CW_TIME;
	uint32_t ID_MOTOR_2_OUT_CCW_TIME;
	uint32_t ID_MOTOR_2_OUT_STOP_TIME;
	
	uint32_t ID_ACTUATOR_1_OP_TIME;
	
	uint32_t ID_MOTOR_3_OP_TIME;
	
	uint32_t ID_HEATER_1_OP_TIME;
	uint32_t ID_HEATER_1_TEMP_UP_LIMIT_TIME;
	
	uint32_t ID_PUMP_2_OP_TIME;
	
	uint32_t ID_MOTOR_4_OP_TIME;
	uint32_t ID_MOTOR_4_CW_TIME;
	uint32_t ID_MOTOR_4_CCW_TIME;
	
	uint32_t ID_MOTOR_5_OP_TIME;
	uint32_t ID_MOTOR_5_ON_TIME;
	uint32_t ID_MOTOR_5_OFF_TIME;
	
	uint32_t ID_MOTOR_6_OP_TIME;
	uint32_t ID_MOTOR_6_CW_TIME;
	uint32_t ID_MOTOR_6_CCW_TIME;
	
	uint32_t ID_MOTOR_7_OP_TIME;
	uint32_t ID_MOTOR_7_SET_RPM;
	
	uint32_t ID_RADIATOR_ON_TEMP;
	uint32_t ID_RADIATOR_OFF_TEMP;
	
	uint32_t ID_HYSTERESIS_TIME;
	
	uint32_t ID_RTC_DATE;
	uint32_t ID_RTC_TIME;
	
} system_parameter_t;

extern timer_tick_t timer;
extern S_RTC_TIME_DATA_T rtc;
extern operation_parameter_t op;
extern system_parameter_t sp;

uint16_t crc16_ccitt(volatile uint8_t *buf, uint32_t len);

uint32_t RTCToUnixTimestamp(S_RTC_TIME_DATA_T *rtc);

void timer_interrupt_handler(void);
void input_interrupt_handler(void);

void emergency_handler(uint32_t state);

void uv_lamp_handler(void);
void temp_up_limit_time_handler(void);
void set_operation_mode(uint16_t mode);
void auto_handler(void);

void Main_Process(void);
void boot_loader(void);

#endif