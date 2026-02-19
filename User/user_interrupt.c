#include "user_common.h"

void SysTick_Handler(void)
{
    timer.systick++;
}

uint32_t getSysTick_ms(void)
{
	return timer.systick;
}

void ADC_IRQHandler(void)
{
    if(ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) 
    {
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
		
        ct_sensor.adc[0] = ADC_GET_CONVERSION_DATA(ADC, 5);
        ct_sensor.adc[1] = ADC_GET_CONVERSION_DATA(ADC, 6);
        ct_sensor.adc[2] = ADC_GET_CONVERSION_DATA(ADC, 7);
        ct_sensor.adc[3] = ADC_GET_CONVERSION_DATA(ADC, 8);
        ct_sensor.adc[4] = ADC_GET_CONVERSION_DATA(ADC, 9);
		
		ct_sensor.adc_done_flag = true;
		
		run_event(STATUS_CT_SENSOR, 0); // ct_sensor_handler
    }
}


void GPAB_IRQHandler(void)
{
    if(GPIO_GET_INT_FLAG(PA, BIT12)) // SENSOR_LEVEL_H_FILTER
    {
        GPIO_CLR_INT_FLAG(PA, BIT12);
		
		if(level_sensor[H_FILTER].flag == false)
		{
			level_sensor[H_FILTER].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PA, BIT13)) // SENSOR_LEVEL_L_FILTER
    {
        GPIO_CLR_INT_FLAG(PA, BIT13);
		
		if(level_sensor[L_FILTER].flag == false)
		{
			level_sensor[L_FILTER].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PA, BIT15)) // SENSOR_LEVEL_LL_FILTER
    {
        GPIO_CLR_INT_FLAG(PA, BIT15);
		
		if(level_sensor[LL_FILTER].flag == false)
		{
			level_sensor[LL_FILTER].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PB, BIT0)) // SENSOR_LEVEL_L_EQ
    {
        GPIO_CLR_INT_FLAG(PB, BIT0);
		
		if(level_sensor[L_EQ].flag == false)
		{
			level_sensor[L_EQ].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PB, BIT1)) // SENSOR_LEVEL_L_S2
    {
        GPIO_CLR_INT_FLAG(PB, BIT1);
		
		if(level_sensor[L_S2].flag == false)
		{
			level_sensor[L_S2].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PB, BIT2)) // SENSOR_LEVEL_L_REUSE
    {
        GPIO_CLR_INT_FLAG(PB, BIT2);
		
		if(level_sensor[L_REUSE].flag == false)
		{
			level_sensor[L_REUSE].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PB, BIT3)) // SENSOR_TEMP_1
    {
        GPIO_CLR_INT_FLAG(PB, BIT3);
		
		if(alert_sensor[TEMP_1].flag == false)
		{
			alert_sensor[TEMP_1].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PB, BIT4)) // SENSOR_TEMP_2
    {
        GPIO_CLR_INT_FLAG(PB, BIT4);
		
		if(alert_sensor[TEMP_2].flag == false)
		{
			alert_sensor[TEMP_2].flag = true;
		}
    }
}

void GPCDEF_IRQHandler(void)
{
	if(GPIO_GET_INT_FLAG(PC, BIT9)) // MOTOR_7_FG
    {
		GPIO_CLR_INT_FLAG(PC, BIT9);
		
		motor_7_watchdog = 0;
		
		uint32_t motor_7_current_count = TIMER_GetCounter(TIMER0);
		uint32_t diff;
		
		if(motor_7_current_count >= motor_7_fg_last_count)
        {
            diff = motor_7_current_count - motor_7_fg_last_count;
        }
        else
        {
            diff = (0xFFFFFF - motor_7_fg_last_count) + motor_7_current_count + 1;
        }
		
		motor_7_fg_last_count = motor_7_current_count;
		
		if(diff > 0)
			op.ID_MOTOR_7_FG = (60 * 1000000) / (diff * 2);
    }
	
	if(GPIO_GET_INT_FLAG(PD, BIT1)) // SW_AUTO_START
    {
        GPIO_CLR_INT_FLAG(PD, BIT1);
		
		if(user_switch[AUTO_START].flag == false)
		{
			user_switch[AUTO_START].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PD, BIT2)) // SW_SPARE_1
    {
        GPIO_CLR_INT_FLAG(PD, BIT2);
		
		if(user_switch[SPARE_1].flag == false)
		{
			user_switch[SPARE_1].flag = true;
		}
    }
	
    if(GPIO_GET_INT_FLAG(PD, BIT3)) // SW_AUTO_STOP
    {
        GPIO_CLR_INT_FLAG(PD, BIT3);
		
		if(user_switch[AUTO_STOP].flag == false)
		{
			user_switch[AUTO_STOP].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PD, BIT4)) // SW_FAUCET
    {
        GPIO_CLR_INT_FLAG(PD, BIT4);
		
		if(user_switch[FAUCET].flag == false)
		{
			user_switch[FAUCET].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PD, BIT14)) // SENSOR_TEMP_3
    {
        GPIO_CLR_INT_FLAG(PD, BIT14);
		
		if(alert_sensor[TEMP_3].flag == false)
		{
			alert_sensor[TEMP_3].flag = true;
		}
    }
	
	if(GPIO_GET_INT_FLAG(PF, BIT2)) // SW_EMERGENCY
    {
        GPIO_CLR_INT_FLAG(PF, BIT2);
		
		if(user_switch[EMERGENCY].flag == false)
		{
			user_switch[EMERGENCY].flag = true;
		}
    }
}

void EINT3_IRQHandler(void)
{
    if(GPIO_GET_INT_FLAG(PD, BIT0)) // SENSOR_ZERO_CROSSING
    {
        GPIO_CLR_INT_FLAG(PD, BIT0);
		
		if(zero_crossing.flag == false)
		{
			zero_crossing.flag = true;
		}
    }
}

void TMR0_IRQHandler(void) // 1us timer
{
    /* Check if the interrupt occurred */
    if(TIMER_GetIntFlag(TIMER0))
    {
        /* Clear TIMER0 interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
    }
}

void TMR1_IRQHandler(void) // 1ms timer
{
    /* Check if the interrupt occurred */
    if(TIMER_GetIntFlag(TIMER1))
    {
        /* Clear TIMER0 interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
		
		timer_interrupt_handler();
		input_interrupt_handler();
    }
}

void TMR2_IRQHandler(void) // 100ms
{
    /* Check if the interrupt occurred */
    if(TIMER_GetIntFlag(TIMER2))
    {
        /* Clear TIMER2 interrupt flag */
        TIMER_ClearIntFlag(TIMER2);
		
		if(registers_nmbs_server_write_flag == Off)
		{
			run_event(STATUS_MODBUS_SERVER_UPDATE, 0); // modbus_server_update_handler
		}
		
		if(motor_7_watchdog++ >= MOTOR_7_RPM_TIMEOUT)
		{
			op.ID_MOTOR_7_FG = 0;
			motor_7_watchdog = MOTOR_7_RPM_TIMEOUT;
		}
    }
}

void TMR3_IRQHandler(void) // 1s
{
    /* Check if the interrupt occurred */
    if(TIMER_GetIntFlag(TIMER3))
    {
        /* Clear TIMER3 interrupt flag */
        TIMER_ClearIntFlag(TIMER3);
		/*if(ct_sensor.adc_done_flag == false)
		{
			ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
			ADC_START_CONV(ADC);
		}*/
    }
}

void RTC_IRQHandler(void)
{
	if(RTC_GET_TICK_INT_FLAG() == 1)
    {
        RTC_CLEAR_TICK_INT_FLAG();
		
		RTC_GetDateAndTime(&rtc);
		
		run_event(STATUS_AUTO, 0);
    }
}

void WDT_IRQHandler(void)
{
    if(WDT_GET_TIMEOUT_INT_FLAG() == 1)
    {
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG();
    }

    if(WDT_GET_TIMEOUT_WAKEUP_FLAG() == 1)
    {
        /* Clear WDT time-out wake-up flag */
        WDT_CLEAR_TIMEOUT_WAKEUP_FLAG();
    }
}