#include "user_common.h"

static uint8_t pump_3_schedule_hour_check = 0xff;
static uint8_t pump_3_last_hour = 0xff;
static uint8_t pump_3_power_on_hour = 0xff;
static uint32_t pump_3_day_counter = 1;
static bool pump_3_day_changed = false;

void pump_1_handler(void)
{
	if(level_sensor[LL_FILTER].state == Disable)
    {
        if(op.ID_PUMP_1 == On)
        {
            PUMP_1 = Off;
			
            op.ID_PUMP_1 = Off;
        }
		
		timer.ID_PUMP_1_ON_DELAY = 0;
		
        return;
    }
	
	if(op.ID_AERATION_3 == On)
	{
		if(sp.ID_PUMP_1_ON_DELAY > 0)
		{
			if(op.ID_PUMP_1 == Off)
			{
				if(++timer.ID_PUMP_1_ON_DELAY >= sp.ID_PUMP_1_ON_DELAY)
				{
					timer.ID_PUMP_1_ON_DELAY = 0;
					
					if(op.ID_VALVE_8 == On)
					{
						VALVE_8 = Off;
						
						op.ID_VALVE_8 = Off;
					}
					
					if(op.ID_VALVE_7 == Off)
					{
						VALVE_7 = On;
						
						op.ID_VALVE_7 = On;
					}
					
					PUMP_1 = On;
					
					op.ID_PUMP_1 = On;
				}
			}
		}
		else
		{
			if(op.ID_PUMP_1 == Off)
            {
				if(op.ID_VALVE_8 == On)
				{
					VALVE_8 = Off;
					
					op.ID_VALVE_8 = Off;
				}
				
				if(op.ID_VALVE_7 == Off)
				{
					VALVE_7 = On;
					
					op.ID_VALVE_7 = On;
				}
					
                PUMP_1 = On;
				
                op.ID_PUMP_1 = On;
            }
		}
	}
	else
	{
		timer.ID_PUMP_1_ON_DELAY = 0;
		
		if(op.ID_PUMP_1 == On)
		{
			PUMP_1 = Off;
			
			op.ID_PUMP_1 = Off;
		}
	}
}

void pump_2_handler(void)
{
	static bool heater_1_was_on = false;
	static uint8_t motor_2_prev_index = 0;
	
	if((motor_2_prev_index == Crush) && (op.ID_MOTOR_2_INDEX == Out))
	{
		if(op.ID_PUMP_2 == Off)
        {
			timer.ID_PUMP_2_OP_TIME = 0;
			
            PUMP_2 = On;
			
            op.ID_PUMP_2 = On;
        }
	}
	
	if(op.ID_HEATER_1 == On)
    {
        heater_1_was_on = true;
    }
	
	if((heater_1_was_on == true) && (op.ID_HEATER_1 == Off))
	{
		if(op.ID_PUMP_2 == On)
		{
			if(++timer.ID_PUMP_2_OP_TIME >= sp.ID_PUMP_2_OP_TIME)
			{
				timer.ID_PUMP_2_OP_TIME = 0;
				
				PUMP_2 = Off;
				
				op.ID_PUMP_2 = Off;
				
				heater_1_was_on = false;
			}
		}
	}
	
	motor_2_prev_index = op.ID_MOTOR_2_INDEX;
}

void pump_3_scheduler_init(void)
{
    pump_3_day_counter = 1;
    pump_3_last_hour = 0xff;
    pump_3_schedule_hour_check = 0xff;
    pump_3_day_changed = false;
    pump_3_power_on_hour = rtc.u32Hour;
	
	op.ID_PUMP_3 = Off;
	op.ID_PUMP_3_START_TIME_INDEX = 0;
}

void pump_3_scheduler(void)
{
	const uint32_t pump_3_schedule[] = {0,
    sp.ID_PUMP_3_START_TIME_1,
    sp.ID_PUMP_3_START_TIME_2,
    sp.ID_PUMP_3_START_TIME_3,
	sp.ID_PUMP_3_START_TIME_4,
	sp.ID_PUMP_3_START_TIME_5,
	};
	
	__disable_irq();
	uint8_t current_hour = rtc.u32Hour;
    uint8_t current_minute = rtc.u32Minute;
    uint8_t current_second = rtc.u32Second;
	__enable_irq();
	uint8_t current_hour_24 = (current_hour == 0) ? 24 : current_hour;
	
	uint32_t arr_size = sizeof(pump_3_schedule) / sizeof(uint32_t);
	
	if(sp.ID_PUMP_3_INTERVAL == 0)
		return;
	
	if((current_hour == 0) && (pump_3_last_hour == 23))
    {
        if(!pump_3_day_changed)
        {
            pump_3_day_counter++;
            pump_3_day_changed = true;
        }
    }
    else if(current_hour != 0)
    {
        pump_3_day_changed = false;
    }
	else{}
	
	if((current_minute == 0) && (current_second < 9))
    {
        if(pump_3_schedule_hour_check != current_hour)
        {
            pump_3_schedule_hour_check = current_hour;
			
            if((((pump_3_day_counter - 1) % sp.ID_PUMP_3_INTERVAL) == 0) && (op.ID_PUMP_3 == Off))
            {
				if(level_sensor[L_REUSE].state == Enable)
				{
					uint8_t power_on_hour_24 = (pump_3_day_counter == 1) ? ((pump_3_power_on_hour == 0) ? 24 : pump_3_power_on_hour) : 0;
					
					for(int i = 1; i < arr_size; i++)
					{
						if(pump_3_schedule[i] == 0)
						{
							continue;
						}
						
						if(current_hour_24 == pump_3_schedule[i])
						{
							if(pump_3_day_counter == 1)
							{
								if(pump_3_schedule[i] <= power_on_hour_24)
								{
									continue;
								}
							}
							
							op.ID_PUMP_3_START_TIME_INDEX = i;
						}
					}
				}
            }
        }
    }
	else if(current_minute != 0)
    {
        pump_3_schedule_hour_check = 0xff;
    }
	else{}
	
    pump_3_last_hour = current_hour;
}

void pump_3_handler(void)
{
	pump_3_scheduler();
	
	if(level_sensor[L_REUSE].state == Disable)
    {
        if(op.ID_PUMP_3 == On)
        {
            timer.ID_PUMP_3_ON_TIME = 0;
            
            PUMP_3 = Off;
            
            op.ID_PUMP_3 = Off;
            op.ID_PUMP_3_START_TIME_INDEX = 0;
        }
        
        return;
    }
	
	if((op.ID_PUMP_3_START_TIME_INDEX != 0) && (sp.ID_PUMP_3_ON_TIME > 0))
    {
		if(op.ID_PUMP_3 == Off)
        {
			timer.ID_PUMP_3_ON_TIME = 0;
			
			if(op.ID_VALVE_7 == On)
			{
				VALVE_7 = Off;
				
				op.ID_VALVE_7 = Off;
			}
			
			if(op.ID_VALVE_8 == Off)
			{
				VALVE_8 = On;
				
				op.ID_VALVE_8 = On;
			}
			
            PUMP_3 = On;
			
            op.ID_PUMP_3 = On;
        }
		else
        {
			if(++timer.ID_PUMP_3_ON_TIME >= sp.ID_PUMP_3_ON_TIME)
			{
				timer.ID_PUMP_3_ON_TIME = 0;
				
				PUMP_3 = Off;
				
				op.ID_PUMP_3 = Off;
				op.ID_PUMP_3_START_TIME_INDEX = 0;
			}
		}
	}
	else if(op.ID_PUMP_3_START_TIME_INDEX == 0)
    {
		if(op.ID_PUMP_3 == On)
		{
			timer.ID_PUMP_3_ON_TIME = 0;
			
			PUMP_3 = Off;
			
            op.ID_PUMP_3 = Off;
		}
	}
	else
	{
		op.ID_PUMP_3_START_TIME_INDEX = 0;

		if(op.ID_PUMP_3 == On)
		{
			timer.ID_PUMP_3_ON_TIME = 0;
			
			PUMP_3 = Off;
			
			op.ID_PUMP_3 = Off;
		}
	}
}
