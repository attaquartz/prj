#include "user_common.h"

volatile uint32_t motor_7_fg_last_count = 0;
volatile uint32_t motor_7_watchdog = 0;
static bool heater_1_was_on = false;

static uint32_t motor_8_day_counter = 1;
static uint8_t motor_8_last_hour = 0xff;
static uint8_t motor_8_schedule_hour_check = 0xff;
static bool motor_8_day_changed = false;

void motor_1_scheduler(void)
{
    static bool valve_2_was_on = false;
	
    if(op.ID_VALVE_2 == On)
    {
        valve_2_was_on = true;
    }
    
    if((valve_2_was_on == true) && (op.ID_VALVE_2 == Off))
    {
        if(op.ID_MOTOR_1_INDEX == Off)
        {
			timer.ID_MOTOR_1_DIR_TIME = 0;
            timer.ID_MOTOR_1_OP_TIME = 0;
			
            op.ID_MOTOR_1_INDEX = On;
            op.ID_MOTOR_1_DIR = CW;
            op.ID_MOTOR_1_DIR_INDEX = Stop;
        }
        
        valve_2_was_on = false;
    }
}

void motor_1_handler(void)
{
	motor_1_scheduler();
	
	if(op.ID_MOTOR_1_INDEX == On)
	{
		switch(op.ID_MOTOR_1_DIR)
		{
			case Stop:
			{
				if(MOTOR_1 == On)
				{
					MOTOR_1 = Off;
					MOTOR_1_DIR = Off;
				}
				
				op.ID_MOTOR_1 = Stop;
				
				if(++timer.ID_MOTOR_1_DIR_TIME >= sp.ID_MOTOR_1_STOP_TIME)
				{
					timer.ID_MOTOR_1_DIR_TIME = 0;
					
					if(op.ID_MOTOR_1_DIR_INDEX == CW)
						op.ID_MOTOR_1_DIR = CCW;
					else
						op.ID_MOTOR_1_DIR = CW;
				}
			}
			break;
			
			case CW:
			{
				if(MOTOR_1 == Off)
				{
					MOTOR_1 = On;
					MOTOR_1_DIR = Off;
				}
				
				op.ID_MOTOR_1 = CW;
				
				if(++timer.ID_MOTOR_1_DIR_TIME >= sp.ID_MOTOR_1_CW_TIME)
				{
					timer.ID_MOTOR_1_DIR_TIME = 0;
					
					op.ID_MOTOR_1_DIR = Stop;
					op.ID_MOTOR_1_DIR_INDEX = CW;
				}
			}
			break;
			
			case CCW:
			{
				if(MOTOR_1 == Off)
				{
					MOTOR_1 = On;
					MOTOR_1_DIR = On;
				}
				
				op.ID_MOTOR_1 = CCW;
				
				if(++timer.ID_MOTOR_1_DIR_TIME >= sp.ID_MOTOR_1_CCW_TIME)
				{
					timer.ID_MOTOR_1_DIR_TIME = 0;
					
					op.ID_MOTOR_1_DIR = Stop;
					op.ID_MOTOR_1_DIR_INDEX = CCW;
				}
			}
			break;
		}
		
		if(++timer.ID_MOTOR_1_OP_TIME >= sp.ID_MOTOR_1_OP_TIME)
		{
			timer.ID_MOTOR_1_OP_TIME = 0;
			timer.ID_MOTOR_1_DIR_TIME = 0;
			
			MOTOR_1 = Off;
			MOTOR_1_DIR = Off;
			
			op.ID_MOTOR_1 = Stop;
			op.ID_MOTOR_1_INDEX = Off;
			op.ID_MOTOR_1_DIR = Stop;
			op.ID_MOTOR_1_DIR_INDEX = Stop;
		}
	}
	else
	{
		if(op.ID_MOTOR_1 != Stop)
		{
			MOTOR_1 = Off;
			MOTOR_1_DIR = Off;
			
			op.ID_MOTOR_1 = Stop;
		}
	}
}

void motor_2_scheduler(void)
{
    static bool motor_1_was_on = false;
	static bool actuator_1_was_on = false;
    
    if(op.ID_MOTOR_1_INDEX == On)
    {
        motor_1_was_on = true;
    }
    
    if((motor_1_was_on == true) && (op.ID_MOTOR_1_INDEX == Off))
    {
        if(op.ID_MOTOR_2_INDEX == 0)
        {
			timer.ID_MOTOR_2_DIR_TIME = 0;
            timer.ID_MOTOR_2_OP_TIME = 0;
			
            op.ID_MOTOR_2_INDEX = Dry;
            op.ID_MOTOR_2_DIR = CW;
            op.ID_MOTOR_2_DIR_INDEX = Stop;   
        }
        
        motor_1_was_on = false;
    }
}

void motor_2_handler(void)
{
	uint32_t sp_op_time = 0;
	uint32_t sp_stop_time = 0, sp_cw_time = 0, sp_ccw_time = 0;
	
	motor_2_scheduler();
	
	if(op.ID_MOTOR_2_INDEX != 0)
	{
		if(op.ID_MOTOR_2_INDEX == Dry)
		{
			sp_op_time = sp.ID_MOTOR_2_DRY_OP_TIME;
			sp_stop_time = sp.ID_MOTOR_2_DRY_STOP_TIME;
			sp_cw_time = sp.ID_MOTOR_2_DRY_CW_TIME;
			sp_ccw_time = sp.ID_MOTOR_2_DRY_CCW_TIME;
		}
		else if(op.ID_MOTOR_2_INDEX == Crush)
		{
			sp_op_time = sp.ID_MOTOR_2_CRUSH_OP_TIME;
			sp_stop_time = sp.ID_MOTOR_2_CRUSH_STOP_TIME;
			sp_cw_time = sp.ID_MOTOR_2_CRUSH_CW_TIME;
			sp_ccw_time = sp.ID_MOTOR_2_CRUSH_CCW_TIME;
		}
		else if(op.ID_MOTOR_2_INDEX == Out)
		{
			sp_op_time = sp.ID_MOTOR_2_OUT_OP_TIME;
			sp_stop_time = sp.ID_MOTOR_2_OUT_STOP_TIME;
			sp_cw_time = sp.ID_MOTOR_2_OUT_CW_TIME;
			sp_ccw_time = sp.ID_MOTOR_2_OUT_CCW_TIME;
		}
		else{}
			
		if(sp_op_time == 0)
		{
			if(op.ID_MOTOR_2_INDEX == Dry)
				op.ID_MOTOR_2_INDEX = Crush;
			else if(op.ID_MOTOR_2_INDEX == Crush)
				op.ID_MOTOR_2_INDEX = Out;
			else if(op.ID_MOTOR_2_INDEX == Out)
				op.ID_MOTOR_2_INDEX = 0;
			else{}
			
			MOTOR_2 = Off;
			MOTOR_2_DIR = Off;
			
			op.ID_MOTOR_2 = Stop;
			op.ID_MOTOR_2_DIR = CW;
			op.ID_MOTOR_2_DIR_INDEX = Stop;
			
			return;
		}
			
		switch(op.ID_MOTOR_2_DIR)
		{
			case Stop:
			{
				if(sp_stop_time > 0)
				{
					if(op.ID_MOTOR_2 != Stop)
					{
						MOTOR_2 = Off;
						MOTOR_2_DIR = Off;
						
						op.ID_MOTOR_2 = Stop;
					}
					
					if(++timer.ID_MOTOR_2_DIR_TIME >= sp_stop_time)
					{
						timer.ID_MOTOR_2_DIR_TIME = 0;
						
						op.ID_MOTOR_2_DIR = (op.ID_MOTOR_2_DIR_INDEX == CW) ? CCW : CW;
					}
				}
				else
				{
					timer.ID_MOTOR_2_DIR_TIME = 0;
					
					op.ID_MOTOR_2_DIR = (op.ID_MOTOR_2_DIR_INDEX == CW) ? CCW : CW;
				}
			}
			break;
			
			case CW:
			{
				if(sp_cw_time > 0)
				{
					if(op.ID_MOTOR_2 != CW)
					{
						MOTOR_2 = On;
						MOTOR_2_DIR = Off;
						
						op.ID_MOTOR_2 = CW;
					}
					
					if(++timer.ID_MOTOR_2_DIR_TIME >= sp_cw_time)
					{
						timer.ID_MOTOR_2_DIR_TIME = 0;
						
						op.ID_MOTOR_2_DIR = Stop;
						op.ID_MOTOR_2_DIR_INDEX = CW;
					}
				}
				else
				{
					timer.ID_MOTOR_2_DIR_TIME = 0;
					
					op.ID_MOTOR_2_DIR = Stop;
					op.ID_MOTOR_2_DIR_INDEX = CW;
				}
			}
			break;
			
			case CCW:
			{
				if(sp_ccw_time > 0)
				{
					if(op.ID_MOTOR_2 != CCW)
					{
						MOTOR_2 = On;
						MOTOR_2_DIR = On;
						
						op.ID_MOTOR_2 = CCW;
					}
					
					if(++timer.ID_MOTOR_2_DIR_TIME >= sp_ccw_time)
					{
						timer.ID_MOTOR_2_DIR_TIME = 0;
						
						op.ID_MOTOR_2_DIR = Stop;
						op.ID_MOTOR_2_DIR_INDEX = CCW;
					}
				}
				else
				{
					timer.ID_MOTOR_2_DIR_TIME = 0;
					
					op.ID_MOTOR_2_DIR = Stop;
					op.ID_MOTOR_2_DIR_INDEX = CCW;
				}
			}
			break;
		}
		
		if(++timer.ID_MOTOR_2_OP_TIME >= sp_op_time)
		{
			timer.ID_MOTOR_2_OP_TIME = 0;
			timer.ID_MOTOR_2_DIR_TIME = 0;
			
			MOTOR_2 = Off;
			MOTOR_2_DIR = Off;
			
			op.ID_MOTOR_2 = Stop;
			op.ID_MOTOR_2_DIR = CW;
			op.ID_MOTOR_2_DIR_INDEX = Stop;
			
			if(op.ID_MOTOR_2_INDEX == Dry)
			{
				op.ID_MOTOR_2_INDEX = Crush;
			}
			else if(op.ID_MOTOR_2_INDEX == Crush)
			{
				op.ID_MOTOR_2_INDEX = Out;
			}
			else if(op.ID_MOTOR_2_INDEX == Out)
			{
				op.ID_MOTOR_2_INDEX = 0;
			}
			else{}
		}
	}
}

void motor_3_handler(void)
{
	static uint8_t motor_2_prev_index = 0;
	
	if((motor_2_prev_index == Crush) && (op.ID_MOTOR_2_INDEX == Out))
	{
		if(op.ID_MOTOR_3 == Off)
        {
			timer.ID_MOTOR_3_OP_TIME = 0;
			
            MOTOR_3 = On;
			
            op.ID_MOTOR_3 = On;
        }
	}
	
	if(op.ID_MOTOR_3 == On)
	{
		if(++timer.ID_MOTOR_3_OP_TIME >= sp.ID_MOTOR_3_OP_TIME)
		{
			timer.ID_MOTOR_3_OP_TIME = 0;
			
			MOTOR_3 = Off;
			
			op.ID_MOTOR_3 = Off;
		}
	}
	
	motor_2_prev_index = op.ID_MOTOR_2_INDEX;
}

void motor_4_handler(void)
{
	if((alert_sensor[TEMP_1].state == Enable) && (op.ID_MOTOR_4 == Off))
    {
		timer.ID_MOTOR_4_OP_TIME = 0;
		timer.ID_MOTOR_4_DIR_TIME = 0;
		
        MOTOR_4 = On;
		
        op.ID_MOTOR_4 = CW;
		op.ID_MOTOR_4_DIR = CW;
    }
	
	if(op.ID_MOTOR_4 != Off)
	{
		if(op.ID_MOTOR_4_DIR == CW)
		{
			if(++timer.ID_MOTOR_4_DIR_TIME >= sp.ID_MOTOR_4_CW_TIME)
			{
				timer.ID_MOTOR_4_DIR_TIME = 0;
				
				MOTOR_4 = On;
				
				op.ID_MOTOR_4 = CCW;
				op.ID_MOTOR_4_DIR = CCW;
			}
		}
		else if(op.ID_MOTOR_4_DIR == CCW)
		{
			if(++timer.ID_MOTOR_4_DIR_TIME >= sp.ID_MOTOR_4_CCW_TIME)
			{
				timer.ID_MOTOR_4_DIR_TIME = 0;
				
				MOTOR_4 = Off;
				
				op.ID_MOTOR_4 = CW;
				op.ID_MOTOR_4_DIR = CW;
			}
		}
		else{}
			
		if(++timer.ID_MOTOR_4_OP_TIME >= sp.ID_MOTOR_4_OP_TIME)
		{
			timer.ID_MOTOR_4_OP_TIME = 0;
			timer.ID_MOTOR_4_DIR_TIME = 0;
			
			MOTOR_4 = Off;
			
			op.ID_MOTOR_4 = Off;
			op.ID_MOTOR_4_DIR = 0;
		}
	}
}

void motor_5_handler(void)
{
	if((alert_sensor[TEMP_1].state == Enable) && (op.ID_MOTOR_5_DIR == Off))
    {
		timer.ID_MOTOR_5_OP_TIME = 0;
		timer.ID_MOTOR_5_DIR_TIME = 0;
		
        MOTOR_5 = On;
		
        op.ID_MOTOR_5 = On;
		op.ID_MOTOR_5_DIR = On_Time;
    }
	
	if(op.ID_MOTOR_5_DIR != Off)
	{
		if(op.ID_MOTOR_5_DIR == On_Time)
		{
			if(++timer.ID_MOTOR_5_DIR_TIME >= sp.ID_MOTOR_5_ON_TIME)
			{
				timer.ID_MOTOR_5_DIR_TIME = 0;
				
				MOTOR_5 = Off;
				
				op.ID_MOTOR_5 = Off;
				op.ID_MOTOR_5_DIR = Off_Time;
			}
		}
		else if(op.ID_MOTOR_5_DIR == Off_Time)
		{
			if(++timer.ID_MOTOR_5_DIR_TIME >= sp.ID_MOTOR_5_OFF_TIME)
			{
				timer.ID_MOTOR_5_DIR_TIME = 0;
				
				MOTOR_5 = On;
				
				op.ID_MOTOR_5 = On;
				op.ID_MOTOR_5_DIR = On_Time;
			}
		}
		
		if(++timer.ID_MOTOR_5_OP_TIME >= sp.ID_MOTOR_5_OP_TIME)
		{
			timer.ID_MOTOR_5_OP_TIME = 0;
			timer.ID_MOTOR_5_DIR_TIME = 0;
			
			MOTOR_5 = Off;
			
			op.ID_MOTOR_5 = Off;
			op.ID_MOTOR_5_DIR = 0;
		}
	}
}

void motor_6_handler(void)
{
	if((alert_sensor[TEMP_1].state == Enable) && (op.ID_MOTOR_6 == Off))
    {
		timer.ID_MOTOR_6_OP_TIME = 0;
		timer.ID_MOTOR_6_DIR_TIME = 0;
		
        MOTOR_6 = On;
		
        op.ID_MOTOR_6 = CW;
		op.ID_MOTOR_6_DIR = CW;
    }
	
	if(op.ID_MOTOR_6 != Off)
	{
		if(op.ID_MOTOR_6_DIR == CW)
		{
			if(++timer.ID_MOTOR_6_DIR_TIME >= sp.ID_MOTOR_6_CW_TIME)
			{
				timer.ID_MOTOR_6_DIR_TIME = 0;
				
				MOTOR_6 = Off;
				
				op.ID_MOTOR_6 = CCW;
				op.ID_MOTOR_6_DIR = CCW;
			}
		}
		else if(op.ID_MOTOR_6_DIR == CCW)
		{
			if(++timer.ID_MOTOR_6_DIR_TIME >= sp.ID_MOTOR_6_CCW_TIME)
			{
				timer.ID_MOTOR_6_DIR_TIME = 0;
				
				MOTOR_6 = On;
				
				op.ID_MOTOR_6 = CW;
				op.ID_MOTOR_6_DIR = CW;
			}
		}
		else{}
			
		if(++timer.ID_MOTOR_6_OP_TIME >= sp.ID_MOTOR_6_OP_TIME)
		{
			timer.ID_MOTOR_6_OP_TIME = 0;
			timer.ID_MOTOR_6_DIR_TIME = 0;
			
			MOTOR_6 = Off;
			
			op.ID_MOTOR_6 = Off;
			op.ID_MOTOR_6_DIR = 0;
		}
	}
}

void motor_7_reset(void)
{
	heater_1_was_on = false;
	op.ID_MOTOR_7 = Off;
	timer.ID_MOTOR_7_OP_TIME = 0;
	
	op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(0);
    PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
}

void motor_7_handler(void)
{
	if((alert_sensor[TEMP_1].state == Enable) && (op.ID_MOTOR_7 == Off))
    {
		timer.ID_MOTOR_7_OP_TIME = 0;
		
		op.ID_MOTOR_7 = On;
        op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(sp.ID_MOTOR_7_SET_RPM);
		
		PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
    }
	
	if(op.ID_HEATER_1 == On)
    {
        heater_1_was_on = true;
    }
	
	if((heater_1_was_on == true) && (op.ID_HEATER_1 == Off))
	{
		if(op.ID_MOTOR_7 == On)
		{
			if(++timer.ID_MOTOR_7_OP_TIME >= sp.ID_MOTOR_7_OP_TIME)
			{
				timer.ID_MOTOR_7_OP_TIME = 0;
				
				op.ID_MOTOR_7 = Off;
				op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(0);
				
				PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
				
				heater_1_was_on = false;
			}
		}
	}
	else if(alert_sensor[TEMP_1].state == Disable)
	{
		if(op.ID_MOTOR_7 == On)
		{
			if(++timer.ID_MOTOR_7_OP_TIME >= sp.ID_MOTOR_7_OP_TIME)
			{
				timer.ID_MOTOR_7_OP_TIME = 0;
				
				op.ID_MOTOR_7 = Off;
				op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(0);
				
				PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
				
				heater_1_was_on = false;
			}
		}
	}
	else{}
}

void motor_7_set(uint32_t state)
{
	if(state == On)
	{
		op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(sp.ID_MOTOR_7_SET_RPM);
	
		PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
	}
	else
	{
		op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(0);
	
		PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
	}
}

void motor_8_scheduler_init(void)
{
    motor_8_day_counter = 1;
    motor_8_last_hour = 0xff;
    motor_8_schedule_hour_check = 0xff;
    motor_8_day_changed = false;
    
    MOTOR_8 = On;
    op.ID_MOTOR_8 = On;
}

void motor_8_handler(void)
{
	const uint32_t sludge_discharge_schedule[] = {0,
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
	
	uint32_t arr_size = sizeof(sludge_discharge_schedule) / sizeof(uint32_t);
	
	if(sp.ID_SLUDGE_DISCHARGE_INTERVAL == 0)
		return;
	
	__disable_irq();
	uint8_t current_hour = rtc.u32Hour;
    uint8_t current_minute = rtc.u32Minute;
    uint8_t current_second = rtc.u32Second;
	__enable_irq();
    
    if((current_hour == 0) && (motor_8_last_hour == 23))
    {
        if(!motor_8_day_changed)
        {
            motor_8_day_counter++;
            motor_8_day_changed = true;
        }
    }
    else if(current_hour != 0)
    {
        motor_8_day_changed = false;
    }
    
    motor_8_last_hour = current_hour;
	
	if(op.ID_MOTOR_8 == On)
    {
         if(((motor_8_day_counter - 1) % sp.ID_SLUDGE_DISCHARGE_INTERVAL) == 0)
        {
            uint32_t current_seconds = (current_hour * 3600) + (current_minute * 60) + current_second;
            
			for(int i = 1; i < arr_size; i++)
            {
                if(sludge_discharge_schedule[i] == 0)
                {
                    continue;
                }
                
                uint32_t pump_hour = (sludge_discharge_schedule[i] == 24) ? 0 : sludge_discharge_schedule[i];
                uint32_t pump_time_seconds = pump_hour * 3600;
                int32_t off_time_seconds = pump_time_seconds - sp.ID_MOTOR_8_OFF_DELAY;
                
                if(off_time_seconds < 0)
                {
                    off_time_seconds += 86400;
                }
                
                if((current_seconds >= off_time_seconds) && (current_seconds < off_time_seconds + 9))
                {
                    MOTOR_8 = Off;
                    op.ID_MOTOR_8 = Off;
					op.ID_MOTOR_8_OFF_TIME_INDEX = i;
                    
                    return;
                }
            }
        }
    }
	
	if((op.ID_WATER_PUMP_1 == Off) && (op.ID_MOTOR_8_OFF_TIME_INDEX == 0))
	{
		timer.ID_MOTOR_8_OFF_DELAY = 0;
		
		if(op.ID_MOTOR_8 == Off)
		{
			MOTOR_8 = On;
			
			op.ID_MOTOR_8 = On;
		}
	}
}

void motor_9_handler(void)
{
	if(op.ID_MOTOR_9 == Off)
	{
		if(MOTOR_9 == On)
		{
			MOTOR_9 = Off;
		}
	}
	else
	{
		if(MOTOR_9 == Off)
		{
			MOTOR_9 = On;
		}
	}
}

void actuator_1_handler(void)
{
	static uint8_t motor_2_prev_index = 0;
	
	if((motor_2_prev_index == Crush) && (op.ID_MOTOR_2_INDEX == Out))
	{
		if(op.ID_ACTUATOR_1 == Off)
        {
			timer.ID_ACTUATOR_1_OP_TIME = 0;
			
            ACTUATOR_1 = On;
			
            op.ID_ACTUATOR_1 = On;
        }
	}
	
	if(op.ID_ACTUATOR_1 == On)
	{
		if(++timer.ID_ACTUATOR_1_OP_TIME >= sp.ID_ACTUATOR_1_OP_TIME)
		{
			timer.ID_ACTUATOR_1_OP_TIME = 0;
			
			ACTUATOR_1 = Off;
			
			op.ID_ACTUATOR_1 = Off;
		}
	}
	
	motor_2_prev_index = op.ID_MOTOR_2_INDEX;
}
