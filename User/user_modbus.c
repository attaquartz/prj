#include "user_common.h"

nmbs_platform_conf platform_conf_client;
nmbs_platform_conf platform_conf_server;

nmbs_callbacks callbacks_server;

nmbs_t nmbs_client;
nmbs_t nmbs_server;

nmbs_error err_nmbs_client;
nmbs_error err_nmbs_server;

nmbs_bitfield coils_nmbs_server = {0};

slave_data_t environment_sensor[ENVIRONMENT_SENSOR_MAX];
slave_data_t power_meter[POWER_METER_MAX];
slave_data_t temperature_controller[TEMPERATURE_CONTROLLER_MAX];

uint16_t registers_nmbs_server[REGS_ADDR_MAX + 1] = {0};
uint16_t registers_nmbs_system_parameter[REGS_SYSTEM_PARAMETER_ADDR_MAX + 1] = {0};

uint8_t registers_nmbs_server_write_flag = 0;
uint32_t last_poll_time = 0;
uint8_t current_poll_id = 0;

int32_t read_uart0(uint8_t *buf, uint16_t count, int32_t timeout_ms, void *arg) 
{
	int32_t read_count = 0;
    uint32_t start_tick = getSysTick_ms();
	
	while(read_count < count)
	{
        if(!UART_IS_RX_READY(PC_UART))
		{
            if(timeout_ms >= 0)
			{
                if((getSysTick_ms() - start_tick) > (uint32_t)timeout_ms)
				{
                    return read_count;
                }
            }
            continue;
        }
		
		buf[read_count++] = UART_READ(PC_UART);
		
		if(timeout_ms >= 0)
		{
            start_tick = getSysTick_ms();
        }
	}
	
	return read_count;
}

int32_t write_uart0(const uint8_t *buf, uint16_t count, int32_t timeout_ms, void *arg)
{
	PC_RE_DE = 1;
	
	for(int i = 0; i < count; i++)
	{
        while(UART_IS_TX_FULL(PC_UART));
        UART_WRITE(PC_UART, buf[i]);
    }
	
	while(!UART_IS_TX_EMPTY(PC_UART));
	
	PC_RE_DE = 0;
	
	return count;
}

int32_t read_uart2(uint8_t *buf, uint16_t count, int32_t timeout_ms, void *arg) 
{
	int32_t read_count = 0;
    uint32_t start_tick = getSysTick_ms();
	
	while(read_count < count)
	{
        if(!UART_IS_RX_READY(HMI_UART))
		{
            if(timeout_ms >= 0)
			{
                if((getSysTick_ms() - start_tick) > (uint32_t)timeout_ms)
				{
                    return read_count;
                }
            }
            continue;
        }
		
		buf[read_count++] = UART_READ(HMI_UART);
		
		if(timeout_ms >= 0)
		{
            start_tick = getSysTick_ms();
        }
	}
	
	return read_count;
}

int32_t write_uart2(const uint8_t *buf, uint16_t count, int32_t timeout_ms, void *arg)
{
	HMI_RE_DE = 1;
	
	for(int i = 0; i < count; i++)
	{
        while(UART_IS_TX_FULL(HMI_UART));
        UART_WRITE(HMI_UART, buf[i]);
    }
	
	while(!UART_IS_TX_EMPTY(HMI_UART));
	
	HMI_RE_DE = 0;
	
	timer.modbus_server_tick = 0;
	op.ID_DEV_STATE |= BIT0;
	
	return count;
}

nmbs_error handle_read_coils_nmbs_server(uint16_t address, uint16_t quantity, nmbs_bitfield coils_out, uint8_t unit_id, void* arg)
{
    if(address + quantity > COILS_ADDR_MAX + 1)
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
    for(int i = 0; i < quantity; i++)
	{
        bool value = nmbs_bitfield_read(coils_nmbs_server, address + i);
        nmbs_bitfield_write(coils_out, i, value);
    }
	
    return NMBS_ERROR_NONE;
}

nmbs_error handle_write_multiple_coils_nmbs_server(uint16_t address, uint16_t quantity, const nmbs_bitfield coils, uint8_t unit_id, void* arg)
{
    if(address + quantity > COILS_ADDR_MAX + 1)
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
    for(int i = 0; i < quantity; i++)
	{
        nmbs_bitfield_write(coils_nmbs_server, address + i, nmbs_bitfield_read(coils, i));
    }
	
	registers_nmbs_server_write_flag = On;
	
    return NMBS_ERROR_NONE;
}

nmbs_error handler_read_holding_registers_nmbs_server(uint16_t address, uint16_t quantity, uint16_t* registers_out, uint8_t unit_id, void* arg)
{
    if(address + quantity > REGS_ADDR_MAX + 1)
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
	if (address < REGS_SYSTEM_PARAMETER_ADDR && address + quantity > REGS_SYSTEM_PARAMETER_ADDR) 
		return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
	if(address >= REGS_SYSTEM_PARAMETER_ADDR)
	{
		uint32_t system_parameter_index = address - REGS_SYSTEM_PARAMETER_ADDR;
		
		 if(system_parameter_index + quantity > REGS_SYSTEM_PARAMETER_ADDR_MAX + 1)
			 return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
		 
		 for(int i = 0; i < quantity; i++)
            registers_out[i] = registers_nmbs_system_parameter[system_parameter_index + i];
	}
	else
	{
		for(int i = 0; i < quantity; i++)
			registers_out[i] = registers_nmbs_server[address + i];
	}
	
    return NMBS_ERROR_NONE;
}

nmbs_error handler_write_single_registers_nmbs_server(uint16_t address, uint16_t value, uint8_t unit_id, void *arg)
{
	if(address >= REGS_SYSTEM_PARAMETER_ADDR)
		return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
    if(address >= REGS_ADDR_MAX)
	{
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }
	
    registers_nmbs_server[address] = value;
	registers_nmbs_server_write_flag = On;
	
	modbus_server_write_handler(address);
	
    return NMBS_ERROR_NONE;
}

nmbs_error handle_write_multiple_registers_nmbs_server(uint16_t address, uint16_t quantity, const uint16_t* registers, uint8_t unit_id, void* arg)
{
	if(address < REGS_SYSTEM_PARAMETER_ADDR)
		return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

    if(((address - 100) % 2) != 0)
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
    if((quantity % 2) != 0)
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
    if((address - REGS_SYSTEM_PARAMETER_ADDR) + quantity > REGS_SYSTEM_PARAMETER_ADDR_MAX + 1)
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	
	uint32_t system_parameter_index = address - REGS_SYSTEM_PARAMETER_ADDR;
	
    for(int i = 0; i < quantity; i++)
        registers_nmbs_system_parameter[system_parameter_index + i] = registers[i];
	
	registers_nmbs_server_write_flag = On;
	
	modbus_server_write_handler(address);
	
    return NMBS_ERROR_NONE;
}

void environment_sensor_handler(uint32_t id)
{
	int16_t temp_raw = ((environment_sensor[id].holding_data[0] + 5) / 10) * 10;
	int16_t humi_raw = ((environment_sensor[id].holding_data[2] + 5) / 10) * 10;
	
	if(id == 0)
	{
		int8_t temperature_in = temp_raw / 10;
		int8_t humidity_in = humi_raw / 10;
		
		op.ID_ENVIRONMENT_IN = (uint16_t)(((uint8_t)temperature_in << 8) | humidity_in);
		op.ID_DEV_STATE |= BIT2;
	}
	else if(id == 1)
	{
		int8_t temperature_out = temp_raw / 10;
		int8_t humidity_out = humi_raw / 10;
		
		op.ID_ENVIRONMENT_OUT = (uint16_t)(((uint8_t)temperature_out << 8) | humidity_out);
		op.ID_DEV_STATE |= BIT3;
	}
	else{}
}

void power_meter_handler(uint32_t id)
{
	op.ID_POWER_METER_V = power_meter[id].holding_data[0];
	op.ID_POWER_METER_A = power_meter[id].holding_data[1];
	op.ID_POWER_METER_W = power_meter[id].holding_data[2];
	op.ID_POWER_METER_WH = (power_meter[id].holding_data[3] << 16) | power_meter[id].holding_data[4];
	op.ID_DEV_STATE |= BIT1;
}

void environment_sensor_error(uint32_t id)
{
	if(id == 0)
	{
		op.ID_ENVIRONMENT_IN = 0;
		op.ID_DEV_STATE &= ~BIT2;
	}
	else if(id == 1)
	{
		op.ID_ENVIRONMENT_OUT = 0;
		op.ID_DEV_STATE &= ~BIT3;
	}
	else{}
}

void power_meter_error(uint32_t id)
{
	op.ID_POWER_METER_V = 0;
	op.ID_POWER_METER_A = 0;
	op.ID_POWER_METER_W = 0;
	op.ID_POWER_METER_WH = 0;
	op.ID_DEV_STATE &= ~BIT1;
}

void modbus_server_update_handler(void)
{
	uint32_t sp_addr = 0;
	
	// operation parameter
	registers_nmbs_server[ID_SW_VER] = SW_VERSION;
	registers_nmbs_server[ID_PRODUCT_ID] = PRODUCT_ID;
	registers_nmbs_server[ID_DEV_ADDR] = RTU_HMI_ADDRESS;
	registers_nmbs_server[ID_DEV_OPERATION] = op.ID_DEV_OPERATION;
	registers_nmbs_server[ID_DEV_STATE] = op.ID_DEV_STATE;
	registers_nmbs_server[ID_POWER_METER_V] = op.ID_POWER_METER_V = 224;
	registers_nmbs_server[ID_POWER_METER_A] = op.ID_POWER_METER_A = 1123;
	registers_nmbs_server[ID_POWER_METER_W] = op.ID_POWER_METER_W = 2515;
											   op.ID_POWER_METER_WH = 150931;
	registers_nmbs_server[ID_POWER_METER_WH] = op.ID_POWER_METER_WH >> 16;
	registers_nmbs_server[ID_POWER_METER_WH + 1] = op.ID_POWER_METER_WH & 0xffff;
	registers_nmbs_server[ID_ENVIRONMENT_IN] = op.ID_ENVIRONMENT_IN = 0x0f39;
	registers_nmbs_server[ID_ENVIRONMENT_OUT] = op.ID_ENVIRONMENT_OUT = 0xf717;
	registers_nmbs_server[ID_FLOW_METER] = op.ID_FLOW_METER = 45;
	registers_nmbs_server[ID_FILTER_GAUGE] = op.ID_FILTER_GAUGE = 89;
	registers_nmbs_server[ID_RADIATOR] = op.ID_RADIATOR;
	registers_nmbs_server[ID_FAN_3] = op.ID_FAN_3;
	registers_nmbs_server[ID_SIGNAL_INDICATOR] = op.ID_SIGNAL_INDICATOR;
	
	registers_nmbs_server[ID_VALVE_1] = op.ID_VALVE_1;
	registers_nmbs_server[ID_PUMP_1] = op.ID_PUMP_1;
	registers_nmbs_server[ID_UV_LAMP] = op.ID_UV_LAMP;
	registers_nmbs_server[ID_AERATION_3] = op.ID_AERATION_3;
	registers_nmbs_server[ID_PUMP_3] = op.ID_PUMP_3;
	registers_nmbs_server[ID_VALVE_7] = op.ID_VALVE_7;
	registers_nmbs_server[ID_VALVE_8] = op.ID_VALVE_8;
	registers_nmbs_server[ID_WATER_PUMP_4] = op.ID_WATER_PUMP_4;
	registers_nmbs_server[ID_AERATION_1] = op.ID_AERATION_1;
	registers_nmbs_server[ID_AERATION_2] = op.ID_AERATION_2;
	
	registers_nmbs_server[ID_WATER_PUMP_1] = op.ID_WATER_PUMP_1;
	registers_nmbs_server[ID_WATER_PUMP_2] = op.ID_WATER_PUMP_2;
	registers_nmbs_server[ID_WATER_PUMP_3] = op.ID_WATER_PUMP_3;
	registers_nmbs_server[ID_VALVE_3] = op.ID_VALVE_3;
	registers_nmbs_server[ID_VALVE_4] = op.ID_VALVE_4;
	registers_nmbs_server[ID_VALVE_5] = op.ID_VALVE_5;
	registers_nmbs_server[ID_VALVE_6] = op.ID_VALVE_6;
	registers_nmbs_server[ID_MOTOR_8] = op.ID_MOTOR_8;
	registers_nmbs_server[ID_MOTOR_9] = op.ID_MOTOR_9;
	registers_nmbs_server[ID_WATER_PUMP_5] = op.ID_WATER_PUMP_5;
	
	registers_nmbs_server[ID_VALVE_2] = op.ID_VALVE_2;
	registers_nmbs_server[ID_MOTOR_1] = op.ID_MOTOR_1;
	registers_nmbs_server[ID_FAN_1] = op.ID_FAN_1;
	registers_nmbs_server[ID_FAN_2] = op.ID_FAN_2;
	registers_nmbs_server[ID_PTC_FAN] = op.ID_PTC_FAN;
	registers_nmbs_server[ID_MOTOR_2] = op.ID_MOTOR_2;
	registers_nmbs_server[ID_ACTUATOR_1] = op.ID_ACTUATOR_1;
	
	registers_nmbs_server[ID_MOTOR_3] = op.ID_MOTOR_3;
	registers_nmbs_server[ID_HEATER_1] = op.ID_HEATER_1;
	registers_nmbs_server[ID_PUMP_2] = op.ID_PUMP_2;
	registers_nmbs_server[ID_MOTOR_4] = op.ID_MOTOR_4;
	registers_nmbs_server[ID_MOTOR_5] = op.ID_MOTOR_5;
	registers_nmbs_server[ID_MOTOR_6] = op.ID_MOTOR_6;
	registers_nmbs_server[ID_MOTOR_7] = op.ID_MOTOR_7;
	registers_nmbs_server[ID_MOTOR_7_FG] = op.ID_MOTOR_7 == On ? 3456 : 0;//op.ID_MOTOR_7_FG;
	
	registers_nmbs_server[ID_LEVEL_SENSOR] = op.ID_LEVEL_SENSOR;
	registers_nmbs_server[ID_SWITCH] = op.ID_SWITCH;
	registers_nmbs_server[ID_SENSOR] = op.ID_SENSOR;
	
	// system parameter
	sp_addr = ID_VALVE_1_INTERVAL - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_INTERVAL >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_INTERVAL & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_1 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_1 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_1 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_2 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_2 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_2 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_3 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_3 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_3 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_4 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_4 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_4 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_5 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_5 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_5 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_6 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_6 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_6 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_7 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_7 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_7 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_8 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_8 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_8 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_9 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_9 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_9 & 0xffff;
	
	sp_addr = ID_VALVE_1_START_TIME_10 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_START_TIME_10 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_START_TIME_10 & 0xffff;
	
	sp_addr = ID_VALVE_1_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_1_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_1_ON_TIME & 0xffff;
	
	sp_addr = ID_PUMP_1_ON_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_1_ON_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_1_ON_DELAY & 0xffff;
	
	sp_addr = ID_UV_LAMP_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_UV_LAMP_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_UV_LAMP_OFF_DELAY & 0xffff;
	
	sp_addr = ID_AERATION_3_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_AERATION_3_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_AERATION_3_OFF_DELAY & 0xffff;
	
	sp_addr = ID_PUMP_3_INTERVAL - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_3_INTERVAL >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_3_INTERVAL & 0xffff;
	
	sp_addr = ID_PUMP_3_START_TIME_1 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_3_START_TIME_1 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_3_START_TIME_1 & 0xffff;
	
	sp_addr = ID_PUMP_3_START_TIME_2 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_3_START_TIME_2 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_3_START_TIME_2 & 0xffff;
	
	sp_addr = ID_PUMP_3_START_TIME_3 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_3_START_TIME_3 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_3_START_TIME_3 & 0xffff;
	
	sp_addr = ID_PUMP_3_START_TIME_4 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_3_START_TIME_4 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_3_START_TIME_4 & 0xffff;
	
	sp_addr = ID_PUMP_3_START_TIME_5 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_3_START_TIME_5 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_3_START_TIME_5 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_4_TIMEOUT - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_4_TIMEOUT >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_4_TIMEOUT & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_INTERVAL - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_INTERVAL >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_INTERVAL & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_1 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_1 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_1 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_2 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_2 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_2 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_3 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_3 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_3 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_4 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_4 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_4 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_5 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_5 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_5 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_6 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_6 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_6 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_7 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_7 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_7 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_8 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_8 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_8 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_9 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_9 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_9 & 0xffff;
	
	sp_addr = ID_SLUDGE_DISCHARGE_START_TIME_10 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_SLUDGE_DISCHARGE_START_TIME_10 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_SLUDGE_DISCHARGE_START_TIME_10 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_1_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_1_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_1_ON_TIME & 0xffff;
	
	sp_addr = ID_WATER_PUMP_2_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_2_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_2_ON_TIME & 0xffff;
	
	sp_addr = ID_VALVE_3_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_3_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_3_ON_TIME & 0xffff;
	
	sp_addr = ID_VALVE_3_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_3_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_3_OFF_DELAY & 0xffff;
	
	sp_addr = ID_VALVE_4_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_4_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_4_ON_TIME & 0xffff;
	
	sp_addr = ID_VALVE_4_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_4_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_4_OFF_DELAY & 0xffff;
	
	sp_addr = ID_VALVE_5_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_5_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_5_ON_TIME & 0xffff;
	
	sp_addr = ID_VALVE_5_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_5_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_5_OFF_DELAY & 0xffff;
	
	sp_addr = ID_VALVE_6_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_6_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_6_ON_TIME & 0xffff;
	
	sp_addr = ID_VALVE_6_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_6_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_6_OFF_DELAY & 0xffff;
	
	sp_addr = ID_MOTOR_8_OFF_DELAY - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_8_OFF_DELAY >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_8_OFF_DELAY & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_INTERVAL - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_INTERVAL >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_INTERVAL & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_1 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_1 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_1 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_2 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_2 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_2 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_3 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_3 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_3 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_4 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_4 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_4 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_5 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_5 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_5 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_6 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_6 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_6 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_7 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_7 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_7 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_8 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_8 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_8 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_9 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_9 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_9 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_10 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_10 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_10 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_11 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_11 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_11 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_12 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_12 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_12 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_13 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_13 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_13 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_14 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_14 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_14 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_15 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_15 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_15 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_16 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_16 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_16 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_17 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_17 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_17 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_18 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_18 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_18 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_19 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_19 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_19 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_20 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_20 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_20 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_21 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_21 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_21 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_22 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_22 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_22 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_23 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_23 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_23 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_START_TIME_24 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_START_TIME_24 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_START_TIME_24 & 0xffff;
	
	sp_addr = ID_WATER_PUMP_5_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_WATER_PUMP_5_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_WATER_PUMP_5_ON_TIME & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_1 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_1 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_1 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_2 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_2 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_2 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_3 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_3 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_3 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_4 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_4 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_4 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_5 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_5 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_5 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_6 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_6 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_6 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_7 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_7 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_7 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_8 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_8 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_8 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_9 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_9 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_9 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_DELAY_10 - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_DELAY_10 >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_DELAY_10 & 0xffff;
	
	sp_addr = ID_VALVE_2_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_VALVE_2_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_VALVE_2_ON_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_1_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_1_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_1_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_1_CW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_1_CW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_1_CW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_1_CCW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_1_CCW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_1_CCW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_1_STOP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_1_STOP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_1_STOP_TIME & 0xffff;
	
	sp_addr = ID_FAN_1_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_FAN_1_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_FAN_1_OP_TIME & 0xffff;
	
	sp_addr = ID_FAN_2_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_FAN_2_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_FAN_2_OP_TIME & 0xffff;
	
	sp_addr = ID_PTC_FAN_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PTC_FAN_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PTC_FAN_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_DRY_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_DRY_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_DRY_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_DRY_CW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_DRY_CW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_DRY_CW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_DRY_CCW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_DRY_CCW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_DRY_CCW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_DRY_STOP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_DRY_STOP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_DRY_STOP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_CRUSH_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_CRUSH_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_CRUSH_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_CRUSH_CW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_CRUSH_CW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_CRUSH_CW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_CRUSH_CCW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_CRUSH_CCW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_CRUSH_CCW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_CRUSH_STOP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_CRUSH_STOP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_CRUSH_STOP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_OUT_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_OUT_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_OUT_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_OUT_CW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_OUT_CW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_OUT_CW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_OUT_CCW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_OUT_CCW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_OUT_CCW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_2_OUT_STOP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_2_OUT_STOP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_2_OUT_STOP_TIME & 0xffff;
	
	sp_addr = ID_ACTUATOR_1_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_ACTUATOR_1_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_ACTUATOR_1_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_3_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_3_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_3_OP_TIME & 0xffff;
	
	sp_addr = ID_HEATER_1_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_HEATER_1_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_HEATER_1_OP_TIME & 0xffff;
	
	sp_addr = ID_HEATER_1_TEMP_UP_LIMIT_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_HEATER_1_TEMP_UP_LIMIT_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_HEATER_1_TEMP_UP_LIMIT_TIME & 0xffff;
	
	sp_addr = ID_PUMP_2_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_PUMP_2_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_PUMP_2_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_4_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_4_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_4_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_4_CW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_4_CW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_4_CW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_4_CCW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_4_CCW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_4_CCW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_5_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_5_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_5_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_5_ON_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_5_ON_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_5_ON_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_5_OFF_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_5_OFF_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_5_OFF_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_6_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_6_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_6_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_6_CW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_6_CW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_6_CW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_6_CCW_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_6_CCW_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_6_CCW_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_7_OP_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_7_OP_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_7_OP_TIME & 0xffff;
	
	sp_addr = ID_MOTOR_7_SET_RPM - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_MOTOR_7_SET_RPM >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_MOTOR_7_SET_RPM & 0xffff;
	
	sp_addr = ID_RADIATOR_ON_TEMP - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_RADIATOR_ON_TEMP >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_RADIATOR_ON_TEMP & 0xffff;
	
	sp_addr = ID_RADIATOR_OFF_TEMP - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_RADIATOR_OFF_TEMP >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_RADIATOR_OFF_TEMP & 0xffff;
	
	sp_addr = ID_HYSTERESIS_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	registers_nmbs_system_parameter[sp_addr] = sp.ID_HYSTERESIS_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_HYSTERESIS_TIME & 0xffff;
	
	sp_addr = ID_RTC_DATE - REGS_SYSTEM_PARAMETER_ADDR;
	//sp.ID_RTC_DATE = ((DEC_TO_BCD(rtc.u32Year - RTC_YEAR2000)) << 24) | ((DEC_TO_BCD(rtc.u32Month)) << 16) | ((DEC_TO_BCD(rtc.u32Day)) << 8) | ((DEC_TO_BCD(rtc.u32DayOfWeek)) << 0);
	registers_nmbs_system_parameter[sp_addr] = sp.ID_RTC_DATE >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_RTC_DATE & 0xffff;
	
	sp_addr = ID_RTC_TIME - REGS_SYSTEM_PARAMETER_ADDR;
	//sp.ID_RTC_TIME = ((DEC_TO_BCD(rtc.u32Hour)) << 16) | ((DEC_TO_BCD(rtc.u32Minute)) << 8) | ((DEC_TO_BCD(rtc.u32Second)) << 0);
	registers_nmbs_system_parameter[sp_addr] = sp.ID_RTC_TIME >> 16;
	registers_nmbs_system_parameter[sp_addr + 1] = sp.ID_RTC_TIME & 0xffff;
}

void modbus_server_write_handler(uint32_t addr)
{
	uint32_t sp_addr = 0;
	
	if(addr >= REGS_SYSTEM_PARAMETER_ADDR)
		sp_addr = addr - REGS_SYSTEM_PARAMETER_ADDR;
	
	switch(addr)
	{
		case ID_SW_VER:
		{
			uint16_t magic_number = registers_nmbs_server[addr];
			registers_nmbs_server[ID_SW_VER] = SW_VERSION;
			
			if(magic_number == 0x7377) // ascii : sw
			{
				boot_loader();
			}
		}
		break;
		
		case ID_DEV_OPERATION:
		{
			op.ID_DEV_OPERATION = registers_nmbs_server[addr];
		}
		break;
		
		case ID_RADIATOR:
		{
			RADIATOR = registers_nmbs_server[addr];
		
			op.ID_RADIATOR = registers_nmbs_server[addr];
		}
		break;
		
		case ID_FAN_3:
		{
			FAN_3 = registers_nmbs_server[addr];
			
			op.ID_FAN_3 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_SIGNAL_INDICATOR:
		{
			LAMP_G = (registers_nmbs_server[addr] & BIT0) ? On : Off;
			LAMP_Y = (registers_nmbs_server[addr] & BIT1) ? On : Off;
			LAMP_R = (registers_nmbs_server[addr] & BIT2) ? On : Off;
			BUZZER = (registers_nmbs_server[addr] & BIT3) ? On : Off;
			LAMP_EMERGENCY = (registers_nmbs_server[addr] & BIT4) ? On : Off;
			LAMP_AUTO_START = (registers_nmbs_server[addr] & BIT5) ? On : Off;
			LAMP_AUTO_STOP = (registers_nmbs_server[addr] & BIT6) ? On : Off;
			LAMP_FAUCET = (registers_nmbs_server[addr] & BIT7) ? On : Off;
			
			op.ID_SIGNAL_INDICATOR = (LAMP_FAUCET << 7) | (LAMP_AUTO_STOP << 6) | (LAMP_AUTO_START << 5) | (LAMP_EMERGENCY << 4) | \
									(BUZZER << 3) | (LAMP_R << 2) | (LAMP_Y << 1) | (LAMP_G << 0);
		}
		break;
		
		case ID_VALVE_1:
		{
			VALVE_1 = registers_nmbs_server[addr];
			
			op.ID_VALVE_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_PUMP_1:
		{
			PUMP_1 = registers_nmbs_server[addr];
			
			op.ID_PUMP_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_UV_LAMP:
		{
			LAMP_UV = registers_nmbs_server[addr];
			
			op.ID_UV_LAMP = registers_nmbs_server[addr];
		}
		break;
		
		case ID_AERATION_3:
		{
			AERATION_3 = registers_nmbs_server[addr];
			
			op.ID_AERATION_3 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_PUMP_3:
		{
			PUMP_3 = registers_nmbs_server[addr];
			
			op.ID_PUMP_3 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_7:
		{
			VALVE_7 = registers_nmbs_server[addr];
			
			op.ID_VALVE_7 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_8:
		{
			VALVE_8 = registers_nmbs_server[addr];
			
			op.ID_VALVE_8 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_WATER_PUMP_4:
		{
			WATER_PUMP_4 = registers_nmbs_server[addr];
			
			op.ID_WATER_PUMP_4 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_AERATION_1:
		{
			AERATION_1 = registers_nmbs_server[addr];
			
			op.ID_AERATION_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_AERATION_2:
		{
			AERATION_2 = registers_nmbs_server[addr];
			
			op.ID_AERATION_2 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_WATER_PUMP_1:
		{
			WATER_PUMP_1 = registers_nmbs_server[addr];
			
			op.ID_WATER_PUMP_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_WATER_PUMP_2:
		{
			WATER_PUMP_2 = registers_nmbs_server[addr];
			
			op.ID_WATER_PUMP_2 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_WATER_PUMP_3:
		{
			WATER_PUMP_3 = registers_nmbs_server[addr];
			
			op.ID_WATER_PUMP_3 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_3:
		{
			VALVE_3 = registers_nmbs_server[addr];
			
			op.ID_VALVE_3 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_4:
		{
			VALVE_4 = registers_nmbs_server[addr];
			
			op.ID_VALVE_4 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_5:
		{
			VALVE_5 = registers_nmbs_server[addr];
			
			op.ID_VALVE_5 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_6:
		{
			VALVE_6 = registers_nmbs_server[addr];
			
			op.ID_VALVE_6 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_8:
		{
			MOTOR_8 = registers_nmbs_server[addr];
			
			op.ID_MOTOR_8 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_9:
		{
			MOTOR_9 = registers_nmbs_server[addr];
			
			op.ID_MOTOR_9 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_WATER_PUMP_5:
		{
			WATER_PUMP_5 = registers_nmbs_server[addr];
			
			op.ID_WATER_PUMP_5 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_VALVE_2:
		{
			VALVE_2 = registers_nmbs_server[addr];
			
			op.ID_VALVE_2 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_1:
		{
			if(registers_nmbs_server[addr] == CW)
			{
				MOTOR_1 = On;
				MOTOR_1_DIR = Off;
			}
			else if(registers_nmbs_server[addr] == CCW)
			{
				MOTOR_1 = On;
				MOTOR_1_DIR = On;
			}
			else
			{
				MOTOR_1 = Off;
				MOTOR_1_DIR = Off;
			}
			
			op.ID_MOTOR_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_FAN_1:
		{
			FAN_1 = registers_nmbs_server[addr];
			
			op.ID_FAN_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_FAN_2:
		{
			FAN_2 = registers_nmbs_server[addr];
			
			op.ID_FAN_2 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_PTC_FAN:
		{
			PTC_FAN_1 = (registers_nmbs_server[addr] & BIT0) ? On : Off;
			PTC_FAN_2 = (registers_nmbs_server[addr] & BIT1) ? On : Off;
			PTC_FAN_3 = (registers_nmbs_server[addr] & BIT2) ? On : Off;
			PTC_FAN_4 = (registers_nmbs_server[addr] & BIT3) ? On : Off;
			PTC_FAN_5 = (registers_nmbs_server[addr] & BIT4) ? On : Off;
			PTC_FAN_6 = (registers_nmbs_server[addr] & BIT5) ? On : Off;
			
			op.ID_PTC_FAN = (PTC_FAN_6 << 5) | (PTC_FAN_5 << 4) | \
							(PTC_FAN_4 << 3) | (PTC_FAN_3 << 2) | (PTC_FAN_2 << 1) | (PTC_FAN_1 << 0);
		}
		break;
		
		case ID_MOTOR_2:
		{
			if(registers_nmbs_server[addr] == CW)
			{
				MOTOR_2 = On;
				MOTOR_2_DIR = Off;
			}
			else if(registers_nmbs_server[addr] == CCW)
			{
				MOTOR_2 = On;
				MOTOR_2_DIR = On;
			}
			else
			{
				MOTOR_2 = Off;
				MOTOR_2_DIR = Off;
			}
			
			op.ID_MOTOR_2 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_ACTUATOR_1:
		{
			ACTUATOR_1 = registers_nmbs_server[addr];
			
			op.ID_ACTUATOR_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_3:
		{
			MOTOR_3 = registers_nmbs_server[addr];
			
			op.ID_MOTOR_3 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_HEATER_1:
		{
			HEATER_1 = registers_nmbs_server[addr];
			
			op.ID_HEATER_1 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_PUMP_2:
		{
			PUMP_2 = registers_nmbs_server[addr];
			
			op.ID_PUMP_2 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_4:
		{
			if(registers_nmbs_server[addr] == CW)
			{
				MOTOR_4 = On;
				MOTOR_4_DIR = Off;
			}
			else if(registers_nmbs_server[addr] == CCW)
			{
				MOTOR_4 = On;
				MOTOR_4_DIR = On;
			}
			else
			{
				MOTOR_4 = Off;
				MOTOR_4_DIR = Off;
			}
			
			op.ID_MOTOR_4 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_5:
		{
			MOTOR_5 = registers_nmbs_server[addr];
			
			op.ID_MOTOR_5 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_6:
		{
			if(registers_nmbs_server[addr] == CW)
			{
				MOTOR_6 = On;
				MOTOR_6_DIR = Off;
			}
			else if(registers_nmbs_server[addr] == CCW)
			{
				MOTOR_6 = On;
				MOTOR_6_DIR = On;
			}
			else
			{
				MOTOR_6 = Off;
				MOTOR_6_DIR = Off;
			}
			
			op.ID_MOTOR_6 = registers_nmbs_server[addr];
		}
		break;
		
		case ID_MOTOR_7:
		{
			op.ID_MOTOR_7 = registers_nmbs_server[addr];
			run_event(STATUS_MOTOR_7, op.ID_MOTOR_7); // motor_7_set
		}
		break;
		
		// system parameter
		case ID_VALVE_1_INTERVAL:
		{
			sp.ID_VALVE_1_INTERVAL = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_1:
		{
			sp.ID_VALVE_1_START_TIME_1 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_2:
		{
			sp.ID_VALVE_1_START_TIME_2 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_3:
		{
			sp.ID_VALVE_1_START_TIME_3 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_4:
		{
			sp.ID_VALVE_1_START_TIME_4 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_5:
		{
			sp.ID_VALVE_1_START_TIME_5 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_6:
		{
			sp.ID_VALVE_1_START_TIME_6 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_7:
		{
			sp.ID_VALVE_1_START_TIME_7 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_8:
		{
			sp.ID_VALVE_1_START_TIME_8 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_9:
		{
			sp.ID_VALVE_1_START_TIME_9 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_START_TIME_10:
		{
			sp.ID_VALVE_1_START_TIME_10 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_1_ON_TIME:
		{
			sp.ID_VALVE_1_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_1_ON_DELAY:
		{
			sp.ID_PUMP_1_ON_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_UV_LAMP_OFF_DELAY:
		{
			sp.ID_UV_LAMP_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_AERATION_3_OFF_DELAY:
		{
			sp.ID_AERATION_3_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_INTERVAL:
		{
			sp.ID_PUMP_3_INTERVAL = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_START_TIME_1:
		{
			sp.ID_PUMP_3_START_TIME_1 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_START_TIME_2:
		{
			sp.ID_PUMP_3_START_TIME_2 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_START_TIME_3:
		{
			sp.ID_PUMP_3_START_TIME_3 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_START_TIME_4:
		{
			sp.ID_PUMP_3_START_TIME_4 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_START_TIME_5:
		{
			sp.ID_PUMP_3_START_TIME_5 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_3_ON_TIME:
		{
			sp.ID_PUMP_3_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_4_TIMEOUT:
		{
			sp.ID_WATER_PUMP_4_TIMEOUT = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_INTERVAL:
		{
			sp.ID_SLUDGE_DISCHARGE_INTERVAL = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_1:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_1 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_2:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_2 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_3:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_3 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_4:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_4 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_5:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_5 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_6:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_6 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_7:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_7 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_8:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_8 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_9:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_9 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_SLUDGE_DISCHARGE_START_TIME_10:
		{
			sp.ID_SLUDGE_DISCHARGE_START_TIME_10 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_1_ON_TIME:
		{
			sp.ID_WATER_PUMP_1_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_2_ON_TIME:
		{
			sp.ID_WATER_PUMP_2_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_3_ON_TIME:
		{
			sp.ID_VALVE_3_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_3_OFF_DELAY:
		{
			sp.ID_VALVE_3_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_4_ON_TIME:
		{
			sp.ID_VALVE_4_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_4_OFF_DELAY:
		{
			sp.ID_VALVE_4_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_5_ON_TIME:
		{
			sp.ID_VALVE_5_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_5_OFF_DELAY:
		{
			sp.ID_VALVE_5_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_6_ON_TIME:
		{
			sp.ID_VALVE_6_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_6_OFF_DELAY:
		{
			sp.ID_VALVE_6_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_8_OFF_DELAY:
		{
			sp.ID_MOTOR_8_OFF_DELAY = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_INTERVAL:
		{
			sp.ID_WATER_PUMP_5_INTERVAL = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_1:
		{
			sp.ID_WATER_PUMP_5_START_TIME_1 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_2:
		{
			sp.ID_WATER_PUMP_5_START_TIME_2 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_3:
		{
			sp.ID_WATER_PUMP_5_START_TIME_3 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_4:
		{
			sp.ID_WATER_PUMP_5_START_TIME_4 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_5:
		{
			sp.ID_WATER_PUMP_5_START_TIME_5 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_6:
		{
			sp.ID_WATER_PUMP_5_START_TIME_6 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_7:
		{
			sp.ID_WATER_PUMP_5_START_TIME_7 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_8:
		{
			sp.ID_WATER_PUMP_5_START_TIME_8 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_9:
		{
			sp.ID_WATER_PUMP_5_START_TIME_9 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_10:
		{
			sp.ID_WATER_PUMP_5_START_TIME_10 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_11:
		{
			sp.ID_WATER_PUMP_5_START_TIME_11 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_12:
		{
			sp.ID_WATER_PUMP_5_START_TIME_12 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_13:
		{
			sp.ID_WATER_PUMP_5_START_TIME_13 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_14:
		{
			sp.ID_WATER_PUMP_5_START_TIME_14 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_15:
		{
			sp.ID_WATER_PUMP_5_START_TIME_15 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_16:
		{
			sp.ID_WATER_PUMP_5_START_TIME_16 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_17:
		{
			sp.ID_WATER_PUMP_5_START_TIME_17 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_18:
		{
			sp.ID_WATER_PUMP_5_START_TIME_18 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_19:
		{
			sp.ID_WATER_PUMP_5_START_TIME_19 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_20:
		{
			sp.ID_WATER_PUMP_5_START_TIME_20 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_21:
		{
			sp.ID_WATER_PUMP_5_START_TIME_21 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_22:
		{
			sp.ID_WATER_PUMP_5_START_TIME_22 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_23:
		{
			sp.ID_WATER_PUMP_5_START_TIME_23 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_START_TIME_24:
		{
			sp.ID_WATER_PUMP_5_START_TIME_24 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_WATER_PUMP_5_ON_TIME:
		{
			sp.ID_WATER_PUMP_5_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_1:
		{
			sp.ID_VALVE_2_ON_DELAY_1 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_2:
		{
			sp.ID_VALVE_2_ON_DELAY_2 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_3:
		{
			sp.ID_VALVE_2_ON_DELAY_3 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_4:
		{
			sp.ID_VALVE_2_ON_DELAY_4 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_5:
		{
			sp.ID_VALVE_2_ON_DELAY_5 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_6:
		{
			sp.ID_VALVE_2_ON_DELAY_6 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_7:
		{
			sp.ID_VALVE_2_ON_DELAY_7 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_8:
		{
			sp.ID_VALVE_2_ON_DELAY_8 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_9:
		{
			sp.ID_VALVE_2_ON_DELAY_9 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_DELAY_10:
		{
			sp.ID_VALVE_2_ON_DELAY_10 = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_VALVE_2_ON_TIME:
		{
			sp.ID_VALVE_2_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_1_OP_TIME:
		{
			sp.ID_MOTOR_1_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_1_CW_TIME:
		{
			sp.ID_MOTOR_1_CW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_1_CCW_TIME:
		{
			sp.ID_MOTOR_1_CCW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_1_STOP_TIME:
		{
			sp.ID_MOTOR_1_STOP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_FAN_1_OP_TIME:
		{
			sp.ID_FAN_1_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_FAN_2_OP_TIME:
		{
			sp.ID_FAN_2_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PTC_FAN_OP_TIME:
		{
			sp.ID_PTC_FAN_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_DRY_OP_TIME:
		{
			sp.ID_MOTOR_2_DRY_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_DRY_CW_TIME:
		{
			sp.ID_MOTOR_2_DRY_CW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_DRY_CCW_TIME:
		{
			sp.ID_MOTOR_2_DRY_CCW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_DRY_STOP_TIME:
		{
			sp.ID_MOTOR_2_DRY_STOP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_CRUSH_OP_TIME:
		{
			sp.ID_MOTOR_2_CRUSH_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_CRUSH_CW_TIME:
		{
			sp.ID_MOTOR_2_CRUSH_CW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_CRUSH_CCW_TIME:
		{
			sp.ID_MOTOR_2_CRUSH_CCW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_CRUSH_STOP_TIME:
		{
			sp.ID_MOTOR_2_CRUSH_STOP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_OUT_OP_TIME:
		{
			sp.ID_MOTOR_2_OUT_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_OUT_CW_TIME:
		{
			sp.ID_MOTOR_2_OUT_CW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_OUT_CCW_TIME:
		{
			sp.ID_MOTOR_2_OUT_CCW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_2_OUT_STOP_TIME:
		{
			sp.ID_MOTOR_2_OUT_STOP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_ACTUATOR_1_OP_TIME:
		{
			sp.ID_ACTUATOR_1_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_3_OP_TIME:
		{
			sp.ID_MOTOR_3_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_HEATER_1_OP_TIME:
		{
			sp.ID_HEATER_1_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_HEATER_1_TEMP_UP_LIMIT_TIME:
		{
			sp.ID_HEATER_1_TEMP_UP_LIMIT_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_PUMP_2_OP_TIME:
		{
			sp.ID_PUMP_2_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_4_OP_TIME:
		{
			sp.ID_MOTOR_4_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_4_CW_TIME:
		{
			sp.ID_MOTOR_4_CW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_4_CCW_TIME:
		{
			sp.ID_MOTOR_4_CCW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_5_OP_TIME:
		{
			sp.ID_MOTOR_5_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_5_ON_TIME:
		{
			sp.ID_MOTOR_5_ON_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_5_OFF_TIME:
		{
			sp.ID_MOTOR_5_OFF_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_6_OP_TIME:
		{
			sp.ID_MOTOR_6_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_6_CW_TIME:
		{
			sp.ID_MOTOR_6_CW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_6_CCW_TIME:
		{
			sp.ID_MOTOR_6_CCW_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_7_OP_TIME:
		{
			sp.ID_MOTOR_7_OP_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_MOTOR_7_SET_RPM:
		{
			sp.ID_MOTOR_7_SET_RPM = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
			run_event(STATUS_MOTOR_7, op.ID_MOTOR_7); // motor_7_set
		}
		break;
		
		case ID_RADIATOR_ON_TEMP:
		{
			sp.ID_RADIATOR_ON_TEMP = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_RADIATOR_OFF_TEMP:
		{
			sp.ID_RADIATOR_OFF_TEMP = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_HYSTERESIS_TIME:
		{
			sp.ID_HYSTERESIS_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
		}
		break;
		
		case ID_RTC_DATE:
		{
			sp.ID_RTC_DATE = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
			RTC_SetDate(RTC_YEAR2000 + BCD_TO_DEC((sp.ID_RTC_DATE >> 24)), BCD_TO_DEC((sp.ID_RTC_DATE >> 16)), BCD_TO_DEC((sp.ID_RTC_DATE >> 8)), BCD_TO_DEC((sp.ID_RTC_DATE >> 0)));
		}
		break;
		
		case ID_RTC_TIME:
		{
			sp.ID_RTC_TIME = registers_nmbs_system_parameter[sp_addr] << 16 | registers_nmbs_system_parameter[sp_addr + 1];
			RTC_SetTime(BCD_TO_DEC((sp.ID_RTC_TIME >> 16)), BCD_TO_DEC((sp.ID_RTC_TIME >> 8)), BCD_TO_DEC((sp.ID_RTC_TIME >> 0)), RTC_CLOCK_24, RTC_AM);
		}
		break;
		
		default:break;
	}
	
	registers_nmbs_server_write_flag = Off;
	
	if(addr >= REGS_SYSTEM_PARAMETER_ADDR)
		run_event(STATUS_EEPROM, 0); // eeprom_Write
}

void ModbusRTU_Init(void)
{
	GPIO_SetMode(PD, BIT11, GPIO_MODE_OUTPUT); // PC_RE_DE
	UART_Open(PC_UART, 9600);
	PC_RE_DE = 0;
	
	GPIO_SetMode(PD, BIT5, GPIO_MODE_OUTPUT); // HMI_RE_DE
	UART_Open(HMI_UART, 115200);
	HMI_RE_DE = 0;
	
    nmbs_platform_conf_create(&platform_conf_client);
    platform_conf_client.transport = NMBS_TRANSPORT_RTU;
    platform_conf_client.read = read_uart0;
    platform_conf_client.write = write_uart0;
	
	nmbs_platform_conf_create(&platform_conf_server);
    platform_conf_server.transport = NMBS_TRANSPORT_RTU;
    platform_conf_server.read = read_uart2;
    platform_conf_server.write = write_uart2;
	
    nmbs_callbacks_create(&callbacks_server);
    //callbacks_server.read_coils = handle_read_coils_nmbs_server;
    //callbacks_server.write_multiple_coils = handle_write_multiple_coils_nmbs_server;
    callbacks_server.read_holding_registers = handler_read_holding_registers_nmbs_server;
	callbacks_server.write_single_register = handler_write_single_registers_nmbs_server;
    callbacks_server.write_multiple_registers = handle_write_multiple_registers_nmbs_server;
	
    err_nmbs_client = nmbs_client_create(&nmbs_client, &platform_conf_client);
	
    if(err_nmbs_client != NMBS_ERROR_NONE)
	{
        // error
    }
	
    nmbs_set_read_timeout(&nmbs_client, 100);
    nmbs_set_byte_timeout(&nmbs_client, 500);
	
	current_poll_id = RTU_PC_START_SLAVE_ID;
	
    err_nmbs_server = nmbs_server_create(&nmbs_server, RTU_HMI_ADDRESS, &platform_conf_server, &callbacks_server);
	
    if(err_nmbs_server != NMBS_ERROR_NONE)
	{
        // error
    }
	
    nmbs_set_read_timeout(&nmbs_server, 100);
    nmbs_set_byte_timeout(&nmbs_server, 100);
}

void ModbusRTU_Process(void)
{
	nmbs_error poll_err = NMBS_ERROR_INVALID_RESPONSE;
	uint32_t index = 0;
	
	err_nmbs_server = nmbs_server_poll(&nmbs_server); // HMI
	
	if(err_nmbs_server == NMBS_ERROR_TRANSPORT)
	{
		// error
	}
	
	if(getSysTick_ms() - last_poll_time >= RTU_PC_READ_INTERVAL_MS) // PC
	{
		last_poll_time = getSysTick_ms();
		
		nmbs_set_destination_rtu_address(&nmbs_client, current_poll_id);
		
		if(current_poll_id < 3)
		{
			index = current_poll_id - 1;
			poll_err = nmbs_read_holding_registers(&nmbs_client, 563, 4, environment_sensor[index].holding_data);
		}
		else if(current_poll_id == 3)
		{
			index = 0;
			poll_err = nmbs_read_holding_registers(&nmbs_client, 0, 7, power_meter[index].holding_data);
		}
		else{}
		
		if(poll_err == NMBS_ERROR_NONE)
		{
			run_event(STATUS_MODBUS_CLIENT, current_poll_id);
		}
		else
		{
			// error
			run_event(STATUS_MODBUS_CLIENT_ERROR, current_poll_id);
		}
		
		current_poll_id++;
			
		if(current_poll_id > RTU_PC_MAX_SLAVES)
		{
			current_poll_id = RTU_PC_START_SLAVE_ID;
		}
	}
}
