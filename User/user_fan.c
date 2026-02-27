#include "user_common.h"

static bool fan_1_handler_motor_1_was_off = true;
static bool fan_2_handler_motor_1_was_off = true;
static bool ptc_fan_handler_motor_1_was_on = false;

void fan_1_reset(void)
{
	FAN_1 = Off;
	
	op.ID_FAN_1 = Off;
	
	timer.ID_FAN_1_OP_TIME = 0;
	
	fan_1_handler_motor_1_was_off = true; //(op.ID_MOTOR_1_INDEX == Off);
}

void fan_1_handler(void)
{
	if((fan_1_handler_motor_1_was_off == true) && (op.ID_MOTOR_1_INDEX == On))
	{
		if(op.ID_FAN_1 == Off)
		{
			timer.ID_FAN_1_OP_TIME = 0;
		
			FAN_1 = On;
			
			op.ID_FAN_1 = On;
		}
	}
	
	fan_1_handler_motor_1_was_off = (op.ID_MOTOR_1_INDEX == Off);
	
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

void fan_2_reset(void)
{
	FAN_2 = Off;
	
	op.ID_FAN_2 = Off;
	
	timer.ID_FAN_2_OP_TIME = 0;
	
	fan_2_handler_motor_1_was_off = true; //(op.ID_MOTOR_1_INDEX == Off);
}

void fan_2_handler(void)
{
	if((fan_2_handler_motor_1_was_off == true) && (op.ID_MOTOR_1_INDEX == On))
	{
		if(op.ID_FAN_2 == Off)
		{
			timer.ID_FAN_2_OP_TIME = 0;
		
			FAN_2 = On;
			
			op.ID_FAN_2 = On;
		}
	}
	
	fan_2_handler_motor_1_was_off = (op.ID_MOTOR_1_INDEX == Off);
	
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

void fan_3_reset(void)
{
	FAN_3 = Off;
	
	op.ID_FAN_3 = Off;
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

void ptc_fan_reset(void)
{
	PTC_FAN_1 = Off;
	PTC_FAN_2 = Off;
	
	op.ID_PTC_FAN = Off;
	
	timer.ID_PTC_FAN_OP_TIME = 0;
	
	ptc_fan_handler_motor_1_was_on = false;
}

void ptc_fan_handler(void)
{
	if(op.ID_MOTOR_1_INDEX == On)
    {
        ptc_fan_handler_motor_1_was_on = true;
    }
	
	if((ptc_fan_handler_motor_1_was_on == true) && (op.ID_MOTOR_1_INDEX == Off))
	{
		if(op.ID_PTC_FAN == Off)
		{
			PTC_FAN_1 = On;
			PTC_FAN_2 = On;
			
			op.ID_PTC_FAN = (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
			
			timer.ID_PTC_FAN_OP_TIME = 0;
		}
		
		ptc_fan_handler_motor_1_was_on = false;
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
				
				op.ID_PTC_FAN = (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
			}
		}
		else
		{
			PTC_FAN_1 = Off;
			PTC_FAN_2 = Off;
			
			op.ID_PTC_FAN = (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
		}
	}
}
