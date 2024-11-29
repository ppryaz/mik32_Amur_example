
/*********  Сканер устройств на шине I2C. Проверка доступности прибора по известному адресу  ****************
*
* В данном примере демонстрируется работа I2C в режиме ведущего.
* Используется I2C_1
* P.1.12 SDA
* P.1.13 SCL
*
* Отображаются  адреса всех подключенных и исправных устройств с интерфейсом I2C
*
*
*	  	MCU type: MIK32 AMUR К1948ВК018
*	 	Board: отладочная плата v0.3
*/
#include "mcu32_memory_map.h"
#include "pad_config.h"
#include "gpio.h"
#include "power_manager.h"
#include "wakeup.h"
#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_i2c.h"
#include "stdlib.h"

#include "uart_lib.h"
#include "xprintf.h"


I2C_HandleTypeDef hi2c1; 	// Дескриптор шины i2c
int column_counter = 0;		// Счетчик столбцов, для красивого вывода

void I2C1_Init(void);
void SystemClock_Config(void);
HAL_I2C_ErrorTypeDef check_I2C_Adderss(uint16_t slaveAdderss);

int main() {
	initClock();
	SystemClock_Config();
	UART_Init(UART_1, 3333, UART_CONTROL1_TE_M |UART_CONTROL1_M_8BIT_M, 0, 0);
	I2C1_Init();

    HAL_StatusTypeDef status;

    xprintf("Start scan ... \r\n");


	for (uint8_t i = 0; i < 128; ++i) {

		HAL_I2C_ErrorTypeDef err = check_I2C_Adderss(i);//hi2c1.ErrorCode;

		if (err == I2C_ERROR_NONE) 	xprintf("0x%x  ", i);
		else						xprintf("  .   ");

		HAL_I2C_Reset(&hi2c1);	// Обязательный сброс всех специфичных для текущего адреса настроек

		// Format table here
		++column_counter;

		if (column_counter > 15) {
			column_counter = 0;
			xprintf("\r\n");
		}
	}

	xprintf("\r\nStop scan ... \r\n");

    while (1)
    {
    }
}
//---------------------------------------------------------------

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
//------------------------------------------------------------------

void initClock() {
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_0_M|PM_CLOCK_APB_P_GPIO_1_M|PM_CLOCK_APB_P_GPIO_2_M;
	PM->CLK_APB_M_SET |= PM_CLOCK_APB_M_PAD_CONFIG_M | PM_CLOCK_APB_M_WU_M| PM_CLOCK_APB_M_PM_M;
}
//------------------------------------------------------------------

void I2C1_Init(void) {
	// Common setting
    hi2c1.Instance = I2C_1;
    hi2c1.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c1.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c1.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c1.Init.AutoEnd = I2C_AUTOEND_ENABLE;

    // Frequency setting
    hi2c1.Clock.PRESC = 5;
    hi2c1.Clock.SCLDEL = 15;
    hi2c1.Clock.SDADEL = 15;
    hi2c1.Clock.SCLH = 15;
    hi2c1.Clock.SCLL = 15;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        xprintf("I2C_Init error\n");
    }
}
//-----------------------------------------------------------------

HAL_I2C_ErrorTypeDef check_I2C_Adderss(uint16_t slaveAdderss) {
	uint32_t status_isr = 0;
	uint32_t Timeout = 10000;			// Min 10000 timeout value

	uint32_t nbytes 	= 1;			// Количество байт которые мы собираемя передавать
	uint8_t data[1];					// Массив изкторого передаем данные

	hi2c1.Instance->CR2 &= ~I2C_CR2_NBYTES_M;		// Очистим счетчик передаваемых байт
	hi2c1.Instance->CR2 |= I2C_CR2_NBYTES(nbytes);	// Мы собираемся передать 1 байт. После этого сработает автоокончание AutoEnd

	HAL_I2C_AutoEnd(&hi2c1, hi2c1.Init.AutoEnd);

	HAL_I2C_Master_SlaveAddress(&hi2c1, slaveAdderss);	// Устанавливаем проеряемый адрес

	hi2c1.Instance->CR2 &= ~I2C_CR2_RD_WRN_M;	// Задать направление передачи - запись
	hi2c1.Instance->CR2 |= I2C_CR2_START_M;		// Стартуем

	HAL_I2C_Master_WaitTXIS(&hi2c1, Timeout);
	hi2c1.Instance->TXDR = *data;				// Чтобы инициировать сигнал STOP

	HAL_I2C_ErrorTypeDef ret = hi2c1.ErrorCode;	// Получим результат обращения к текущему адресу
	return ret;
}


//-------------------------------------------------------

void xputc(char c) {							// for xprintf()
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}

