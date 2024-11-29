/*******************************************************************************
 * **********         Библиотека для работы с LCD2004          *****************
 ************  модель с поддержкой руссокго языка  WH2004A_YGH_CT  *************
 *******************************************************************************
 *******************************************************************************
 * Библиотека позволяет выводить на дисплей символы русского алфавита,
 * в сочетании с латинскими символами, цифрами и знаками препинания.
 * Для корректной работы исходные файлы должны быть закодированы в формате UTF8
 *
 * 							!!!!!! ATTENTION!!!!!
 * Encoding of source texts in windows 1251 is not supported!!
 * Please save your files in UTF8 before using!
 *
 * Не поддерживаются символы №, {}
 * Для соединения с дисплеем по I2C, используется конвертер PCF8574
 *
 * Адаптирована для работы  микроконтроллером  MIK32 Амур К1948ВК018
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3
 */

#ifndef LCD2004A_I2C_H
#define LCD2004A_I2C_H
#include <stdint.h> 			// uint8_t
#include "mik32_hal_i2c.h" 		// HAL_StatusTypeDef

#define WH2004A_YGH_CT			// Тип дисплея с поддержкой кириллицы
#define MAX_LENGTH_LINE	 50		// Максимальная длина строки

// General commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_CURSORSHIFT 0x10
#define LCD_DISPLAYCONTROL 0x08
#define LCD_ENTRYMODESET 0x04
#define LCD_FUNCTIONSET 0x20
#define LCD_RETURNHOME 0x02
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Entrymode flags
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Displaycontrol flags
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// Cursorshift flags
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

//  Functionset flags
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Write modes for lcd_write_byte
#define LCD_CMD 0
#define LCD_CHR 1

// ------------------  Функции для работы с lcd2004a -------------------------------------

void i2c_lcd_init(uint8_t n_i2c, uint8_t n_addr);	// Инициализация i2c и lcd задаём адрес slave
void lcd_send_byte(uint8_t val, uint8_t mode); 		// <- not meant for direct calling
void lcd_backlight(uint8_t on);						// Включить выключить подсветку если on=0 lcd_off инача lcd_on
void lcd_move(int x, int y);						// Установить позицию курсора x=0..19 y=0..3
void lcd_write(char *data);							// Выводим строку начиная с текущей позиции

//--------  Main function for print Cyrrilic Sympol in utf8 ----------------------------

void lcd_print(uint16_t x_pos, uint16_t y_pos, uint8_t text[]); // выводим строку

//---------------------------------------------------------------------------------------
// ---------------------------    Макросы   ---------------------------------------------

#define lcd_mvwrite(x, y, data) do { lcd_move(x, y); lcd_write(data); } while (0)			// Выводим строку начиная с указаной текущей позиции
#define lcd_send_cmd(cmd) lcd_send_byte(cmd, LCD_CMD)										// Выполнить команду (см даташит lcd)
#define lcd_send_chr(chr) lcd_send_byte(chr, LCD_CHR)										// Выводит символ в текущую позицию и увеличивает её на 1
#define lcd_off() lcd_send_cmd(LCD_DISPLAYCONTROL | LCD_DISPLAYOFF)							// Скрыть текст на экране
#define lcd_on() lcd_send_cmd(LCD_DISPLAYCONTROL | LCD_DISPLAYON)							// Показать текст на экране
#define lcd_clear() do { lcd_send_cmd(LCD_CLEARDISPLAY); lcd_send_cmd(LCD_RETURNHOME); } while (0) // Очистить экран
#define lcd_cgramset(addr) lcd_send_cmd(LCD_SETCGRAMADDR|((addr)<<3))						// Записать образ символа в ОЗУ lcd
#endif
