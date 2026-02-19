#include "user_common.h"

void fan_1_handler(void)
{
	static bool motor_1_was_off = true;
	
	if((motor_1_was_off == true) && (op.ID_MOTOR_1_INDEX == On))
	{
		if(op.ID_FAN_1 == Off)
		{
			timer.ID_FAN_1_OP_TIME = 0;
		
			FAN_1 = On;
			
			op.ID_FAN_1 = On;
		}
	}
	
	motor_1_was_off = (op.ID_MOTOR_1_INDEX == Off);
	
	if(op.ID_FAN_1 == On)
	{
		if(sp.ID_FAN_1_OP_TIME > 0)
		{
			if(++timer.ID_FAN_1_OP_TIME >= sp.ID_FAN_1_OP_TIME)
			{
				timer.ID_FAN_1_OP_TIME = 0;
				
				FAN_1 = Off;
				
				op.ID_FAN_1 = Off;
			}
		}
		else
		{
			FAN_1 = Off;
				
			op.ID_FAN_1 = Off;
		}
	}
}

void fan_2_handler(void)
{
	static bool motor_1_was_off = true;
	
	if((motor_1_was_off == true) && (op.ID_MOTOR_1_INDEX == On))
	{
		if(op.ID_FAN_2 == Off)
		{
			timer.ID_FAN_2_OP_TIME = 0;
		
			FAN_2 = On;
			
			op.ID_FAN_2 = On;
		}
	}
	
	motor_1_was_off = (op.ID_MOTOR_1_INDEX == Off);
	
	if(op.ID_FAN_2 == On)
	{
		if(sp.ID_FAN_2_OP_TIME > 0)
		{
			if(++timer.ID_FAN_2_OP_TIME >= sp.ID_FAN_2_OP_TIME)
			{
				timer.ID_FAN_2_OP_TIME = 0;
				
				FAN_2 = Off;
				
				op.ID_FAN_2 = Off;
			}
		}
		else
		{
			FAN_2 = Off;
				
			op.ID_FAN_2 = Off;
		}
	}
}

void fan_3_handler(void)
{
	if(op.ID_FAN_3 == Off)
	{
		if(FAN_3 == On)
		{
			FAN_3 = Off;
		}
	}
	else
	{
		if(FAN_3 == Off)
		{
			FAN_3 = On;
		}
	}
}

void ptc_fan_handler(void)
{
	static bool motor_1_was_on = false;
	static uint8_t motor_2_prev_index = 0; 
	
	if(op.ID_MOTOR_1_INDEX == On)
    {
        motor_1_was_on = true;
    }
	
	if((motor_1_was_on == true) && (op.ID_MOTOR_1_INDEX == Off))
	{
		if(op.ID_PTC_FAN == Off)
		{
			PTC_FAN_1 = On;
			PTC_FAN_2 = On;
			PTC_FAN_3 = On;
			PTC_FAN_4 = On;
			PTC_FAN_5 = On;
			PTC_FAN_6 = On;
			
			op.ID_PTC_FAN = (PTC_FAN_6 << 5) | (PTC_FAN_5 << 4) | \
							(PTC_FAN_4 << 3) | (PTC_FAN_3 << 2) | (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
		}
		
		motor_1_was_on = false;
	}
	
	if(op.ID_PTC_FAN != Off)
	{
		if(sp.ID_PTC_FAN_OP_TIME > 0)
		{
			if(++timer.ID_PTC_FAN_OP_TIME >= sp.ID_PTC_FAN_OP_TIME)
			{
				timer.ID_PTC_FAN_OP_TIME = 0;
				
				PTC_FAN_1 = Off;
				PTC_FAN_2 = Off;
				PTC_FAN_3 = Off;
				PTC_FAN_4 = Off;
				PTC_FAN_5 = Off;
				PTC_FAN_6 = Off;
				
				op.ID_PTC_FAN = (PTC_FAN_6 << 5) | (PTC_FAN_5 << 4) | \
								(PTC_FAN_4 << 3) | (PTC_FAN_3 << 2) | (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
			}
		}
		else
		{
			PTC_FAN_1 = Off;
			PTC_FAN_2 = Off;
			PTC_FAN_3 = Off;
			PTC_FAN_4 = Off;
			PTC_FAN_5 = Off;
			PTC_FAN_6 = Off;
			
			op.ID_PTC_FAN = (PTC_FAN_6 << 5) | (PTC_FAN_5 << 4) | \
							(PTC_FAN_4 << 3) | (PTC_FAN_3 << 2) | (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
		}
	}
	
	 motor_2_prev_index = op.ID_MOTOR_2_INDEX;
}
