#include "user_common.h"

static uint8_t water_pump_1_schedule_hour_check = 0xff;
static uint8_t water_pump_1_last_hour = 0xff;
static uint8_t water_pump_1_power_on_hour = 0xff;
static uint32_t water_pump_1_day_counter = 1;
static bool water_pump_1_day_changed = false;

static uint8_t water_pump_2_schedule_hour_check = 0xff;
static uint8_t water_pump_2_last_hour = 0xff;
static uint8_t water_pump_2_power_on_hour = 0xff;
static uint32_t water_pump_2_day_counter = 1;
static bool water_pump_2_day_changed = false;

static uint8_t water_pump_5_schedule_hour_check = 0xff;
static uint8_t water_pump_5_last_hour = 0xff;
static uint8_t water_pump_5_power_on_hour = 0xff;
static uint32_t water_pump_5_day_counter = 1;
static bool water_pump_5_day_changed = false;

void water_pump_1_scheduler_init(void)
{
    water_pump_1_day_counter = 1;
    water_pump_1_last_hour = 0xff;
    water_pump_1_schedule_hour_check = 0xff;
    water_pump_1_day_changed = false;
    water_pump_1_power_on_hour = rtc.u32Hour;
	
	op.ID_WATER_PUMP_1 = Off;
	op.ID_WATER_PUMP_1_START_TIME_INDEX = 0;
}

void water_pump_1_scheduler(void)
{
	const uint32_t water_pump_1_schedule[] = {0,
    sp.ID_SLUDGE_DISCHARGE_START_TIME_1,
    sp.ID_SLUDGE_DISCHARGE_START_TIME_2,
    sp.ID_SLUDGE_DISCHARGE_START_TIME_3,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_4,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_5,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_6,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_7,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_8,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_9,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_10,
	};
	
	uint8_t current_hour = rtc.u32Hour;
    uint8_t current_minute = rtc.u32Minute;
    uint8_t current_second = rtc.u32Second;
	uint8_t current_hour_24 = (current_hour == 0) ? 24 : current_hour;
	
	uint32_t arr_size = sizeof(water_pump_1_schedule) / sizeof(uint32_t);
	
	if(sp.ID_SLUDGE_DISCHARGE_INTERVAL == 0)
		return;
	
	if((current_hour == 0) && (water_pump_1_last_hour == 23))
    {
        if(!water_pump_1_day_changed)
        {
            water_pump_1_day_counter++;
            water_pump_1_day_changed = true;
        }
    }
    else if(current_hour != 0)
    {
        water_pump_1_day_changed = false;
    }
	else{}
	
	if((current_minute == 0) && (current_second < 9))
    {
        if(water_pump_1_schedule_hour_check != current_hour)
        {
            water_pump_1_schedule_hour_check = current_hour;
			
            if((((water_pump_1_day_counter - 1) % sp.ID_SLUDGE_DISCHARGE_INTERVAL) == 0) && (op.ID_WATER_PUMP_1 == Off))
            {
				if(op.ID_MOTOR_8 == Off)
				{
					uint8_t power_on_hour_24 = (water_pump_1_day_counter == 1) ? ((water_pump_1_power_on_hour == 0) ? 24 : water_pump_1_power_on_hour) : 0;
					
					for(int i = 1; i < arr_size; i++)
					{
						if(water_pump_1_schedule[i] == 0)
						{
							continue;
						}
						
						if(current_hour_24 == water_pump_1_schedule[i])
						{
							if(water_pump_1_day_counter == 1)
							{
								if(water_pump_1_schedule[i] <= power_on_hour_24)
								{
									continue;
								}
							}
							
							op.ID_WATER_PUMP_1_START_TIME_INDEX = i;
						}
					}
				}
            }
        }
    }
	else if(current_minute != 0)
    {
        water_pump_1_schedule_hour_check = 0xff;
    }
	else{}
	
    water_pump_1_last_hour = current_hour;
}

void water_pump_1_handler(void)
{
	water_pump_1_scheduler();
	
	if((op.ID_WATER_PUMP_1_START_TIME_INDEX != 0) && (sp.ID_WATER_PUMP_1_ON_TIME > 0))
    {
		if(op.ID_WATER_PUMP_1 == Off)
        {
			timer.ID_WATER_PUMP_1_ON_TIME = 0;
			
            WATER_PUMP_1 = On;
			
            op.ID_WATER_PUMP_1 = On;
        }
		else
        {
			if(++timer.ID_WATER_PUMP_1_ON_TIME >= sp.ID_WATER_PUMP_1_ON_TIME)
			{
				timer.ID_WATER_PUMP_1_ON_TIME = 0;
				
				WATER_PUMP_1 = Off;
				
				op.ID_WATER_PUMP_1 = Off;
				op.ID_WATER_PUMP_1_START_TIME_INDEX = 0;
				op.ID_MOTOR_8_OFF_TIME_INDEX = 0;
			}
		}
	}
	else if(op.ID_WATER_PUMP_1_START_TIME_INDEX == 0)
    {
		if(op.ID_WATER_PUMP_1 == On)
		{
			timer.ID_WATER_PUMP_1_ON_TIME = 0;
			
			WATER_PUMP_1 = Off;
			
            op.ID_WATER_PUMP_1 = Off;
		}
	}
	else
	{
		op.ID_WATER_PUMP_1_START_TIME_INDEX = 0;
		op.ID_MOTOR_8_OFF_TIME_INDEX = 0; 
		
		if(op.ID_WATER_PUMP_1 == On)
		{
			WATER_PUMP_1 = Off;
			
			op.ID_WATER_PUMP_1 = Off;
		}
	}
}

void water_pump_2_scheduler_init(void)
{
    water_pump_2_day_counter = 1;
    water_pump_2_last_hour = 0xff;
    water_pump_2_schedule_hour_check = 0xff;
    water_pump_2_day_changed = false;
    water_pump_2_power_on_hour = rtc.u32Hour;
	
	op.ID_WATER_PUMP_2 = Off;
	op.ID_WATER_PUMP_2_START_TIME_INDEX = 0;
}

void water_pump_2_scheduler(void)
{
	const uint32_t water_pump_2_schedule[] = {0,
    sp.ID_SLUDGE_DISCHARGE_START_TIME_1,
    sp.ID_SLUDGE_DISCHARGE_START_TIME_2,
    sp.ID_SLUDGE_DISCHARGE_START_TIME_3,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_4,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_5,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_6,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_7,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_8,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_9,
	sp.ID_SLUDGE_DISCHARGE_START_TIME_10,
	};
	
	uint8_t current_hour = rtc.u32Hour;
    uint8_t current_minute = rtc.u32Minute;
    uint8_t current_second = rtc.u32Second;
	uint8_t current_hour_24 = (current_hour == 0) ? 24 : current_hour;
	
	uint32_t arr_size = sizeof(water_pump_2_schedule) / sizeof(uint32_t);
	
	if(sp.ID_SLUDGE_DISCHARGE_INTERVAL == 0)
		return;
	
	if((current_hour == 0) && (water_pump_2_last_hour == 23))
    {
        if(!water_pump_2_day_changed)
        {
            water_pump_2_day_counter++;
            water_pump_2_day_changed = true;
        }
    }
    else if(current_hour != 0)
    {
        water_pump_2_day_changed = false;
    }
	else{}
	
	if((current_minute == 0) && (current_second < 9))
    {
        if(water_pump_2_schedule_hour_check != current_hour)
        {
            water_pump_2_schedule_hour_check = current_hour;
			
            if((((water_pump_2_day_counter - 1) % sp.ID_SLUDGE_DISCHARGE_INTERVAL) == 0) && (op.ID_WATER_PUMP_2 == Off))
            {
				if(op.ID_MOTOR_8 == Off)
				{
					uint8_t power_on_hour_24 = (water_pump_2_day_counter == 1) ? ((water_pump_2_power_on_hour == 0) ? 24 : water_pump_2_power_on_hour) : 0;
					
					for(int i = 1; i < arr_size; i++)
					{
						if(water_pump_2_schedule[i] == 0)
						{
							continue;
						}
						
						if(current_hour_24 == water_pump_2_schedule[i])
						{
							if(water_pump_2_day_counter == 1)
							{
								if(water_pump_2_schedule[i] <= power_on_hour_24)
								{
									continue;
								}
							}
							
							op.ID_WATER_PUMP_2_START_TIME_INDEX = i;
						}
					}
				}
            }
        }
    }
	else if(current_minute != 0)
    {
        water_pump_2_schedule_hour_check = 0xff;
    }
	else{}
	
    water_pump_2_last_hour = current_hour;
}

void water_pump_2_handler(void)
{
	water_pump_2_scheduler();
	
	if((op.ID_WATER_PUMP_2_START_TIME_INDEX != 0) && (sp.ID_WATER_PUMP_2_ON_TIME > 0))
    {
		if(op.ID_WATER_PUMP_2 == Off)
        {
			timer.ID_WATER_PUMP_2_ON_TIME = 0;
			
            WATER_PUMP_2 = On;
			
            op.ID_WATER_PUMP_2 = On;
        }
		else
        {
			if(++timer.ID_WATER_PUMP_2_ON_TIME >= sp.ID_WATER_PUMP_2_ON_TIME)
			{
				timer.ID_WATER_PUMP_2_ON_TIME = 0;
				
				WATER_PUMP_2 = Off;
				
				op.ID_WATER_PUMP_2 = Off;
				op.ID_WATER_PUMP_2_START_TIME_INDEX = 0;
				op.ID_MOTOR_8_OFF_TIME_INDEX = 0;
			}
		}
	}
	else if(op.ID_WATER_PUMP_2_START_TIME_INDEX == 0)
    {
		if(op.ID_WATER_PUMP_2 == On)
		{
			timer.ID_WATER_PUMP_2_ON_TIME = 0;
			
			WATER_PUMP_2 = Off;
			
            op.ID_WATER_PUMP_2 = Off;
		}
	}
	else
	{
		op.ID_WATER_PUMP_2_START_TIME_INDEX = 0;
		op.ID_MOTOR_8_OFF_TIME_INDEX = 0; 
		
		if(op.ID_WATER_PUMP_2 == On)
		{
			WATER_PUMP_2 = Off;
			
			op.ID_WATER_PUMP_2 = Off;
		}
	}
}

void water_pump_3_handler(void)
{
	if(op.ID_WATER_PUMP_3 == Off)
	{
		if((level_sensor[L_FILTER].state == Disable) && (level_sensor[H_FILTER].state == Disable) && (level_sensor[L_S2].state == Enable) && (op.ID_VALVE_1 == Off))
		{
			WATER_PUMP_3 = On;

			op.ID_WATER_PUMP_3 = On;
		}
	}
	else
	{
		if((level_sensor[H_FILTER].state == Enable) || (op.ID_VALVE_1 == On))
		{
			WATER_PUMP_3 = Off;
			
			op.ID_WATER_PUMP_3 = Off;
		}
	}
}

void water_pump_4_handler(void)
{
	bool off_condition = false;
	
	if(op.ID_WATER_PUMP_4 == On)
	{
		if((user_switch[FAUCET].state == Off) || (level_sensor[L_REUSE].state == Disable))
		{
			off_condition = true;
		}
		else
		{
			if(++timer.ID_WATER_PUMP_4_TIMEOUT >= sp.ID_WATER_PUMP_4_TIMEOUT)
			{
				off_condition = true;
			}
		}
	}
	else
	{
		if((user_switch[FAUCET].state == On) && (level_sensor[L_REUSE].state == Enable))
		{
			timer.ID_WATER_PUMP_4_TIMEOUT = 0;
			
			LAMP_FAUCET = On;
            WATER_PUMP_4 = On;
            
            op.ID_WATER_PUMP_4 = On;
            op.ID_SWITCH |= BIT2;
		}
	}
	
	if(off_condition == true)
	{
		timer.ID_WATER_PUMP_4_TIMEOUT = 0;
		
		LAMP_FAUCET = Off;

		WATER_PUMP_4 = Off;
	
		op.ID_WATER_PUMP_4 = Off;
		op.ID_SWITCH &= ~BIT2;
	}
}

void water_pump_5_scheduler_init(void)
{
    water_pump_5_day_counter = 1;
    water_pump_5_last_hour = 0xff;
    water_pump_5_schedule_hour_check = 0xff;
    water_pump_5_day_changed = false;
    water_pump_5_power_on_hour = rtc.u32Hour;
	
	op.ID_WATER_PUMP_5 = Off;
	op.ID_WATER_PUMP_5_START_TIME_INDEX = 0;
}

void water_pump_5_scheduler(void)
{
	const uint32_t water_pump_5_schedule[] = {0,
    sp.ID_WATER_PUMP_5_START_TIME_1,
    sp.ID_WATER_PUMP_5_START_TIME_2,
    sp.ID_WATER_PUMP_5_START_TIME_3,
	sp.ID_WATER_PUMP_5_START_TIME_4,
	sp.ID_WATER_PUMP_5_START_TIME_5,
	sp.ID_WATER_PUMP_5_START_TIME_6,
	sp.ID_WATER_PUMP_5_START_TIME_7,
	sp.ID_WATER_PUMP_5_START_TIME_8,
	sp.ID_WATER_PUMP_5_START_TIME_9,
	sp.ID_WATER_PUMP_5_START_TIME_10,
	sp.ID_WATER_PUMP_5_START_TIME_11,
    sp.ID_WATER_PUMP_5_START_TIME_12,
    sp.ID_WATER_PUMP_5_START_TIME_13,
	sp.ID_WATER_PUMP_5_START_TIME_14,
	sp.ID_WATER_PUMP_5_START_TIME_15,
	sp.ID_WATER_PUMP_5_START_TIME_16,
	sp.ID_WATER_PUMP_5_START_TIME_17,
	sp.ID_WATER_PUMP_5_START_TIME_18,
	sp.ID_WATER_PUMP_5_START_TIME_19,
	sp.ID_WATER_PUMP_5_START_TIME_20,
	sp.ID_WATER_PUMP_5_START_TIME_21,
	sp.ID_WATER_PUMP_5_START_TIME_22,
	sp.ID_WATER_PUMP_5_START_TIME_23,
	sp.ID_WATER_PUMP_5_START_TIME_24,
	};
	
	uint8_t current_hour = rtc.u32Hour;
    uint8_t current_minute = rtc.u32Minute;
    uint8_t current_second = rtc.u32Second;
	uint8_t current_hour_24 = (current_hour == 0) ? 24 : current_hour;
	
	uint32_t arr_size = sizeof(water_pump_5_schedule) / sizeof(uint32_t);
	
	if(sp.ID_WATER_PUMP_5_INTERVAL == 0)
		return;
	
	if((current_hour == 0) && (water_pump_5_last_hour == 23))
    {
        if(!water_pump_5_day_changed)
        {
            water_pump_5_day_counter++;
            water_pump_5_day_changed = true;
        }
    }
    else if(current_hour != 0)
    {
        water_pump_5_day_changed = false;
    }
	else{}
	
	if((current_minute == 0) && (current_second < 9))
    {
        if(water_pump_5_schedule_hour_check != current_hour)
        {
            water_pump_5_schedule_hour_check = current_hour;
			
            if((((water_pump_5_day_counter - 1) % sp.ID_WATER_PUMP_5_INTERVAL) == 0) && (op.ID_WATER_PUMP_5 == Off))
            {
				uint8_t power_on_hour_24 = (water_pump_5_day_counter == 1) ? ((water_pump_5_power_on_hour == 0) ? 24 : water_pump_5_power_on_hour) : 0;
				
				for(int i = 1; i < arr_size; i++)
				{
					if(water_pump_5_schedule[i] == 0)
					{
						continue;
					}
					
					if(current_hour_24 == water_pump_5_schedule[i])
					{
						if(water_pump_5_day_counter == 1)
						{
							if(water_pump_5_schedule[i] <= power_on_hour_24)
							{
								continue;
							}
						}
						
						op.ID_WATER_PUMP_5_START_TIME_INDEX = i;
					}
				}
            }
        }
    }
	else if(current_minute != 0)
    {
        water_pump_5_schedule_hour_check = 0xff;
    }
	else{}
	
    water_pump_5_last_hour = current_hour;
}

void water_pump_5_handler(void)
{
	water_pump_5_scheduler();
	
	if((op.ID_WATER_PUMP_5_START_TIME_INDEX != 0) && (sp.ID_WATER_PUMP_5_ON_TIME > 0))
    {
		if(op.ID_WATER_PUMP_5 == Off)
        {
			timer.ID_WATER_PUMP_5_ON_TIME = 0;
			
            WATER_PUMP_5 = On;
			
            op.ID_WATER_PUMP_5 = On;
        }
		else
        {
			if(++timer.ID_WATER_PUMP_5_ON_TIME >= sp.ID_WATER_PUMP_5_ON_TIME)
			{
				timer.ID_WATER_PUMP_5_ON_TIME = 0;
				
				WATER_PUMP_5 = Off;
				
				op.ID_WATER_PUMP_5 = Off;
				op.ID_WATER_PUMP_5_START_TIME_INDEX = 0;
			}
		}
	}
	else if(op.ID_WATER_PUMP_5_START_TIME_INDEX == 0)
    {
		if(op.ID_WATER_PUMP_5 == On)
		{
			timer.ID_WATER_PUMP_5_ON_TIME = 0;
			
			WATER_PUMP_5 = Off;
			
            op.ID_WATER_PUMP_5 = Off;
		}
	}
	else
	{
		op.ID_WATER_PUMP_5_START_TIME_INDEX = 0;
		
		if(op.ID_WATER_PUMP_5 == On)
		{
			WATER_PUMP_5 = Off;
			
			op.ID_WATER_PUMP_5 = Off;
		}
	}
}
