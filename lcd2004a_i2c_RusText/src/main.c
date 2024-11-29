#include "lcd2004A_i2c.h"
#include "string.h"
#include "uart_lib.h"
#include "xprintf.h"

/* ************  Пример работы с дисплеем LCD2004 модель WH2004A_YGH_CT   ************************
 * ************				 с поддержкой русского языка			  	  ************************
 *	Пример демонстрирует вывод поочередно всех русских букв,
 *	всех латинских  и служебных символов, и четырехстрочного меню-списка, в котором совмещены
 *	всех ранее использованные символы.
 *
 *	Файл должен иметь кодировку UTF8
 *	Адрес дисплея:   0x27
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3, с программатором
 */

#define ADDRESS_DISPLAY 0x27
#define SWITCH_DELAY	5		// задержка переключения экранов  в секундах

void SystemClock_Config(void);

void printRusAlphabet();
void printEngAlphabet();
void printTempalteMenu();

//------------------------------------------------------------------------------------

int main() {
	SystemClock_Config();
	UART_Init(UART_1, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

	PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_0_M;

	// LEDs init
	GPIO_0->DIRECTION_OUT = 1 << (9);
	GPIO_0->OUTPUT ^= (0b1) << (9);

	for (volatile int i = 0; i < 10000; i++);
	GPIO_0->OUTPUT ^= (0b1) << (9);

	for (volatile int i = 0; i < 10000; i++);
	GPIO_0->OUTPUT ^= (0b1) << (9);

	i2c_lcd_init(1, 0x27);

	uint32_t counter = 0;
	uint8_t switcher = 0;
	uint32_t switch_delay = SWITCH_DELAY;

	printTempalteMenu();
	HAL_DelayMs(1000);
    while (1)
    {
    	if (counter > switch_delay) {
    		if (!switcher)  {
    			printRusAlphabet();
    			++switcher;
    		} else if(switcher == 1){
    			printEngAlphabet();
    			++switcher;
    		} else {
    			printTempalteMenu();
    			switcher = 0;
    		}

    		counter = 0;
    	}
    	

    	GPIO_0->OUTPUT ^= (0b1)<<(9);
        for (volatile int i = 0; i < 1000000; i++);
        counter++;
    }
}
//--------------------------------------------------------------------------------------

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};
    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}
//---------------------------------------------------------------------------------------

void printRusAlphabet() {
	lcd_clear();
	HAL_Delay(100);
	lcd_print(0, 0, "АБВГДЕЁЖЗИЙКЛМНОПРС");
	HAL_Delay(100);
	lcd_print(0, 1, "ТУФХЦЧШЩЪЫЬЭЮЯ");
	HAL_Delay(100);
	lcd_print(0, 2, "абвгдеёжзийклмнопрс"); //RuSymb !\";%:?*/,.
	HAL_Delay(100);
	lcd_print(0, 3, "туфхцчшщъыьэюя"); //@#$%^&*()-=_+;:[]<>?
	HAL_Delay(100);

	lcd_on();
}
//--------------------------------------------------------------------------------------

void printEngAlphabet() {
	lcd_clear();

	HAL_Delay(100);
	lcd_print(0, 0, "AaBbCcDdEeFfGgHhIiJj");
	HAL_Delay(100);
	lcd_print(0, 1, "KkLlMmNnOoPpQqRrSsTt");
	HAL_Delay(100);
	lcd_print(0, 2, "UuVvWwXxYyZz"); //RuSymb !\";%:?*/,.
	HAL_Delay(100);
	lcd_print(0, 3, "!@#$%^&*()[];:'\"<>?"); //@#$%^&*()-=_+;:[]<>?
	HAL_Delay(100);

	lcd_on();
}
//--------------------------------------------------------------------------------------

void printTempalteMenu() {
	lcd_clear();
	
	HAL_Delay(100);
	lcd_print(0, 0, "1.First item");
	HAL_Delay(100);
	lcd_print(0, 1, "2.Второй пункт");
	HAL_Delay(100);
	lcd_print(0, 2, "3.Старт Force GPTP"); //RuSymb !\";%:?*/,.
	HAL_Delay(100);
	lcd_print(0, 3, "      Меридиан     "); //@#$%^&*()-=_+;:[]<>?
	HAL_Delay(100);
	
	lcd_on();
}
//------------------------------------------------------------------------------------

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
