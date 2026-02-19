#include "user_common.h"

static uint8_t valve_1_schedule_hour_check = 0xff;
static uint8_t valve_1_last_hour = 0xff;
static uint8_t valve_1_power_on_hour = 0xff;
static uint32_t valve_1_day_counter = 1;
static bool valve_1_day_changed = false;

void valve_1_scheduler_init(void)
{
    valve_1_day_counter = 1;
    valve_1_last_hour = 0xff;
    valve_1_schedule_hour_check = 0xff;
    valve_1_day_changed = false;
    valve_1_power_on_hour = rtc.u32Hour;
	
	op.ID_VALVE_1 = Off;
	op.ID_VALVE_1_START_TIME_INDEX = 0;
}

void valve_1_scheduler(void)
{
	const uint32_t valve_1_schedule[] = {0,
    sp.ID_VALVE_1_START_TIME_1,
    sp.ID_VALVE_1_START_TIME_2,
    sp.ID_VALVE_1_START_TIME_3,
    sp.ID_VALVE_1_START_TIME_4,
    sp.ID_VALVE_1_START_TIME_5,
    sp.ID_VALVE_1_START_TIME_6,
	sp.ID_VALVE_1_START_TIME_7,
	sp.ID_VALVE_1_START_TIME_8,
	sp.ID_VALVE_1_START_TIME_9,
	sp.ID_VALVE_1_START_TIME_10,
	};
	
	uint8_t current_hour = rtc.u32Hour;
    uint8_t current_minute = rtc.u32Minute;
    uint8_t current_second = rtc.u32Second;
	uint8_t current_hour_24 = (current_hour == 0) ? 24 : current_hour;
	
	uint32_t arr_size = sizeof(valve_1_schedule) / sizeof(uint32_t);
	
	if(sp.ID_VALVE_1_INTERVAL == 0)
		return;
	
	if((current_hour == 0) && (valve_1_last_hour == 23))
    {
		if(!valve_1_day_changed)
        {
            valve_1_day_counter++;
            valve_1_day_changed = true;
        }
    }
    else if(current_hour != 0)
    {
        valve_1_day_changed = false;
    }
	else{}
	
	if((current_minute == 0) && (current_second < 9))
    {
        if(valve_1_schedule_hour_check != current_hour)
        {
            valve_1_schedule_hour_check = current_hour;
			
            if((((valve_1_day_counter - 1) % sp.ID_VALVE_1_INTERVAL) == 0) && (op.ID_VALVE_1 == Off))
            {
				uint8_t power_on_hour_24 = (valve_1_day_counter == 1) ? ((valve_1_power_on_hour == 0) ? 24 : valve_1_power_on_hour) : 0;
				
                for(int i = 1; i < arr_size; i++)
                {
                    if(valve_1_schedule[i] == 0)
                    {
                        continue;
                    }
                    
                    if(current_hour_24 == valve_1_schedule[i])
                    {
						if(valve_1_day_counter == 1)
                        {
                            if(valve_1_schedule[i] <= power_on_hour_24)
                            {
                                continue;
                            }
                        }
						
						op.ID_VALVE_1_START_TIME_INDEX = i;
                    }
                }
            }
        }
    }
	else if(current_minute != 0)
    {
        valve_1_schedule_hour_check = 0xff;
    }
	else{}
	
    valve_1_last_hour = current_hour;
}

void valve_1_handler(void)
{
	valve_1_scheduler();
	
	if((op.ID_VALVE_1_START_TIME_INDEX != 0) && (sp.ID_VALVE_1_ON_TIME > 0))
    {
		if(op.ID_VALVE_1 == Off)
        {
			timer.ID_VALVE_1_ON_TIME = 0;
			
            VALVE_1 = On;
			
            op.ID_VALVE_1 = On;
        }
		else
        {
			if(++timer.ID_VALVE_1_ON_TIME >= sp.ID_VALVE_1_ON_TIME)
			{
				timer.ID_VALVE_1_ON_TIME = 0;
				
				VALVE_1 = Off;
				
				op.ID_VALVE_1 = Off;
				op.ID_VALVE_1_START_TIME_INDEX = 0;
			}
		}
	}
	else if(op.ID_VALVE_1_START_TIME_INDEX == 0)
    {
		if(op.ID_VALVE_1 == On)
		{
			timer.ID_VALVE_1_ON_TIME = 0;
			
			VALVE_1 = Off;
			
            op.ID_VALVE_1 = Off;
		}
	}
	else
	{
		op.ID_VALVE_1_START_TIME_INDEX = 0;

		if(op.ID_VALVE_1 == On)
		{
			VALVE_1 = Off;
			
			op.ID_VALVE_1 = Off;
		}
	}
}

void valve_2_scheduler(void)
{
    static bool water_pump_1_was_on = false;
    static bool water_pump_2_was_on = false;
    
    if((op.ID_WATER_PUMP_1 == On) && (op.ID_WATER_PUMP_2 == On))
    {
        water_pump_1_was_on = true;
        water_pump_2_was_on = true;
        
        if(op.ID_WATER_PUMP_1_START_TIME_INDEX != 0)
        {
            op.ID_VALVE_2_OP_INDEX = op.ID_WATER_PUMP_1_START_TIME_INDEX;
        }
        else if(op.ID_WATER_PUMP_2_START_TIME_INDEX != 0)
        {
            op.ID_VALVE_2_OP_INDEX = op.ID_WATER_PUMP_2_START_TIME_INDEX;
        }
		else{}
    }
    
    if((water_pump_1_was_on == true) && (water_pump_2_was_on == true) && (op.ID_WATER_PUMP_1 == Off) && (op.ID_WATER_PUMP_2 == Off))
    {
        if(op.ID_VALVE_2 == Off)
        {
            if(op.ID_VALVE_2_OP_INDEX != 0)
            {
                water_pump_1_was_on = false;
                water_pump_2_was_on = false;
            }
        }
    }
}

void valve_2_handler(void)
{
    const uint32_t valve_2_on_delay[] = {0,
        sp.ID_VALVE_2_ON_DELAY_1,
        sp.ID_VALVE_2_ON_DELAY_2,
        sp.ID_VALVE_2_ON_DELAY_3,
        sp.ID_VALVE_2_ON_DELAY_4,
        sp.ID_VALVE_2_ON_DELAY_5,
        sp.ID_VALVE_2_ON_DELAY_6,
        sp.ID_VALVE_2_ON_DELAY_7,
        sp.ID_VALVE_2_ON_DELAY_8,
        sp.ID_VALVE_2_ON_DELAY_9,
        sp.ID_VALVE_2_ON_DELAY_10,
    };
	
	valve_2_scheduler();
    
    if(op.ID_VALVE_2_OP_INDEX != 0)
    {
        if(op.ID_VALVE_2 == Off)
        {
			if((op.ID_WATER_PUMP_1 == Off) && (op.ID_WATER_PUMP_2 == Off))
			{
				if(valve_2_on_delay[op.ID_VALVE_2_OP_INDEX] > 0)
				{
					if(++timer.ID_VALVE_2_ON_DELAY >= valve_2_on_delay[op.ID_VALVE_2_OP_INDEX])
					{
						timer.ID_VALVE_2_ON_DELAY = 0;
						timer.ID_VALVE_2_ON_TIME = 0;
						
						VALVE_2 = On;
						
						op.ID_VALVE_2 = On;
					}
				}
				else
				{
					timer.ID_VALVE_2_ON_TIME = 0;
					
					VALVE_2 = On;
					
					op.ID_VALVE_2 = On;
					
				}
			}
			else
			{
				timer.ID_VALVE_2_ON_DELAY = 0;
			}
        }
        else
        {
            if(sp.ID_VALVE_2_ON_TIME > 0)
            {
                if(++timer.ID_VALVE_2_ON_TIME >= sp.ID_VALVE_2_ON_TIME)
                {
                    timer.ID_VALVE_2_ON_TIME = 0;
                    
                    VALVE_2 = Off;
					
                    op.ID_VALVE_2 = Off;
                    op.ID_VALVE_2_OP_INDEX = 0;
                }
            }
            else
            {
                VALVE_2 = Off;
				
                op.ID_VALVE_2 = Off;
                op.ID_VALVE_2_OP_INDEX = 0;
            }
        }
    }
    else
    {
        if(op.ID_VALVE_2 == On)
        {
            VALVE_2 = Off;
			
            op.ID_VALVE_2 = Off;
        }
    }
}

void valve_3_handler(void)
{
	if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_3_ON_TIME)
	{
		if(sp.ID_VALVE_3_ON_TIME == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_3_OFF_DELAY;
			
			return;
		}
		
		if(VALVE_3 == Off)
		{
			VALVE_3 = On;
		}
		
		op.ID_VALVE_3 = On;
		
		if(++timer.ID_VALVE_3_ON_TIME >= sp.ID_VALVE_3_ON_TIME)
		{
			timer.ID_VALVE_3_ON_TIME = 0;
			
			VALVE_3 = Off;
			
			op.ID_VALVE_3 = Off;
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_3_OFF_DELAY;
		}
	}
	else if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_3_OFF_DELAY)
	{
		if(sp.ID_VALVE_3_OFF_DELAY == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_4_ON_TIME;
			
			return;
		}
		
		if(VALVE_3 == On)
		{
			VALVE_3 = Off;
		}
		
		op.ID_VALVE_3 = Off;
		
		if(++timer.ID_VALVE_3_OFF_DELAY >= sp.ID_VALVE_3_OFF_DELAY)
		{
			timer.ID_VALVE_3_OFF_DELAY = 0;
			
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_4_ON_TIME;
		}
	}
	else{}
}

void valve_4_handler(void)
{
	if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_4_ON_TIME)
	{
		if(sp.ID_VALVE_4_ON_TIME == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_4_OFF_DELAY;
			
			return;
		}
		
		if(VALVE_4 == Off)
		{
			VALVE_4 = On;
		}
		
		op.ID_VALVE_4 = On;
		
		if(++timer.ID_VALVE_4_ON_TIME >= sp.ID_VALVE_4_ON_TIME)
		{
			timer.ID_VALVE_4_ON_TIME = 0;
			
			VALVE_4 = Off;
			
			op.ID_VALVE_4 = Off;
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_4_OFF_DELAY;
		}
	}
	else if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_4_OFF_DELAY)
	{
		if(sp.ID_VALVE_4_OFF_DELAY == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_5_ON_TIME;
			
			return;
		}
		
		if(VALVE_4 == On)
		{
			VALVE_4 = Off;
		}
		
		op.ID_VALVE_4 = Off;
		
		if(++timer.ID_VALVE_4_OFF_DELAY >= sp.ID_VALVE_4_OFF_DELAY)
		{
			timer.ID_VALVE_4_OFF_DELAY = 0;
			
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_5_ON_TIME;
		}
	}
	else{}
}

void valve_5_handler(void)
{
	if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_5_ON_TIME)
	{
		if(sp.ID_VALVE_5_ON_TIME == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_5_OFF_DELAY;
			
			return;
		}
		
		if(VALVE_5 == Off)
		{
			VALVE_5 = On;
		}
		
		op.ID_VALVE_5 = On;
		
		if(++timer.ID_VALVE_5_ON_TIME >= sp.ID_VALVE_5_ON_TIME)
		{
			timer.ID_VALVE_5_ON_TIME = 0;
			
			VALVE_5 = Off;
			
			op.ID_VALVE_5 = Off;
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_5_OFF_DELAY;
		}
	}
	else if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_5_OFF_DELAY)
	{
		if(sp.ID_VALVE_5_OFF_DELAY == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_6_ON_TIME;
			
			return;
		}
		
		if(VALVE_5 == On)
		{
			VALVE_5 = Off;
		}
		
		op.ID_VALVE_5 = Off;
		
		if(++timer.ID_VALVE_5_OFF_DELAY >= sp.ID_VALVE_5_OFF_DELAY)
		{
			timer.ID_VALVE_5_OFF_DELAY = 0;
			
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_6_ON_TIME;
		}
	}
	else{}
}

void valve_6_handler(void)
{
	if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_6_ON_TIME)
	{
		if(sp.ID_VALVE_6_ON_TIME == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_6_OFF_DELAY;
			
			return;
		}
		
		if(VALVE_6 == Off)
		{
			VALVE_6 = On;
		}
		
		op.ID_VALVE_6 = On;
		
		if(++timer.ID_VALVE_6_ON_TIME >= sp.ID_VALVE_6_ON_TIME)
		{
			timer.ID_VALVE_6_ON_TIME = 0;
			
			VALVE_6 = Off;
			
			op.ID_VALVE_6 = Off;
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_6_OFF_DELAY;
		}
	}
	else if(op.ID_VALVE_AIR_LIFTER_INDEX == ID_VALVE_6_OFF_DELAY)
	{
		if(sp.ID_VALVE_6_OFF_DELAY == 0)
		{
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_3_ON_TIME;
			
			return;
		}
		
		if(VALVE_6 == On)
		{
			VALVE_6 = Off;
		}
		
		op.ID_VALVE_6 = Off;
		
		if(++timer.ID_VALVE_6_OFF_DELAY >= sp.ID_VALVE_6_OFF_DELAY)
		{
			timer.ID_VALVE_6_OFF_DELAY = 0;
			
			op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_3_ON_TIME;
		}
	}
	else{}
}

void valve_7_handler(void)
{
	if(op.ID_VALVE_7 == Off)
	{
		if((op.ID_VALVE_8 == Off) && (op.ID_PUMP_1 == On))
		{
			VALVE_7 = On;
			
			op.ID_VALVE_7 = On;
		}
	}
	else
	{
		if((op.ID_VALVE_8 == On) || (op.ID_PUMP_1 == Off))
		{
			VALVE_7 = Off;
			
			op.ID_VALVE_7 = Off;
		}
	}
}

void valve_8_handler(void)
{
	if(op.ID_VALVE_8 == Off)
	{
		if((op.ID_VALVE_7 == Off) && (op.ID_PUMP_3 == On))
		{
			VALVE_8 = On;
			
			op.ID_VALVE_8 = On;
		}
	}
	else
	{
		if((op.ID_VALVE_7 == On) || (op.ID_PUMP_3 == Off)) 
		{
			VALVE_8 = Off;
			
			op.ID_VALVE_8 = Off;
		}
	}
}
