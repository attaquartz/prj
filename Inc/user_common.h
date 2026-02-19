#ifndef __USER_COMMON_H__
#define __USER_COMMON_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "NUC029xGE.h"
#include "user_operation.h"
#include "user_valve.h"
#include "user_pump.h"
#include "user_water_pump.h"
#include "user_motor.h"
#include "user_aeration.h"
#include "user_heater.h"
#include "user_fan.h"
#include "user_input.h"
#include "nanomodbus.h"
#include "user_modbus.h"
#include "user_flash.h"
#include "user_eeprom.h"
#include "user_event.h"

#define SW_VERSION 0x0100
#define PRODUCT_ID 0X0100

#define HMI_UART UART2
#define HMI_RE_DE PD5
#define HMI_RX UART2_RXD
#define HMI_TX UART2_TXD

#define PC_UART UART0
#define PC_RE_DE PD11
#define PC_TX UART0_TXD
#define PC_RX UART0_RXD

#define MOTOR_1 PE13
#define MOTOR_1_DIR PB15
#define MOTOR_2 PB14
#define MOTOR_2_DIR PB13
#define MOTOR_3 PE11
#define MOTOR_4 PE10
#define MOTOR_5 PC11
#define MOTOR_6 PE12
#define MOTOR_7_PWM PWM1_CH1
#define MOTOR_7_FG PC9
#define MOTOR_8 PE8
#define MOTOR_9 PE1

#define PUMP_1 PF3
#define PUMP_2 PD7
#define PUMP_3 PC4

#define WATER_PUMP_1 PC6
#define WATER_PUMP_2 PC5
#define WATER_PUMP_3 PD15
#define WATER_PUMP_4 PD10
#define WATER_PUMP_5 PC8

#define AERATION_1 PE5
#define AERATION_2 PE4
#define AERATION_3 PC7

#define VALVE_1 PA14
#define VALVE_2 PA0
#define VALVE_3 PA3
#define VALVE_4 PB12
#define VALVE_5 PC15
#define VALVE_6 PF6
#define VALVE_7 PA10
#define VALVE_8 PF7

#define ACTUATOR_1 PF4

#define FAN_1 PA8
#define FAN_2 PA9
#define FAN_3 PE9

#define PTC_FAN_6 PA11
#define PTC_FAN_5 PF5
#define PTC_FAN_4 PA7
#define PTC_FAN_3 PA6
#define PTC_FAN_2 PA5
#define PTC_FAN_1 PA4

#define LAMP_R PC0
#define LAMP_Y PC14
#define LAMP_G PC13
#define LAMP_AUTO_START PC3
#define LAMP_AUTO_STOP PC2
#define LAMP_FAUCET PB7
#define LAMP_EMERGENCY PC1
#define LAMP_UV PE0

#define SENSOR_TEMP_1 PB3
#define SENSOR_TEMP_2 PB4
#define SENSOR_TEMP_3 PD14

#define SENSOR_LEVEL_H_FILTER PA12
#define SENSOR_LEVEL_L_FILTER PA13
#define SENSOR_LEVEL_LL_FILTER PA15
#define SENSOR_LEVEL_L_EQ PB0
#define SENSOR_LEVEL_L_S2 PB1
#define SENSOR_LEVEL_L_REUSE PB2

#define SENSOR_CT_MOTOR_6 ADC0_CH5
#define SENSOR_CT_MOTOR_4 ADC0_CH6
#define SENSOR_CT_MOTOR_3 ADC0_CH7
#define SENSOR_CT_MOTOR_2 ADC0_CH8
#define SENSOR_CT_MOTOR_1 ADC0_CH9

#define SW_AUTO_START PD1
#define SW_AUTO_STOP PD3
#define SW_FAUCET PD4
#define SW_EMERGENCY PF2
#define SW_SPARE_1 PD2

#define SENSOR_ZERO_CROSSING PD0//INT3

#define LED_OPERATION PD8

#define HEATER_1 PB6

#define BUZZER PC12

#define RADIATOR PB5

#define FLOW_METER_1 PD9

#define BCD_TO_DEC(bcd) ((((bcd) & 0xff) >> 4) * 10 + (((bcd) & 0xff) & 0x0f))
#define DEC_TO_BCD(dec) (((dec) / 10 * 16) + ((dec) % 10))

#define OP(mask, value) ((uint16_t)(((uint16_t)(mask) << 8) | (uint8_t)(value)))
#define OP_MASK(op) ((uint8_t)((op) >> 8))
#define OP_VALUE(op) ((uint8_t)(op))

#define DEV_OPERATION(flag, op) ((flag) = ((uint8_t)(flag) & (uint8_t)~OP_MASK(op)) | OP_VALUE(op))

#define IS_LEAP_YEAR(y) (((y) % 400 == 0) || (((y) % 4 == 0) && ((y) % 100 != 0)))

typedef enum
{
	EINT135 = 0, // ZERO_CROSSING
	GPAB = 1,
	GPCDEF = 1,
	TIMER_1 = 2, // 1ms
	TIMER_0 = 3, // 1us
	TIMER_2 = 4, // 100ms
	TIMER_3 = 5, // 1s
	
} NVIC_Priority_e;

enum
{
    WP_Manual  = OP(1u << 0, 0u),
    WP_Auto    = OP(1u << 0, 1u << 0),

    SD_Manual  = OP(1u << 1, 0u),
    SD_Auto    = OP(1u << 1, 1u << 1),

    IM_Manual  = OP(1u << 2, 0u),
    IM_Auto    = OP(1u << 2, 1u << 2),

    ALL_Manual = OP((1u << 0) | (1u << 1) | (1u << 2), 0u),
    ALL_Auto   = OP((1u << 0) | (1u << 1) | (1u << 2),
                    (1u << 0) | (1u << 1) | (1u << 2)),
};

enum
{
	WP_Status = (1u << 0),
	SD_Status = (1u << 1),
	IM_Status = (1u << 2),
};

enum 
{
	WATER_PUMP_1_START_1 = 0x01,
	WATER_PUMP_1_START_2 = 0x02,
	WATER_PUMP_1_START_3 = 0x04,
	WATER_PUMP_2_START_1 = 0x10,
	WATER_PUMP_2_START_2 = 0x20,
	WATER_PUMP_2_START_3 = 0x40,
};

enum
{
	Off = 0,
	On = 1,
};

enum
{
	Manual = 0,
	Auto = 1,
};

enum
{
	Disable = 0,
	Enable = 1,
};

enum
{
	Stop = 0,
	CCW = 1,
	CW = 2,
};

enum
{
	On_Time = 1,
	Off_Time = 2,
};

enum 
{
	Dry = 1,
	Crush = 2,
	Out = 3,
};

enum 
{
	H_FILTER = 0,
	L_FILTER = 1,
	LL_FILTER = 2,
	L_REUSE = 3,
	L_EQ = 4,
	L_S2 = 5,
};

enum 
{
	TEMP_1 = 0,
	TEMP_2 = 1,
	TEMP_3 = 2,
	TEMP_4 = 3,
	ZERO_CROSSING = 4,
};

enum 
{
	AUTO_START = 0,
	AUTO_STOP = 1,
	FAUCET = 2,
	EMERGENCY = 3,
	SPARE_1 = 4,
};

enum 
{
	CT_MOTOR_1 = 0,
	CT_MOTOR_2 = 1,
	CT_MOTOR_3 = 2,
	CT_MOTOR_4 = 3,
	CT_MOTOR_5 = 4,
};

uint32_t getSysTick_ms(void);

#endif /* __USER_COMMON_H__ */