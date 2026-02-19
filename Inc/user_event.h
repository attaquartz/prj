#ifndef __USER_EVENT_H__
#define __USER_EVENT_H__

#include "user_common.h"

#define EVENT_QUEUE_SIZE 512

typedef enum
{
    STATUS_IDLE = 0,
	STATUS_AUTO,
	STATUS_ZERO_CROSSING,
	STATUS_LEVEL_SENSOR,
	STATUS_ALERT_SENSOR,
	STATUS_USER_SWITCH,
	STATUS_CT_SENSOR,
	STATUS_MOTOR_7,
	STATUS_MODBUS_SERVER_UPDATE,
	STATUS_MODBUS_CLIENT,
	STATUS_MODBUS_CLIENT_ERROR,
	STATUS_FLASH_RECORD,
	STATUS_EEPROM,
	
	END_OF_EVENT_STATUS
} EventStatus;

typedef struct
{
	int32_t status;
    int32_t index;
} Event;

typedef struct
{
    Event buffer[EVENT_QUEUE_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
} EventQueue;

void Event_Init(void);
void Event_Handler(void);
void run_event(int32_t status, int32_t index);

#endif