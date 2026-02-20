#include "user_common.h"

timer_tick_t timer;
S_RTC_TIME_DATA_T rtc;
operation_parameter_t op;
system_parameter_t sp;

/* CRC16 implementation acording to CCITT standards */
const unsigned short crc16tab[256] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

uint16_t crc16_ccitt(volatile uint8_t *buf, uint32_t len)
{	
	uint16_t counter;
	uint16_t crc = 0;

	for(counter = 0; counter < len; counter++)
	{
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ (*buf++)) & 0x00ff];
	}

	return crc;
}

uint32_t RTCToUnixTimestamp(S_RTC_TIME_DATA_T *rtc)
{
	const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint32_t timestamp = 0;
    uint32_t year, month, day, hour, minute, second;
    uint32_t days = 0;
	
	if(rtc == NULL)
		return 0;
	
    year = rtc->u32Year;
    month = rtc->u32Month;
    day = rtc->u32Day;
    hour = rtc->u32Hour;
    minute = rtc->u32Minute;
    second = rtc->u32Second;
    
	for(int y = 1970; y < year; y++)
    {
        if(IS_LEAP_YEAR(y))
            days += 366;
        else
            days += 365;
    }
	
	for(int m = 1; m < month; m++)
    {
        days += days_in_month[m - 1];
        
        if(m == 2 && IS_LEAP_YEAR(year))
            days++;
    }
	
	days += (day - 1);
    
    timestamp = (days * 86400) + (hour * 3600) + (minute * 60) + second;
	
	return timestamp;
}

void timer_interrupt_handler(void)
{
	if((op.ID_DEV_STATE & BIT0) && (++timer.modbus_server_tick >= MODBUS_SERVER_DETECT_MS))
	{
		op.ID_DEV_STATE &= ~BIT0;
	}
}

void input_interrupt_handler(void)
{
	if(zero_crossing.flag == true)
	{
		if((SENSOR_ZERO_CROSSING == 1) && (zero_crossing.state == 0))
		{
			if(++zero_crossing.tick >= ZERO_CROSSING_DETECT_MS)
			{
				zero_crossing.flag = false;
				zero_crossing.state = Enable;
				zero_crossing.tick = 0;
				
				run_event(STATUS_ZERO_CROSSING, 0); // zero_crossing_handler
			}
		}
		else
		{
			zero_crossing.flag = false;
			zero_crossing.state = Disable;
			zero_crossing.tick = 0;
			
			run_event(STATUS_ZERO_CROSSING, 0); // zero_crossing_handler
		}
	}
	
	for(int i = 0; i < LEVEL_SENSOR_MAX; i++)
	{
		if(level_sensor[i].flag == true)
		{
			if((LEVEL_SENSOR(i) == Enable) && (level_sensor[i].state == Disable))
			{
				if(++level_sensor[i].tick >= sp.ID_HYSTERESIS_TIME)
				{
					level_sensor[i].flag = false;
					level_sensor[i].state = Enable;
					level_sensor[i].tick = 0;
					
					run_event(STATUS_LEVEL_SENSOR, i); // level_sensor_handler
				}
			}
			else
			{
				level_sensor[i].flag = false;
				level_sensor[i].state = Disable;
				level_sensor[i].tick = 0;
				
				run_event(STATUS_LEVEL_SENSOR, i); // level_sensor_handler
			}
		}
	}
	
	for(int i = 0; i < ALERT_SENSOR_MAX; i++)
	{
		if(alert_sensor[i].flag == true)
		{
			if((ALERT_SWITCH(i) == Enable) && (alert_sensor[i].state == Disable))
			{
				if(++alert_sensor[i].tick >= ALERT_SENSOR_DETECT_MS)
				{
					alert_sensor[i].flag = false;
					alert_sensor[i].state = Enable;
					alert_sensor[i].tick = 0;
					
					run_event(STATUS_ALERT_SENSOR, i); // alert_sensor_handler
				}
			}
			else
			{
				alert_sensor[i].flag = false;
				alert_sensor[i].state = Disable;
				alert_sensor[i].tick = 0;
				
				run_event(STATUS_ALERT_SENSOR, i); // alert_sensor_handler
			}
		}
	}
	
	for(int i = 0; i < USER_SWITCH_MAX; i++)
	{
		if(user_switch[i].flag == true)
		{
			if((USER_SWITCH(i) == On) && (user_switch[i].state == Off))
			{
				if(++user_switch[i].tick >= USER_SWITCH_DETECT_MS)
				{
					user_switch[i].flag = false;
					user_switch[i].state = On;
					user_switch[i].tick = 0;
					
					run_event(STATUS_USER_SWITCH, i); // user_switch_handler
				}
			}
			else
			{
				user_switch[i].flag = false;
				user_switch[i].state = Off;
				user_switch[i].tick = 0;
				
				run_event(STATUS_USER_SWITCH, i); // user_switch_handler
			}
		}
	}
}

void emergency_handler(uint32_t state)
{
	if(state == Enable)
	{
		set_operation_mode(ALL_Manual);
		
		MOTOR_1 = Off;
		MOTOR_1_DIR = Stop;
        op.ID_MOTOR_1 = Stop;
		op.ID_MOTOR_1_DIR = Stop;
		op.ID_MOTOR_1_INDEX = Off;
		op.ID_MOTOR_1_DIR_INDEX = Stop;
        timer.ID_MOTOR_1_OP_TIME  = 0;
		timer.ID_MOTOR_1_DIR_TIME  = 0;
		
		MOTOR_2 = Off;
		MOTOR_2_DIR = Stop;
        op.ID_MOTOR_2 = Stop;
		op.ID_MOTOR_2_DIR = Stop;
		op.ID_MOTOR_2_INDEX = 0;
		op.ID_MOTOR_2_DIR_INDEX = Stop;
        timer.ID_MOTOR_2_OP_TIME = 0;
		timer.ID_MOTOR_2_DIR_TIME  = 0;
		
		MOTOR_3 = Off;
        op.ID_MOTOR_3 = Off;
        timer.ID_MOTOR_3_OP_TIME = 0;
		
		MOTOR_4 = Off;
        op.ID_MOTOR_4 = Stop;
		op.ID_MOTOR_4_DIR = Stop;
        timer.ID_MOTOR_4_OP_TIME = 0;
		timer.ID_MOTOR_4_DIR_TIME = 0;
		
		MOTOR_5 = Off;
        op.ID_MOTOR_5 = Stop;
		op.ID_MOTOR_5_DIR = Stop;
        timer.ID_MOTOR_5_OP_TIME = 0;
		timer.ID_MOTOR_5_DIR_TIME = 0;

        MOTOR_6 = Off;
        op.ID_MOTOR_6 = Stop;
		op.ID_MOTOR_6_DIR = Stop;
        timer.ID_MOTOR_6_OP_TIME = 0;
		timer.ID_MOTOR_6_DIR_TIME = 0;

        motor_7_reset();
		
		MOTOR_8 = Off;
        op.ID_MOTOR_8 = Off;
		op.ID_MOTOR_8_OFF_TIME_INDEX = 0;
        timer.ID_MOTOR_8_OFF_DELAY = 0;

        MOTOR_9 = Off;
        op.ID_MOTOR_9 = Off;
		
		PUMP_1 = Off;
		op.ID_PUMP_1 = Off;
		timer.ID_PUMP_1_ON_DELAY = 0;
		
        PUMP_2 = Off;
		op.ID_PUMP_2 = Off;
		timer.ID_PUMP_2_OP_TIME = 0;
		
        PUMP_3 = Off;
		op.ID_PUMP_3 = Off;
		op.ID_PUMP_3_START_TIME_INDEX = 0;
		timer.ID_PUMP_3_ON_TIME = 0;
		
		WATER_PUMP_1 = Off;
		op.ID_WATER_PUMP_1 = Off;
		op.ID_WATER_PUMP_1_START_TIME_INDEX = 0;
		timer.ID_WATER_PUMP_1_ON_TIME = 0;
		
        WATER_PUMP_2 = Off;
		op.ID_WATER_PUMP_2 = Off;
		op.ID_WATER_PUMP_2_START_TIME_INDEX = 0;
		timer.ID_WATER_PUMP_2_ON_TIME = 0;
		
        WATER_PUMP_3 = Off;
		op.ID_WATER_PUMP_3 = Off;
		
        WATER_PUMP_4 = Off;
		op.ID_WATER_PUMP_4 = Off;
		timer.ID_WATER_PUMP_4_TIMEOUT = 0;
		
        WATER_PUMP_5 = Off;
		op.ID_WATER_PUMP_5 = Off;
		op.ID_WATER_PUMP_5_START_TIME_INDEX = 0;
		timer.ID_WATER_PUMP_5_ON_TIME = 0;
		
		AERATION_1 = Off;
		op.ID_AERATION_1 = Off;
		
        AERATION_2 = Off;
		op.ID_AERATION_2 = Off;
		
        AERATION_3 = Off;
		op.ID_AERATION_3 = Off;
		timer.ID_AERATION_3_OFF_DELAY = 0;
		
		VALVE_1 = Off;
		op.ID_VALVE_1 = Off;
		op.ID_VALVE_1_START_TIME_INDEX = 0;
		timer.ID_VALVE_1_ON_TIME = 0;
		
        VALVE_2 = Off;
		op.ID_VALVE_2 = Off;
		op.ID_VALVE_2_OP_INDEX = 0;
		timer.ID_VALVE_2_ON_TIME = 0;
		timer.ID_VALVE_2_ON_DELAY = 0;
		
        VALVE_3 = Off;
		op.ID_VALVE_3 = Off;
		timer.ID_VALVE_3_ON_TIME = 0;
		timer.ID_VALVE_3_OFF_DELAY = 0;
		
        VALVE_4 = Off;
		op.ID_VALVE_4 = Off;
		timer.ID_VALVE_4_ON_TIME = 0;
		timer.ID_VALVE_4_OFF_DELAY = 0;
		
        VALVE_5 = Off;
		op.ID_VALVE_5 = Off;
		timer.ID_VALVE_5_ON_TIME = 0;
		timer.ID_VALVE_5_OFF_DELAY = 0;
		
        VALVE_6 = Off;
		op.ID_VALVE_6 = Off;
		timer.ID_VALVE_6_ON_TIME = 0;
		timer.ID_VALVE_6_OFF_DELAY = 0;
		
        VALVE_7 = Off;
		op.ID_VALVE_7 = Off;
		
        VALVE_8 = Off;
		op.ID_VALVE_8 = Off;
		
		op.ID_VALVE_AIR_LIFTER_INDEX = 0;
		
		ACTUATOR_1 = Off;
		op.ID_ACTUATOR_1 = Off;
		timer.ID_ACTUATOR_1_OP_TIME = 0;
		
		FAN_1 = Off;
		op.ID_FAN_1 = Off;
		timer.ID_FAN_1_OP_TIME = 0;
		
        FAN_2 = Off;
		op.ID_FAN_2 = Off;
		timer.ID_FAN_2_OP_TIME = 0;
		
        FAN_3 = Off;
		op.ID_FAN_3 = Off;
		
		PTC_FAN_1 = Off;
        PTC_FAN_2 = Off;
        PTC_FAN_3 = Off;
        PTC_FAN_4 = Off;
        PTC_FAN_5 = Off;
        PTC_FAN_6 = Off;
        op.ID_PTC_FAN = Off;
		timer.ID_PTC_FAN_OP_TIME = 0;
		
		HEATER_1 = Off;
		op.ID_HEATER_1 = Off;
        timer.ID_HEATER_1_OP_TIME = 0;
        timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
        timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME_2 = 0;
		
		RADIATOR = Off;
		op.ID_RADIATOR = Off;
		timer.ID_RADIATOR_DELAY_TIME = 0;
		
		LAMP_UV = Off;
		op.ID_UV_LAMP = Off;
		timer.ID_UV_LAMP_OFF_DELAY = 0;
		
		LAMP_EMERGENCY = On;
        op.ID_SIGNAL_INDICATOR |= (uint16_t)(1u << 4);
        op.ID_SWITCH |= BIT3;
	}
	else
	{
		/*set_operation_mode(ALL_Auto);
	
		op.ID_AERATION_2 = On;
		op.ID_MOTOR_9 = On;
		op.ID_FAN_3 = On;
		op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_3_ON_TIME;
		
		valve_1_scheduler_init();
		pump_3_scheduler_init();
		water_pump_1_scheduler_init();
		water_pump_2_scheduler_init();
		water_pump_5_scheduler_init();
		motor_8_scheduler_init();*/
		
		LAMP_EMERGENCY = Off;
		op.ID_SIGNAL_INDICATOR &= ~(uint16_t)(1u << 4);
        op.ID_SWITCH &= ~BIT3;
	}
}

void uv_lamp_handler(void)
{
	static bool pump_1_was_on = false;
	
	if(op.ID_UV_LAMP == Off)
	{
		if(level_sensor[L_FILTER].state == Enable)
		{
			LAMP_UV = On;
			
			op.ID_UV_LAMP = On;
			
			pump_1_was_on = false;
		}
	}
	else
	{
		if(op.ID_PUMP_1 == On)
        {
            pump_1_was_on = true;
			
            timer.ID_UV_LAMP_OFF_DELAY = 0;
        }
		
		if((pump_1_was_on == true) && (op.ID_PUMP_1 == Off))
		{
			if(sp.ID_UV_LAMP_OFF_DELAY > 0)
			{
				if(++timer.ID_UV_LAMP_OFF_DELAY >= sp.ID_UV_LAMP_OFF_DELAY)
				{
					timer.ID_UV_LAMP_OFF_DELAY = 0;
					
					LAMP_UV = Off;
					
					op.ID_UV_LAMP = Off;
				}
			}
			else
			{
				LAMP_UV = Off;
				
				op.ID_UV_LAMP = Off;
			}
		}
		else
		{
			timer.ID_UV_LAMP_OFF_DELAY = 0;
		}
	}
}

void temp_up_limit_time_handler(void)
{
	if(op.ID_HEATER_1 == On)
	{
		if(alert_sensor[TEMP_4].state == Disable)
		{
			if(++timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME >= sp.ID_HEATER_1_TEMP_UP_LIMIT_TIME)
			{
				timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
				
				if(op.ID_HEATER_1 == On)
				{
					timer.ID_HEATER_1_OP_TIME = 0;
					
					HEATER_1 = Off;
					
					op.ID_HEATER_1 = Off;
				}
			}
		}
		else
		{
			timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME = 0;
		}
	}
	
	if((op.ID_MOTOR_4 != Off) || (op.ID_MOTOR_5_DIR != Off) || (op.ID_MOTOR_6 != Off))
	{
		if(alert_sensor[TEMP_1].state == Disable)
		{
			if(++timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME_2 >= sp.ID_HEATER_1_TEMP_UP_LIMIT_TIME)
			{
				timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME_2 = 0;
				
				if(op.ID_MOTOR_4 != Off)
				{
					timer.ID_MOTOR_4_OP_TIME = 0;
					timer.ID_MOTOR_4_DIR_TIME = 0;
					
					MOTOR_4 = Off;
				
					op.ID_MOTOR_4 = Off;
					op.ID_MOTOR_4_DIR = 0;
				}
				
				if(op.ID_MOTOR_5_DIR != Off)
				{
					timer.ID_MOTOR_5_OP_TIME = 0;
					timer.ID_MOTOR_5_DIR_TIME = 0;
					
					MOTOR_5 = Off;
				
					op.ID_MOTOR_5 = Off;
					op.ID_MOTOR_5_DIR = 0;
				}
				
				if(op.ID_MOTOR_6 != Off)
				{
					timer.ID_MOTOR_6_OP_TIME = 0;
					timer.ID_MOTOR_6_DIR_TIME = 0;
					
					MOTOR_6 = Off;
				
					op.ID_MOTOR_6 = Off;
					op.ID_MOTOR_6_DIR = 0;
				}
			}
		}
		else
		{
			timer.ID_HEATER_1_TEMP_UP_LIMIT_TIME_2 = 0;
		}
	}
}

void set_operation_mode(uint16_t mode)
{
	DEV_OPERATION(op.ID_DEV_OPERATION, mode);

    if(mode == ALL_Auto)
    {
        LAMP_AUTO_START = On;
		
        op.ID_SIGNAL_INDICATOR |=  (uint16_t)(1u << 5);
    }
    else
    {
        LAMP_AUTO_START = Off;
		
        op.ID_SIGNAL_INDICATOR &= ~(uint16_t)(1u << 5);
    }
}

void auto_handler(void)
{
	
	if((op.ID_DEV_OPERATION & WP_Status) != Manual)
	{
		// FILTER & REUSE
		valve_1_handler();
		pump_1_handler();
		uv_lamp_handler();
		aeration_3_handler();
		pump_3_handler();
		valve_7_handler();
		valve_8_handler();
		aeration_1_handler();
		aeration_2_handler();
		
		// BIO TANK
		water_pump_1_handler();
		water_pump_2_handler();
		water_pump_3_handler();
		valve_3_handler();
		valve_4_handler();
		valve_5_handler();
		valve_6_handler();
		motor_8_handler();
		motor_9_handler();
		water_pump_5_handler();
	}
	
	if((op.ID_DEV_OPERATION & SD_Status) != Manual)
	{
		// SQUEEZE & DRY
		valve_2_handler();
		motor_1_handler();
		fan_1_handler();
		fan_2_handler();
		ptc_fan_handler();
		motor_2_handler();
		actuator_1_handler();
	}
	
	if((op.ID_DEV_OPERATION & IM_Status) != Manual)
	{
		// INDUCTION MODULE
		motor_3_handler();
		heater_1_handler();
		pump_2_handler();
		motor_4_handler();
		motor_5_handler();
		motor_6_handler();
		motor_7_handler();
		temp_up_limit_time_handler();
	}
	

	radiator_handler();
	fan_3_handler();
	water_pump_4_handler();
}

void Main_Process(void)
{
	Event_Handler();
}

void boot_loader(void)
{
    SYS_UnlockReg();
    
    FMC_Open();
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;
	
    FMC->ISPCMD = FMC_ISPCMD_VECMAP;
    FMC->ISPADDR = FMC_LDROM_BASE;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
	
    while (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk);
	
    SYS->RSTSTS = (SYS_RSTSTS_PORF_Msk | SYS_RSTSTS_PINRF_Msk);
	
    NVIC_SystemReset();
	
    while(1);
}
