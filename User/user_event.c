#include "user_common.h"

EventQueue g_event_queue;

void EventQueue_Init(EventQueue *queue);
bool EventQueue_Enqueue(EventQueue *queue, int32_t status, int32_t index);
bool EventQueue_Dequeue(EventQueue *queue, Event *event);
bool EventQueue_IsEmpty(const EventQueue *queue);
bool EventQueue_IsFull(const EventQueue *queue);
uint32_t EventQueue_Count(const EventQueue *queue);

void event_handler(int32_t status, int32_t index);

void EventQueue_Init(EventQueue *queue)
{
    queue->head = 0;
    queue->tail = 0;
}

bool EventQueue_IsEmpty(const EventQueue *queue)
{
    return queue->head == queue->tail;
}

bool EventQueue_IsFull(const EventQueue *queue)
{
    return ((queue->tail + 1) % EVENT_QUEUE_SIZE) == queue->head;
}

uint32_t EventQueue_Count(const EventQueue *queue)
{
    if (queue->tail >= queue->head)
        return queue->tail - queue->head;
    else
        return EVENT_QUEUE_SIZE - queue->head + queue->tail;
}

bool EventQueue_Enqueue(EventQueue *queue, int32_t status, int32_t index)
{
    uint32_t next_tail;
	
    __disable_irq();
    
    next_tail = (queue->tail + 1) % EVENT_QUEUE_SIZE;
	
	if (next_tail == queue->head)
	{
		__enable_irq();
		
		return false;
	}
    
    queue->buffer[queue->tail].status = status;
    queue->buffer[queue->tail].index = index;
	
    queue->tail = next_tail;
    
    __enable_irq();
	
    return true;
}

bool EventQueue_Dequeue(EventQueue *queue, Event *event)
{
	__disable_irq();
	
    if (EventQueue_IsEmpty(queue))
	{
		__enable_irq();
        return false;
    }
    
    event->status = queue->buffer[queue->head].status;
    event->index = queue->buffer[queue->head].index;
    
    queue->head = (queue->head + 1) % EVENT_QUEUE_SIZE;
    
    __enable_irq();
	
    return true;
}

void Event_Init(void)
{
    EventQueue_Init(&g_event_queue);
}

void Event_Handler(void)
{
	Event event;
    
    while (EventQueue_Dequeue(&g_event_queue, &event))
	{
        event_handler(event.status, event.index);
    }
}

void run_event(int32_t status, int32_t index)
{
	if(!EventQueue_Enqueue(&g_event_queue, status, index))
	{
		// error
	}
}

void event_handler(int32_t status, int32_t index)
{
	LED_OPERATION ^= On;
	
	switch(status)
	{
		case STATUS_IDLE:
		{
			
		}
		break;
		
		case STATUS_AUTO:
		{
			auto_handler();
		}
		break;
		
		case STATUS_ZERO_CROSSING:
		{
			zero_crossing_handler();
		}
		break;
		
		case STATUS_LEVEL_SENSOR:
		{
			level_sensor_handler(index);
		}
		break;
		
		case STATUS_ALERT_SENSOR:
		{
			alert_sensor_handler(index);
		}
		break;
		
		case STATUS_USER_SWITCH:
		{
			user_switch_handler(index);
		}
		break;
		
		case STATUS_CT_SENSOR:
		{
			ct_sensor_handler();
		}
		break;
		
		case STATUS_MOTOR_7:
		{
			motor_7_set(index);
		}
		break;
		
		case STATUS_MODBUS_SERVER_UPDATE:
		{
			modbus_server_update_handler();
		}
		break;
		
		case STATUS_MODBUS_CLIENT:
		{
			uint32_t id = 0;
			
			if((index >= 1) && (index < 3))
			{
				id = index - 1;
				environment_sensor_handler(id);
			}
			else if(index == 3)
			{
				id = 0;
				power_meter_handler(id);
			}
			else{}
		}
		break;
		
		case STATUS_MODBUS_CLIENT_ERROR:
		{
			uint32_t id = 0;
			
			if((index >= 1) && (index < 3))
			{
				id = index - 1;
				environment_sensor_error(id);
			}
			else if(index == 3)
			{
				id = 0;
				power_meter_error(id);
			}
			else{}
		}
		break;
		
		case STATUS_FLASH_RECORD:
		{
			flash_record();
		}
		break;
		
		case STATUS_EEPROM:
		{
			eeprom_Write();
		}
		break;
		
		default: break;
	}
}
