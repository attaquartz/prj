#include "user_common.h"

void aeration_1_handler(void)
{
	if((op.ID_VALVE_3 == Off) && (op.ID_VALVE_4 == Off) && (op.ID_VALVE_5 == Off) && (op.ID_VALVE_6 == Off))
	{
		if(AERATION_1 == On)
		{
			AERATION_1 = Off;
		}
		
		op.ID_AERATION_1 = Off;
	}
	else
	{
		if(AERATION_1 == Off)
		{
			AERATION_1 = On;
		}
		
		op.ID_AERATION_1 = On;
	}
}

void aeration_2_handler(void)
{
	if(op.ID_AERATION_2 == On)
	{
		if(AERATION_2 == Off)
		{
			AERATION_2 = On;
		}
	}
	else
	{
		if(AERATION_2 == On)
		{
			AERATION_2 = Off;
		}
	}
}

void aeration_3_handler(void)
{
	static bool pump_1_was_on = false;
	
	if(op.ID_AERATION_3 == Off)
	{
		pump_1_was_on = false;
		
		if(level_sensor[L_FILTER].state == Enable)
        {
            AERATION_3 = On;
			
            op.ID_AERATION_3 = On;
        }
	}
	else
	{
		if(op.ID_PUMP_1 == On)
        {
            pump_1_was_on = true;
			
            timer.ID_AERATION_3_OFF_DELAY = 0;
        }
		
		if((pump_1_was_on == true) && (op.ID_PUMP_1 == Off))
        {
			if(sp.ID_AERATION_3_OFF_DELAY > 0)
			{
				if(++timer.ID_AERATION_3_OFF_DELAY >= sp.ID_AERATION_3_OFF_DELAY)
				{
					timer.ID_AERATION_3_OFF_DELAY = 0;
					
					AERATION_3 = Off;
					
					op.ID_AERATION_3 = Off;
					
					pump_1_was_on = false;
				}
			}
			else
			{
				AERATION_3 = Off;
				
				op.ID_AERATION_3 = Off;
				
				pump_1_was_on = false;
			}
		}
		else if(pump_1_was_on == false)
		{
			timer.ID_AERATION_3_OFF_DELAY = 0;
		}
		else{}
	}
}
