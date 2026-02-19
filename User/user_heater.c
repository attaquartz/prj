#include "user_common.h"

void heater_1_handler(void)
{
	static uint8_t motor_2_prev_index = 0;
	
	if((motor_2_prev_index == Crush) && (op.ID_MOTOR_2_INDEX == Out))
	{
		if(op.ID_HEATER_1 == Off)
        {
			timer.ID_HEATER_1_OP_TIME = 0;
			timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
			
            HEATER_1 = On;
			
            op.ID_HEATER_1 = On;
        }
	}
	
	if((alert_sensor[TEMP_3].state == Disable) && (op.ID_HEATER_1 == Off))
    {
		timer.ID_HEATER_1_OP_TIME = 0;
        timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
		
        HEATER_1 = On;
		
        op.ID_HEATER_1 = On;
    }
	
	if(op.ID_HEATER_1 == On)
	{
		if(alert_sensor[TEMP_2].state == Enable)
		{
			timer.ID_HEATER_1_OP_TIME = 0;
			timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
			
			HEATER_1 = Off;
			
			op.ID_HEATER_1 = Off;
		}
		else
		{
			if(++timer.ID_HEATER_1_OP_TIME >= sp.ID_HEATER_1_OP_TIME)
			{
				timer.ID_HEATER_1_OP_TIME = 0;
				timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
				
				HEATER_1 = Off;
				
				op.ID_HEATER_1 = Off;
			}
		}
	}
	
	motor_2_prev_index = op.ID_MOTOR_2_INDEX;
}

void radiator_handler(void)
{
	int8_t temperature_in = op.ID_ENVIRONMENT_IN >> 8;
	
	if(op.ID_RADIATOR == Off)
	{
		if(temperature_in < sp.ID_RADIATOR_ON_TEMP)
		{
			if(++timer.ID_RADIATOR_DELAY_TIME >= sp.ID_HYSTERESIS_TIME)
			{
				timer.ID_RADIATOR_DELAY_TIME = 0;
				
				RADIATOR = On;
				
				op.ID_RADIATOR = On;
			}
		}
		else
		{
			timer.ID_RADIATOR_DELAY_TIME = 0;
		}
	}
	else
	{
		if(temperature_in > sp.ID_RADIATOR_OFF_TEMP)
		{
			if(++timer.ID_RADIATOR_DELAY_TIME >= sp.ID_HYSTERESIS_TIME)
			{
				timer.ID_RADIATOR_DELAY_TIME = 0;
				
				RADIATOR = Off;
				
				op.ID_RADIATOR = Off;
			}
		}
		else
		{
			timer.ID_RADIATOR_DELAY_TIME = 0;
		}
	}
}
