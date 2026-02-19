#include "user_common.h"

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
	
	/* Enable clock source */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk|CLK_PWRCTL_HIRCEN_Msk);
	
	/* Waiting for clock source ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk|CLK_STATUS_HIRCSTB_Msk);

    /* Set core clock */
    CLK_SetCoreClock(FREQ_72MHZ);

    /* Enable IP clock */
    //CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);
	CLK_EnableModuleClock(UART1_MODULE);
    CLK_EnableModuleClock(UART2_MODULE);
	CLK_EnableModuleClock(RTC_MODULE);
    CLK_EnableModuleClock(WDT_MODULE);
    CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, 0);

    /* Set IP clock */
    //CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADCSEL_PCLK0, CLK_CLKDIV0_ADC(1));
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL1_PWM1SEL_PCLK1, MODULE_NoMsk);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, MODULE_NoMsk);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, MODULE_NoMsk);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_PCLK1, MODULE_NoMsk);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_LIRC, MODULE_NoMsk);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_HIRC, CLK_CLKDIV0_UART(1));
	CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UARTSEL_HIRC, CLK_CLKDIV0_UART(1));
    CLK_SetModuleClock(UART2_MODULE, CLK_CLKSEL1_UARTSEL_HIRC, CLK_CLKDIV0_UART(1));
	CLK_SetModuleClock(RTC_MODULE, CLK_CLKSEL2_RTCSEL_LIRC, MODULE_NoMsk);
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, MODULE_NoMsk);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();
	
	if(SysTick_Config(SystemCoreClock / 1000))
    {
        while (1);
    }
	
	SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE7MFP_Msk | SYS_GPE_MFPL_PE6MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE7MFP_ICE_DAT | SYS_GPE_MFPL_PE6MFP_ICE_CLK);
}

void GPIO_Init(void)
{
	SYS->GPA_MFPH = SYS_GPA_MFPH_PA15MFP_GPIO | SYS_GPA_MFPH_PA14MFP_GPIO | SYS_GPA_MFPH_PA13MFP_GPIO | SYS_GPA_MFPH_PA12MFP_GPIO | SYS_GPA_MFPH_PA11MFP_GPIO | SYS_GPA_MFPH_PA10MFP_GPIO | SYS_GPA_MFPH_PA9MFP_GPIO | SYS_GPA_MFPH_PA8MFP_GPIO;
    SYS->GPA_MFPL = SYS_GPA_MFPL_PA7MFP_GPIO | SYS_GPA_MFPL_PA6MFP_GPIO | SYS_GPA_MFPL_PA5MFP_GPIO | SYS_GPA_MFPL_PA4MFP_GPIO | SYS_GPA_MFPL_PA3MFP_GPIO | SYS_GPA_MFPL_PA0MFP_GPIO;
	
    SYS->GPB_MFPH = SYS_GPB_MFPH_PB15MFP_GPIO | SYS_GPB_MFPH_PB14MFP_GPIO | SYS_GPB_MFPH_PB13MFP_GPIO | SYS_GPB_MFPH_PB12MFP_GPIO;
    SYS->GPB_MFPL = SYS_GPB_MFPL_PB7MFP_GPIO | SYS_GPB_MFPL_PB6MFP_GPIO | SYS_GPB_MFPL_PB5MFP_GPIO | SYS_GPB_MFPL_PB4MFP_GPIO | SYS_GPB_MFPL_PB3MFP_GPIO | SYS_GPB_MFPL_PB2MFP_GPIO | SYS_GPB_MFPL_PB1MFP_GPIO | SYS_GPB_MFPL_PB0MFP_GPIO;
	
    SYS->GPC_MFPH = SYS_GPC_MFPH_PC15MFP_GPIO | SYS_GPC_MFPH_PC14MFP_GPIO | SYS_GPC_MFPH_PC13MFP_GPIO | SYS_GPC_MFPH_PC12MFP_GPIO | SYS_GPC_MFPH_PC11MFP_GPIO | SYS_GPC_MFPH_PC9MFP_GPIO | SYS_GPC_MFPH_PC8MFP_GPIO;
    SYS->GPC_MFPL = SYS_GPC_MFPL_PC7MFP_GPIO | SYS_GPC_MFPL_PC6MFP_GPIO | SYS_GPC_MFPL_PC5MFP_GPIO | SYS_GPC_MFPL_PC4MFP_GPIO | SYS_GPC_MFPL_PC3MFP_GPIO | SYS_GPC_MFPL_PC2MFP_GPIO | SYS_GPC_MFPL_PC1MFP_GPIO | SYS_GPC_MFPL_PC0MFP_GPIO;
	
    SYS->GPD_MFPH = SYS_GPD_MFPH_PD15MFP_GPIO | SYS_GPD_MFPH_PD14MFP_GPIO | SYS_GPD_MFPH_PD11MFP_GPIO | SYS_GPD_MFPH_PD10MFP_GPIO | SYS_GPD_MFPH_PD9MFP_GPIO | SYS_GPD_MFPH_PD8MFP_GPIO;
    SYS->GPD_MFPL = SYS_GPD_MFPL_PD7MFP_GPIO | SYS_GPD_MFPL_PD5MFP_GPIO | SYS_GPD_MFPL_PD4MFP_GPIO | SYS_GPD_MFPL_PD3MFP_GPIO | SYS_GPD_MFPL_PD2MFP_GPIO | SYS_GPD_MFPL_PD1MFP_GPIO | SYS_GPD_MFPL_PD0MFP_INT3;
	
    SYS->GPE_MFPH = SYS_GPE_MFPH_PE13MFP_GPIO | SYS_GPE_MFPH_PE12MFP_GPIO | SYS_GPE_MFPH_PE11MFP_GPIO | SYS_GPE_MFPH_PE10MFP_GPIO | SYS_GPE_MFPH_PE9MFP_GPIO | SYS_GPE_MFPH_PE8MFP_GPIO;
    SYS->GPE_MFPL = SYS_GPE_MFPL_PE5MFP_GPIO | SYS_GPE_MFPL_PE4MFP_GPIO | SYS_GPE_MFPL_PE1MFP_GPIO | SYS_GPE_MFPL_PE0MFP_GPIO;
	
    SYS->GPF_MFPL = SYS_GPF_MFPL_PF7MFP_GPIO | SYS_GPF_MFPL_PF6MFP_GPIO | SYS_GPF_MFPL_PF5MFP_GPIO | SYS_GPF_MFPL_PF4MFP_GPIO | SYS_GPF_MFPL_PF3MFP_GPIO | SYS_GPF_MFPL_PF2MFP_GPIO /*| SYS_GPF_MFPL_PF1MFP_X32_IN | SYS_GPF_MFPL_PF0MFP_X32_OUT*/;
}

void ADC0_Init(void)
{
	GPIO_DISABLE_DIGITAL_PATH(PB, BIT8 | BIT9 | BIT10 | BIT11);
	GPIO_DISABLE_DIGITAL_PATH(PE, BIT2);
	
	SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB11MFP_Msk | SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk | SYS_GPB_MFPH_PB8MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB11MFP_ADC0_CH8 | SYS_GPB_MFPH_PB10MFP_ADC0_CH7 | SYS_GPB_MFPH_PB9MFP_ADC0_CH6 | SYS_GPB_MFPH_PB8MFP_ADC0_CH5);
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE2MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE2MFP_ADC0_CH9);
}

void PWM1_Init(void)
{
	SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC10MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC10MFP_PWM1_CH1);
	
	SYS_ResetModule(PWM1_RST);
}

void UART0_Init(void)
{
	SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD13MFP_Msk | SYS_GPD_MFPH_PD12MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD13MFP_UART0_RXD | SYS_GPD_MFPH_PD12MFP_UART0_TXD);
}

void UART2_Init(void)
{
	SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD6MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD6MFP_UART2_TXD);
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE3MFP_Msk);
    SYS->GPE_MFPL |= (SYS_GPE_MFPL_PE3MFP_UART2_RXD);
}

void TIMER0_Init(void)
{
	TIMER_Open(TIMER0, TIMER_CONTINUOUS_MODE, 1000000); // 1us
	TIMER_SET_PRESCALE_VALUE(TIMER0, 71);
	TIMER_Start(TIMER0);
}

void TIMER1_Init(void)
{
	TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000); // 1ms
	TIMER_EnableInt(TIMER1);
	NVIC_EnableIRQ(TMR1_IRQn);
	TIMER_Start(TIMER1);
	NVIC_SetPriority(TMR1_IRQn, TIMER_1);
}

void TIMER2_Init(void)
{
	TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 10); // 100ms
	TIMER_EnableInt(TIMER2);
	NVIC_EnableIRQ(TMR2_IRQn);
	TIMER_Start(TIMER2);
	NVIC_SetPriority(TMR2_IRQn, TIMER_2);
}

void TIMER3_Init(void)
{
	TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1); // 1s
	TIMER_EnableInt(TIMER3);
	NVIC_EnableIRQ(TMR3_IRQn);
	TIMER_Start(TIMER3);
	NVIC_SetPriority(TMR3_IRQn, TIMER_3);
}

void RTC_Init(void)
{
	S_RTC_TIME_DATA_T sWriteRTC;
	
	sWriteRTC.u32Year       = RTC_YEAR2000 + BCD_TO_DEC((sp.ID_RTC_DATE >> 24));
    sWriteRTC.u32Month      = BCD_TO_DEC((sp.ID_RTC_DATE >> 16));
    sWriteRTC.u32Day        = BCD_TO_DEC((sp.ID_RTC_DATE >> 8));
    sWriteRTC.u32DayOfWeek  = BCD_TO_DEC((sp.ID_RTC_DATE >> 0));
    sWriteRTC.u32Hour       = BCD_TO_DEC((sp.ID_RTC_TIME >> 16));
    sWriteRTC.u32Minute     = BCD_TO_DEC((sp.ID_RTC_TIME >> 8));
    sWriteRTC.u32Second     = BCD_TO_DEC((sp.ID_RTC_TIME >> 0));
    sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
	
    while(RTC_Open(&sWriteRTC));
	
	RTC_EnableInt(RTC_INTEN_TICKIEN_Msk);
    RTC_SetTickPeriod(RTC_TICK_1_SEC);
	NVIC_EnableIRQ(RTC_IRQn);
}

void WDT_Init(void)
{
	if(WDT_GET_RESET_FLAG() == 1)
    {
        WDT_CLEAR_RESET_FLAG();
    }
	
	NVIC_EnableIRQ(WDT_IRQn);
	WDT_Open(WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, TRUE, TRUE);
	WDT_EnableInt();
}

void Blower_Init(void)
{
	op.ID_MOTOR_7 = Off;
	op.ID_MOTOR_7_OUTPUT = MOTOR_7_RPM(0);
	
	GPIO_SetMode(PC, BIT9, GPIO_MODE_INPUT); // MOTOR_7_FG
	GPIO_EnableInt(PC, 9, GPIO_INT_FALLING);
	GPIO_DISABLE_DEBOUNCE(PC, BIT9);
	
	PWM_ConfigOutputChannel(PWM1, 1, MOTOR_7_FREQ, op.ID_MOTOR_7_OUTPUT);
	PWM_EnableOutput(PWM1, 2);
	PWM_Start(PWM1, 2); // MOTOR_7
}

void Zero_Crossing_Init(void)
{
	GPIO_SetMode(PD, BIT0, GPIO_MODE_INPUT);
    GPIO_EnableInt(PD, 0, GPIO_INT_BOTH_EDGE);
    NVIC_EnableIRQ(EINT135_IRQn);
	NVIC_SetPriority(EINT135_IRQn, TIMER_0);
	
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PD, BIT0);
}

void Level_Sensor_Init(void)
{
	GPIO_SetMode(PA, BIT12, GPIO_MODE_INPUT); // SENSOR_LEVEL_H_FILTER
	GPIO_EnableInt(PA, 12, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PA, BIT12);
	
	GPIO_SetMode(PA, BIT13, GPIO_MODE_INPUT); // SENSOR_LEVEL_L_FILTER
	GPIO_EnableInt(PA, 13, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PA, BIT13);
	
	GPIO_SetMode(PA, BIT15, GPIO_MODE_INPUT); // SENSOR_LEVEL_LL_FILTER
	GPIO_EnableInt(PA, 15, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PA, BIT15);
	
	GPIO_SetMode(PB, BIT0, GPIO_MODE_INPUT); // SENSOR_LEVEL_L_EQ
	GPIO_EnableInt(PB, 0, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PB, BIT0);
	
	GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT); // SENSOR_LEVEL_L_S2
	GPIO_EnableInt(PB, 1, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PB, BIT1);
	
	GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT); // SENSOR_LEVEL_L_REUSE
	GPIO_EnableInt(PB, 2, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PB, BIT2);
}

void Alert_Sensor_Init(void)
{
	GPIO_SetMode(PB, BIT3, GPIO_MODE_INPUT); // SENSOR_TEMP_1 AL1
	GPIO_EnableInt(PB, 3, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PB, BIT3);
	
	GPIO_SetMode(PB, BIT4, GPIO_MODE_INPUT); // SENSOR_TEMP_2 AL2
	GPIO_EnableInt(PB, 4, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PB, BIT4);
	
	GPIO_SetMode(PD, BIT14, GPIO_MODE_INPUT); // SENSOR_TEMP_3 OUT
	GPIO_EnableInt(PD, 14, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PD, BIT14);
	
	/*GPIO_SetMode(PD, BIT14, GPIO_MODE_INPUT); // SENSOR_TEMP_4 AL3
	GPIO_EnableInt(PD, 14, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PD, BIT14);*/
}

void Switch_Init(void)
{
	GPIO_SetMode(PD, BIT1, GPIO_MODE_INPUT); // SW_AUTO_START
	GPIO_EnableInt(PD, 1, GPIO_INT_RISING);
	GPIO_ENABLE_DEBOUNCE(PD, BIT1);
	
	GPIO_SetMode(PD, BIT2, GPIO_MODE_INPUT); // SW_SPARE_1
	GPIO_EnableInt(PD, 2, GPIO_INT_RISING);
	GPIO_ENABLE_DEBOUNCE(PD, BIT2);
	
	GPIO_SetMode(PD, BIT3, GPIO_MODE_INPUT); // SW_AUTO_STOP
	GPIO_EnableInt(PD, 3, GPIO_INT_RISING);
	GPIO_ENABLE_DEBOUNCE(PD, BIT3);
	
	GPIO_SetMode(PD, BIT4, GPIO_MODE_INPUT); // SW_FAUCET
	GPIO_EnableInt(PD, 4, GPIO_INT_RISING);
	GPIO_ENABLE_DEBOUNCE(PD, BIT4);
	
	GPIO_SetMode(PF, BIT2, GPIO_MODE_INPUT); // SW_EMERGENCY
	GPIO_EnableInt(PF, 2, GPIO_INT_BOTH_EDGE);
	GPIO_ENABLE_DEBOUNCE(PF, BIT2);
	
	NVIC_EnableIRQ(GPAB_IRQn);
	NVIC_SetPriority(GPAB_IRQn, GPAB);
	
	NVIC_EnableIRQ(GPCDEF_IRQn);
	NVIC_SetPriority(GPCDEF_IRQn, GPCDEF);
}

void CT_Sensor_Init(void)
{
	ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, ((1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9)));
	ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
	ADC_POWER_ON(ADC);
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	ADC_START_CONV(ADC);
	
	while(ct_sensor.adc_done_flag) // dummy read
	{
		ct_sensor.adc_done_flag = false;
	}
}

void Load_Output_Init(void)
{
	uint8_t on_off = 0;
	
	GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT); // VALVE_2
	GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT); // VALVE_3
	GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT); // PTC_FAN_1
	GPIO_SetMode(PA, BIT5, GPIO_MODE_OUTPUT); // PTC_FAN_2
	GPIO_SetMode(PA, BIT6, GPIO_MODE_OUTPUT); // PTC_FAN_3
	GPIO_SetMode(PA, BIT7, GPIO_MODE_OUTPUT); // PTC_FAN_4
	GPIO_SetMode(PA, BIT8, GPIO_MODE_OUTPUT); // FAN_1
	GPIO_SetMode(PA, BIT9, GPIO_MODE_OUTPUT); // FAN_2
	GPIO_SetMode(PA, BIT10, GPIO_MODE_OUTPUT); // VALVE_7
	GPIO_SetMode(PA, BIT11, GPIO_MODE_OUTPUT); // PTC_FAN_6
	GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT); // VALVE_1
	
	GPIO_SetMode(PB, BIT5, GPIO_MODE_OUTPUT); // RADIATOR
	GPIO_SetMode(PB, BIT6, GPIO_MODE_OUTPUT); // HEATER_1
	GPIO_SetMode(PB, BIT7, GPIO_MODE_OUTPUT); // LAMP_FAUCET
	GPIO_SetMode(PB, BIT12, GPIO_MODE_OUTPUT); // VALVE_4
	GPIO_SetMode(PB, BIT13, GPIO_MODE_OUTPUT); // MOTOR_2_DIR
	GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT); // MOTOR_2
	GPIO_SetMode(PB, BIT15, GPIO_MODE_OUTPUT); // MOTOR_1_DIR
	
	GPIO_SetMode(PC, BIT0, GPIO_MODE_OUTPUT); // LAMP_R
	GPIO_SetMode(PC, BIT1, GPIO_MODE_OUTPUT); // LAMP_EMERGENCY
	GPIO_SetMode(PC, BIT2, GPIO_MODE_OUTPUT); // LAMP_AUTO_STOP
	GPIO_SetMode(PC, BIT3, GPIO_MODE_OUTPUT); // LAMP_AUTO_START
	GPIO_SetMode(PC, BIT4, GPIO_MODE_OUTPUT); // PUMP_4
	GPIO_SetMode(PC, BIT5, GPIO_MODE_OUTPUT); // WATER_PUMP_2
	GPIO_SetMode(PC, BIT6, GPIO_MODE_OUTPUT); // WATER_PUMP_1
	GPIO_SetMode(PC, BIT7, GPIO_MODE_OUTPUT); // AERATION_3
	GPIO_SetMode(PC, BIT8, GPIO_MODE_OUTPUT); // WATER_PUMP_5
	GPIO_SetMode(PC, BIT11, GPIO_MODE_OUTPUT); // MOTOR_5
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT); // BUZZER
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT); // LAMP_G
	GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT); // LAMP_Y
	GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT); // VALVE_5
	
	GPIO_SetMode(PD, BIT7, GPIO_MODE_OUTPUT); // PUMP_2
	GPIO_SetMode(PD, BIT8, GPIO_MODE_OUTPUT); // LED_OPERATION
	GPIO_SetMode(PD, BIT10, GPIO_MODE_OUTPUT); // WATER_PUMP_4
	GPIO_SetMode(PD, BIT15, GPIO_MODE_OUTPUT); // WATER_PUMP_3
	
	GPIO_SetMode(PE, BIT0, GPIO_MODE_OUTPUT); // LAMP_UV
	GPIO_SetMode(PE, BIT1, GPIO_MODE_OUTPUT); // MOTOR_9
	GPIO_SetMode(PE, BIT4, GPIO_MODE_OUTPUT); // AERATION_2
	GPIO_SetMode(PE, BIT5, GPIO_MODE_OUTPUT); // AERATION_1
	GPIO_SetMode(PE, BIT8, GPIO_MODE_OUTPUT); // MOTOR_8
	GPIO_SetMode(PE, BIT9, GPIO_MODE_OUTPUT); // FAN_3
	GPIO_SetMode(PE, BIT10, GPIO_MODE_OUTPUT); // MOTOR_4
	GPIO_SetMode(PE, BIT11, GPIO_MODE_OUTPUT); // MOTOR_3
	GPIO_SetMode(PE, BIT12, GPIO_MODE_OUTPUT); // MOTOR_6
	GPIO_SetMode(PE, BIT13, GPIO_MODE_OUTPUT); // MOTOR_1
	
	GPIO_SetMode(PF, BIT3, GPIO_MODE_OUTPUT); // PUMP_1
	GPIO_SetMode(PF, BIT4, GPIO_MODE_OUTPUT); // ACTUATOR_1
	GPIO_SetMode(PF, BIT5, GPIO_MODE_OUTPUT); // PTC_FAN_5
	GPIO_SetMode(PF, BIT6, GPIO_MODE_OUTPUT); // VALVE_6
	GPIO_SetMode(PF, BIT7, GPIO_MODE_OUTPUT); // VALVE_8
	
	MOTOR_1 = on_off;
	MOTOR_1_DIR = on_off;
	MOTOR_2 = on_off;
	MOTOR_2_DIR = on_off;
	MOTOR_3 = on_off;
	MOTOR_4 = on_off;
	MOTOR_5 = on_off;
	MOTOR_6 = on_off;
	
	PUMP_1 = on_off;
	PUMP_2 = on_off;
	PUMP_3 = on_off;
	
	WATER_PUMP_1 = on_off;
	WATER_PUMP_2 = on_off;
	WATER_PUMP_3 = on_off;
	WATER_PUMP_4 = on_off;
	WATER_PUMP_5 = on_off;
	
	AERATION_1 = on_off;
	AERATION_2 = on_off;
	AERATION_3 = on_off;
	
	VALVE_1 = on_off;
	VALVE_2 = on_off;
	VALVE_3 = on_off;
	VALVE_4 = on_off;
	VALVE_5 = on_off;
	VALVE_6 = on_off;
	VALVE_7 = on_off;
	VALVE_8 = on_off;
	
	ACTUATOR_1 = on_off;
	
	FAN_1 = on_off;
	FAN_2 = on_off;
	FAN_3 = on_off;
	
	PTC_FAN_1 = on_off;
	PTC_FAN_2 = on_off;
	PTC_FAN_3 = on_off;
	PTC_FAN_4 = on_off;
	PTC_FAN_5 = on_off;
	PTC_FAN_6 = on_off;
	
	LAMP_R = on_off;
	LAMP_Y = on_off;
	LAMP_G = on_off;
	
	LAMP_AUTO_START = on_off;
	LAMP_AUTO_STOP = on_off;
	LAMP_FAUCET = on_off;
	LAMP_EMERGENCY = on_off;
	
	LAMP_UV = on_off;
	
	LED_OPERATION ^= on_off;
	
	HEATER_1 = on_off;
	
	BUZZER = on_off;
	
	RADIATOR = on_off;
}

void Device_Init(void)
{
	for(int i = 0; i < LEVEL_SENSOR_MAX; i++)
	{
		if(LEVEL_SENSOR(i) == Enable)
		{
			level_sensor[i].state = Enable;
			
			run_event(STATUS_LEVEL_SENSOR, i); // level_sensor_handler
		}
	}
	
	for(int i = 0; i < ALERT_SENSOR_MAX; i++)
	{
		if(ALERT_SWITCH(i) == Enable)
		{
			alert_sensor[i].state = Enable;
			
			run_event(STATUS_ALERT_SENSOR, i); // alert_sensor_handler
		}
	}
	
	for(int i = 0; i < USER_SWITCH_MAX; i++)
	{
		if(USER_SWITCH(i) == On)
		{
			user_switch[i].state = On;
			
			run_event(STATUS_USER_SWITCH, i); // user_switch_handler
		}
	}
	
	set_operation_mode(ALL_Auto);
	
	op.ID_AERATION_2 = On;
	op.ID_MOTOR_9 = On;
	op.ID_FAN_3 = On;
	op.ID_VALVE_AIR_LIFTER_INDEX = ID_VALVE_3_ON_TIME;
	
	valve_1_scheduler_init();
	pump_3_scheduler_init();
	water_pump_1_scheduler_init();
	water_pump_2_scheduler_init();
	water_pump_5_scheduler_init();
	motor_8_scheduler_init();
}

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
	
    SYS_Init();
	GPIO_Init();
	ADC0_Init();
	PWM1_Init();
	UART0_Init();
	UART2_Init();
	TIMER0_Init();
	TIMER1_Init();
	TIMER2_Init();
	TIMER3_Init();
	//WDT_Init();
	
    /* Lock protected registers */
    SYS_LockReg();
	
	Event_Init();
	eeprom_Read();
	//Flash_Init();
	RTC_Init();
	Blower_Init();
	Zero_Crossing_Init();
	Level_Sensor_Init();
	Alert_Sensor_Init();
	Switch_Init();
	//CT_Sensor_Init();
	Load_Output_Init();
	ModbusRTU_Init();
	Device_Init();
	
	while(1)
	{
		Main_Process();
		ModbusRTU_Process();
		//WDT_RESET_COUNTER();
	}
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
