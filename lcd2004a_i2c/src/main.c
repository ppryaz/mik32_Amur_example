#include "lcd2004A_i2c.h"
#include "mik32_hal.h"

/*
 * Пример управления дисплеем LCD2004 через I2C конвертер *
 *	Адрес дисплея 0x27
 *
 *	Вывод меню из 4 строк
 */
#define DISPLAY_ADDRESS 0x27

void SystemClock_Config(void);

int main()
{
    SystemClock_Config();

    PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M; //тактирование GPIO_0_

	// LEDs init
	GPIO_0->DIRECTION_OUT =  1<<(9);
	GPIO_0->OUTPUT ^= (0b1)<<(9);
	for (volatile int i = 0; i < 10000; i++);
	GPIO_0->OUTPUT ^= (0b1)<<(9);
	for (volatile int i = 0; i < 10000; i++);
	GPIO_0->OUTPUT ^= (0b1)<<(9);

	//инициализация i2c и lcd2004
	i2c_lcd_init(1, DISPLAY_ADDRESS);
	HAL_Delay(100);

    HAL_Delay(100);							// Для корректного вывода нескольких строк, между строками нужен delay 100 msec
    lcd_mvwrite(0, 0, "1. Hello Granit");
    HAL_Delay(100);
    lcd_mvwrite(0, 1, "2. MIK32 AMUR");
    HAL_Delay(100);
    lcd_mvwrite(0, 2, "3. SCB MERIDIAN");
    HAL_Delay(100);
    lcd_mvwrite(0, 3, "4. PASCD Strong");
    HAL_Delay(100);

    lcd_on();

    while (1) {
    	GPIO_0->OUTPUT ^= (0b1)<<(9);			// LED Blink
        for (volatile int i = 0; i < 500000; i++);
    }
}

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
