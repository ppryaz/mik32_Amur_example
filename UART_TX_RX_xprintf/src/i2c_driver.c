#include <mcu32_memory_map.h>
#include <pad_config.h>
#include <gpio.h>
#include <spifi.h>
#include <uart.h>
#include <power_manager.h>

#include "i2c_driver.h"


#define I2C_SCL_PORT	(GPIO_1)
#define I2C_SCL_PIN		(1)

#define I2C_SDA_PORT	(GPIO_1)
#define I2C_SDA_PIN		(0)

#define LCD_RST_PORT	(GPIO_2)
#define LCD_RST_PIN		(7)

void OLED_RST_1(void)
{
	LCD_RST_PORT->SET = 1<<LCD_RST_PIN;
}

void OLED_RST_0(void)
{
	LCD_RST_PORT->CLEAR = 1<<LCD_RST_PIN;
}

void Driver_Delay_ms(int delay)
{
	for (volatile int i = 0; i < delay*1000; i++);
}

void HAL_Delay(int delay)
{
	delay *=3;
	for (volatile int i = 0; i < delay; i++);
}
void iic_init(void)
{
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_1_M;
	PM->CLK_APB_M_SET |= PM_CLOCK_APB_M_PAD_CONFIG_M;
	//configure as GPIO
	//PAD_CONFIG->PORT_1_CFG |= (0b01)<<(I2C_SDA_PIN*2);
	//PAD_CONFIG->PORT_1_CFG |= (0b01)<<(I2C_SCL_PIN*2);
	//PAD_CONFIG->PORT_1_CFG |= (0b01)<<(LCD_RST_PIN*2);
	// both pull up
	PAD_CONFIG->PORT_1_PUPD |= (0b01)<<(I2C_SDA_PIN*2);
	PAD_CONFIG->PORT_1_PUPD |= (0b01)<<(I2C_SCL_PIN*2);

	LCD_RST_PORT->DIRECTION_OUT =  (0b1)<<LCD_RST_PIN;
	I2C_SCL_PORT->DIRECTION_OUT =  (0b1)<<I2C_SCL_PIN;
	I2C_SDA_PORT->DIRECTION_IN =  (0b1)<<I2C_SDA_PIN;
	I2C_SCL_PORT->SET = 1<<I2C_SCL_PIN;
	I2C_SDA_PORT->SET = 1<<I2C_SDA_PIN;
	HAL_Delay(1);
}

void iic_start(void)
{
	I2C_SDA_PORT->DIRECTION_OUT =  (0b1)<<I2C_SDA_PIN;
	I2C_SDA_PORT->CLEAR = 1<<I2C_SDA_PIN;
	I2C_SCL_PORT->CLEAR = 1<<I2C_SCL_PIN;
	HAL_Delay(1);
}


void iic_write_byte(char byte)
{
	I2C_SDA_PORT->DIRECTION_OUT =  (0b1)<<I2C_SDA_PIN;
	for (int i = 0; i<8; i++)
	{
		if (byte&0x80)
			I2C_SDA_PORT->SET = 1<<I2C_SDA_PIN;
		else
			I2C_SDA_PORT->CLEAR = 1<<I2C_SDA_PIN;
		//HAL_Delay(1);
		I2C_SCL_PORT->SET = 1<<I2C_SCL_PIN;
		HAL_Delay(1);
		I2C_SCL_PORT->CLEAR = 1<<I2C_SCL_PIN;
		if (i == 7)
			I2C_SDA_PORT->DIRECTION_IN =  (0b1)<<I2C_SDA_PIN;
		byte <<= 1;
	}
}

int iic_wait_for_ack(void)
{
	int ack = 0;
	I2C_SCL_PORT->SET = 1<<I2C_SCL_PIN;
	HAL_Delay(1);
	if (I2C_SDA_PORT->SET & (1<<I2C_SDA_PIN))
		ack = 1;
	I2C_SCL_PORT->CLEAR = 1<<I2C_SCL_PIN;
	HAL_Delay(1);
	return ack;
}

void iic_stop(void)
{
	I2C_SDA_PORT->DIRECTION_OUT =  (0b1)<<I2C_SDA_PIN;
	I2C_SDA_PORT->SET = 1<<I2C_SDA_PIN;
	//HAL_Delay(1);
	I2C_SDA_PORT->CLEAR = 1<<I2C_SDA_PIN;
	HAL_Delay(1);
	I2C_SCL_PORT->SET = 1<<I2C_SCL_PIN;
	HAL_Delay(1);
	I2C_SDA_PORT->DIRECTION_IN =  (0b1)<<I2C_SDA_PIN;
	HAL_Delay(1);
}
