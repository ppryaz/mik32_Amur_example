#include "lcd2004A_i2c.h"
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LCD_ROW0 0x80
#define LCD_ROW1 0xC0
#define LCD_ROW2 0x94
#define LCD_ROW3 0xD4
#define LCD_ENABLE_BIT 0x04

I2C_HandleTypeDef hi2c;
HAL_StatusTypeDef error_code = HAL_ERROR;
static uint8_t bclg, i2c, addr;

//-----------     Setup code table for cyrrilic symbol utf8  ---------------------
//--------------------------------------------------------------------------------
// If you want to add a display with  a different character map, add it here.
// The character in the table should be arranged in alphabetical order,
// uppercase firse, then lowercase.
//
// If your display does not support Cyrrilic, you will have to create and  load to display into 8 free display cell,
// and define you own table utf_recode[] of correlation between Russian utf8 and your display code map.
//
// Кодовая таблица кириллицы в дисплее WH2004A_YGH_CT с поддержкой русского языка

#ifdef WH2004A_YGH_CT
const char utf_recode[] =
       { 0x41,0xa0,0x42,0xa1,0xe0,0x45,0xa3,0xa4,0xa5,0xa6,0x4b,0xa7,0x4d,0x48,0x4f,
         0xa8,0x50,0x43,0x54,0xa9,0xaa,0x58,0xe1,0xab,0xac,0xe2,0xad,0xae,0x62,0xaf,0xb0,0xb1,
         0x61,0xb2,0xb3,0xb4,0xe3,0x65,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0x6f,
         0xbe,0x70,0x63,0xbf,0x79,0xe4,0x78,0xe5,0xc0,0xc1,0xe6,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xa2,0xb5
        };
#endif

static HAL_StatusTypeDef
I2C1_Init(void)
{

	/* Общие настройки */
    hi2c.Instance = i2c ? I2C_1 : I2C_0;
    hi2c.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c.Init.AutoEnd = I2C_AUTOEND_ENABLE;
    /* Настройка частоты */
    hi2c.Clock.PRESC = 5;
    hi2c.Clock.SCLDEL = 15;
    hi2c.Clock.SDADEL = 15;
    hi2c.Clock.SCLH = 15;
    hi2c.Clock.SCLL = 15;
    return(HAL_I2C_Init(&hi2c));// != HAL_OK)
    {
        //xprintf("I2C_Init error\n");
    }
}

void i2c_lcd_init(uint8_t n_i2c, uint8_t n_addr) {
	bclg = LCD_BACKLIGHT;
	i2c = n_i2c;
	addr =n_addr;
    if (I2C1_Init() != HAL_OK) {
    }
	/* required magic taken from pi libs */
	lcd_send_cmd(/*lcd,*/ 0x03);
	lcd_send_cmd(/*lcd,*/ 0x03);
	lcd_send_cmd(/*lcd,*/ 0x03);
	lcd_send_cmd(/*lcd,*/ 0x02);
	/* initialize display config */
	lcd_send_cmd(/*lcd,*/ LCD_ENTRYMODESET | LCD_ENTRYLEFT);
	lcd_send_cmd(/*lcd,*/ LCD_FUNCTIONSET | LCD_2LINE);
	lcd_on(/*lcd*/);
	lcd_clear(/*lcd*/);
	//return lcd;
}

static void
i2c_write(uint8_t b)
{
	uint8_t data[1];
	data[0]=b;
	error_code = HAL_I2C_Master_Transmit(&hi2c, addr, data, sizeof(data),I2C_TIMEOUT_DEFAULT);
	        if (error_code != HAL_OK)
	        	{
	            	//xprintf("Master_Transmit error #%d, code %d, ISR %d\n", error_code, hi2c0.ErrorCode, hi2c0.Instance->ISR);
	        		HAL_I2C_Reset(&hi2c);
	        	};
	        if (hi2c.Init.AutoEnd == I2C_AUTOEND_DISABLE)
	         	 {
	                hi2c.Instance->CR2 |= I2C_CR2_STOP_M;
	         	 }
}

static void
lcd_toggle_enable(/*int fd,*/ uint8_t val)
{
	//HAL_Delay(100);//usleep(666); /* generous delay to not mess up controller */
	i2c_write(/*fd,*/ val | LCD_ENABLE_BIT);
	//HAL_Delay(100);//usleep(666);
	i2c_write(/*fd,*/ val & ~LCD_ENABLE_BIT);
	//HAL_Delay(100);//usleep(666);
}

void
lcd_send_byte(uint8_t val, uint8_t mode) {
	uint8_t buf = mode | (val & 0xF0) | bclg;
	i2c_write(buf); /* write first nibble */
	lcd_toggle_enable(buf);
	buf = mode | ((val << 4) & 0xF0) | bclg;
	i2c_write(buf); /* write second nibble */
	lcd_toggle_enable(buf);
}

void
lcd_backlight(uint8_t on)
{
	bclg  = on ? LCD_BACKLIGHT : LCD_NOBACKLIGHT;
	lcd_on();
}

void
lcd_write(char *data)
{
	for (uint8_t i = 0; i < strlen(data); ++i) {
		lcd_send_chr(data[i]);
	}
}

void
lcd_move(int x, int y)
{
	uint8_t pos_addr = x;
	switch(y) {
		case 0:
			pos_addr += LCD_ROW0;
			break;
		case 1:
			pos_addr += LCD_ROW1;
			break;
		case 2:
			pos_addr += LCD_ROW2;
			break;
		case 3:
			pos_addr += LCD_ROW3;
			break;
	}
	lcd_send_cmd(pos_addr);
}

//-------------------------------------------------------------------------------------

void lcd_print(uint16_t x_pos, uint16_t y_pos, uint8_t source[]) {

	uint8_t output_array[MAX_LENGTH_LINE]= {0};			// Output array

	uint8_t utf_hi_char = 0;
	uint32_t output_array_size = 0;

	uint32_t size = strlen(source);

	if (size > MAX_LENGTH_LINE) size = MAX_LENGTH_LINE;

	for (int i = 0; i < size ; ++i) {
		uint8_t out_char = source[i];
														// UTF-8 handling
		if (source[i] >= 0x80) { 						// Если в source[i] старший байт utf8
			if (source[i] >= 0xc0) {					//
				utf_hi_char = source[i] - 0xd0;			// Получаем 0 или 1 старшего байта utf8
			} else {
				source[i] -= 0X80;						// Обрабатываем младший байт utf8
				if (source[i] == 1 && !utf_hi_char) {
					output_array[output_array_size] = utf_recode[64]; // Position Ё
					++output_array_size;
				} else if (source[i] == 0x11 && utf_hi_char == 1) {
					output_array[output_array_size] = utf_recode[65]; // Position ё
					++output_array_size;
				} else {
					uint8_t byte = utf_recode[(source[i] + (utf_hi_char << 6)) - 0x10]; // Позиция нужного нам символа в массиве

					output_array[output_array_size] = byte;	// Add cyrrilic sybmol utf8 code which define in display code map
					++output_array_size;
				}
			}
		} else {
			output_array[output_array_size] = out_char;
			++output_array_size;						// Если однобайтовая кодировка символов ANSII
		}
	}
		lcd_mvwrite(x_pos, y_pos, output_array);
}
