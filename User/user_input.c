#include "user_common.h"

input_t zero_crossing;
input_t level_sensor[LEVEL_SENSOR_MAX];
input_t alert_sensor[ALERT_SENSOR_MAX];
input_t user_switch[USER_SWITCH_MAX];
ct_t ct_sensor;

void zero_crossing_handler(void)
{
	op.ID_SENSOR = (op.ID_SENSOR & ~(1 << ZERO_CROSSING)) | (zero_crossing.state << ZERO_CROSSING);
	
	if(zero_crossing.state == Enable)
	{
		
	}
	else
	{
		
	}
}

void level_sensor_handler(uint32_t lv)
{
	op.ID_LEVEL_SENSOR = (op.ID_LEVEL_SENSOR & ~(1 << lv)) | (level_sensor[lv].state << lv);
	
	if(level_sensor[lv].state == Enable)
	{
		switch(lv)
		{
			case H_FILTER:
			{
				
			}
			break;
			
			case L_FILTER:
			{
				
			}
			break;
			
			case LL_FILTER:
			{
				
			}
			break;
			
			case L_REUSE:
			{
				
			}
			break;
			
			case L_EQ:
			{
				
			}
			break;
			
			case L_S2:
			{
				
			}
			break;
			
			default:break;
		}
	}
	else
	{
		switch(lv)
		{
			case H_FILTER:
			{
				
			}
			break;
			
			case L_FILTER:
			{
				
			}
			break;
			
			case LL_FILTER:
			{
				
			}
			break;
			
			case L_REUSE:
			{
				
			}
			break;
			
			case L_EQ:
			{
				
			}
			break;
			
			case L_S2:
			{
				
			}
			break;
			
			default:break;
		}
	}
}

void alert_sensor_handler(uint32_t alert)
{
	op.ID_SENSOR = (op.ID_SENSOR & ~(1 << alert)) | (alert_sensor[alert].state << alert);
	
	if(alert_sensor[alert].state == Enable)
	{
		switch(alert)
		{
			case TEMP_1:
			{
				
			}
			break;
			
			case TEMP_2:
			{
				
			}
			break;
			
			case TEMP_3:
			{
				
			}
			break;
			
			case TEMP_4:
			{
				
			}
			break;
			
			default:break;
		}
	}
	else
	{
		switch(alert)
		{
			case TEMP_1:
			{
				
			}
			break;
			
			case TEMP_2:
			{
				
			}
			break;
			
			case TEMP_3:
			{
				
			}
			break;
			
			case TEMP_4:
			{
				
			}
			break;
			
			default:break;
		}
	}
}

void user_switch_handler(uint32_t sw)
{
	op.ID_SWITCH = (op.ID_SWITCH & ~(1 << sw)) | (user_switch[sw].state << sw);
	
	if(user_switch[sw].state == Enable)
	{
		switch(sw)
		{
			case AUTO_START:
			{
				
			}
			break;
			
			case AUTO_STOP:
			{
				
			}
			break;
			
			case FAUCET:
			{
				
			}
			break;
			
			case EMERGENCY:
			{
				emergency_handler(Enable);
			}
			break;
			
			default:break;
		}
	}
	else
	{
		switch(sw)
		{
			case AUTO_START:
			{
				
			}
			break;
			
			case AUTO_STOP:
			{
				
			}
			break;
			
			case FAUCET:
			{
				
			}
			break;
			
			case EMERGENCY:
			{
				emergency_handler(Disable);
			}
			break;
			
			default:break;
		}
	}
}

void ct_sensor_handler(void)
{
	if(ct_sensor.adc_done_flag == true)
	{
		ct_sensor.adc_done_flag = false;
		
		// check load state, ct_sensor.state
	}
}