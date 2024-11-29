/*
 * #Пример демонстрирует стирание, запись и чтение EEPROM с использованием HAL библиотеки.
 *
 * Возможные режимы тестирования
 * 	1) Write All Pages 		- стирание, запись и чтение всех страниц EEPROM
 * 	(закомментированы запись и стирание, чтобы не затирать bootloader)
 * 	В режиме чтения через UART выводится содержимое всех ячеек в терминал
 * 	2) Write Odd/Even Pages - стирание, запись и чтение четных и нечетных страниц
 * 	(закомментированы стирание, запись и чтение)
 * 	3) Write Single Pages 	- стирание, запись и чтение одной страницы
 *
 * В секции 3:  Write Single Page
 * Раскомментировать HAL_EEPROM_Erase, HAL_EEPROM_Write - для теста записи данных на конкретную страницу
 * Закомментировать HAL_EEPROM_Erase, HAL_EEPROM_Write - записанные данные сохранятся, и будут выведены в терминал
 *
 * uint16_t address = 7680 == обращение к странице 60.
 * Расчет адреса: адрес = номер_стреницы * кол-во_слов_на_странице * 4
 *
 */

#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include <csr.h>

#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_eeprom.h"
#include "mik32_hal.h"
#include "uart.h"
#include "uart_lib.h"
#include "xprintf.h"

extern unsigned long __TEXT_START__;

#define EEPROM_OP_TIMEOUT 100000
#define USART_TIMEOUT 1000
#define EEPROM_PAGE_WORDS 32
#define EEPROM_PAGE_COUNT 64

#define STATUS_LED_PORT GPIO_0
#define STATUS_LED_PIN PORT0_9


HAL_EEPROM_HandleTypeDef heeprom;

void SystemClock_Config();
void ClockConfig(void);

void UART_1_init(void);
void EEPROM_Init();
void GPIO_Init();

int main() {
	SystemClock_Config();
	ClockConfig();
	GPIO_Init();

	UART_1_init();
    EEPROM_Init();

    xprintf("\n==== HAL_EEPROM Example ====\r\n");

    uint32_t write_data_buf[EEPROM_PAGE_WORDS] = {};
    uint32_t read_data_buf[EEPROM_PAGE_WORDS] = {};


//-------------------------------------------------------------------------------------------------
//------------------    1) Write All Pages  -------------------------------------------------------
//-------------------------------------------------------------------------------------------------

    xprintf("\n==== 1) Write All Pages ====\r\n");

    int result1 = 0;

    xprintf("Erasing...\r\n");
    // HAL_EEPROM_Erase(&heeprom, 0, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ALL, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++) write_data_buf[i] = 0x55555555;

    xprintf("Writing...\r\n");
    // HAL_EEPROM_Write(&heeprom, 0, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ALL, EEPROM_OP_TIMEOUT);

    xprintf("Reading...\r\n");
	for (int s = 0; s < EEPROM_PAGE_COUNT; s++) {
		HAL_EEPROM_Read(&heeprom, s * EEPROM_PAGE_WORDS * 4, read_data_buf,
				EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);
		xprintf("\r\nPage  %d\r\n", s);

		for (int i = 0; i < EEPROM_PAGE_WORDS; i++) {
			xprintf(" && %d", read_data_buf[i]);
			if (write_data_buf[i] != 0x55555555) result1 = 1;
		}

		HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
	}

	if (result1) {
		xprintf("Error detected!\r\n");
	} else {
		xprintf("OK!\r\n");
	}

//--------------------------------------------------------------------------------------------------
//---------------	 2) Write Odd/Even Pages 	----------------------------------------------------
//--------------------------------------------------------------------------------------------------
    xprintf("\n= 2) Write Odd/Even Pages  =\r\n");

    int result2 = 0;

    HAL_EEPROM_SetMode(&heeprom, HAL_EEPROM_MODE_THREE_STAGE);

    xprintf("Erasing Odd...\r\n");
    // HAL_EEPROM_Erase(&heeprom, 0, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ODD, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++) write_data_buf[i] = 0x55555555;


    xprintf("Writing Odd...\r\n");
    // HAL_EEPROM_Write(&heeprom, 0, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_ODD, EEPROM_OP_TIMEOUT);


    xprintf("Erasing Even...\r\n");
    // HAL_EEPROM_Erase(&heeprom, 0 * 4, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_EVEN, EEPROM_OP_TIMEOUT);

    for (int i = 0; i < EEPROM_PAGE_WORDS; i++) write_data_buf[i] = 0xAAAAAAAA;


    xprintf("Writing Even...\r\n");
    // HAL_EEPROM_Write(&heeprom, 0 * 4, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_EVEN, EEPROM_OP_TIMEOUT);


    xprintf("Reading...\r\n");
    for (int s = 0; s < EEPROM_PAGE_COUNT; s += 2)  {
    // HAL_EEPROM_Read(&heeprom, 0 * EEPROM_PAGE_WORDS * 4, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

		for (int i = 0; i < EEPROM_PAGE_WORDS; i++) if (read_data_buf[i] != 0x55555555) result2 = 1;

		// HAL_EEPROM_Read(&heeprom, (0 + 1) * EEPROM_PAGE_WORDS * 4, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

		for (int i = 0; i < EEPROM_PAGE_WORDS; i++) if (read_data_buf[i] != 0xAAAAAAAA) result2 = 1;

		HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
	}

    HAL_EEPROM_SetMode(&heeprom, HAL_EEPROM_MODE_TWO_STAGE);

	if (result2) {
		xprintf("Error detected!\r\n");
	} else {
		xprintf("OK!\r\n");
	}
//--------------------------------------------------------------------------------------------------
//-------------------------    3) Write Single Pages -----------------------------------------------
//--------------------------------------------------------------------------------------------------

	xprintf("\n== 3) Write Single Pages  ==\r\n");

    int result3 = 0;

    xprintf("Erasing...\r\n");
    HAL_EEPROM_Erase(&heeprom, 7680, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);

	for (int i = 0; i < EEPROM_PAGE_WORDS; i++) write_data_buf[i] = i;

    xprintf("Writing...\r\n");
    HAL_EEPROM_Write(&heeprom, 7680, write_data_buf, EEPROM_PAGE_WORDS, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);

    xprintf("Reading...\r\n");
    HAL_EEPROM_Read(&heeprom, 7680, read_data_buf, EEPROM_PAGE_WORDS, EEPROM_OP_TIMEOUT);

	for (int i = 0; i < EEPROM_PAGE_WORDS; i++) {
		xprintf(" && %d", read_data_buf[i]);
		if (read_data_buf[i] != i) result3 = 1;
	}

	if (result3) {
		xprintf("Error detected!\r\n");
	} else {
		xprintf("OK!\r\n");
	}

    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_HIGH);

    int result = result1 | result2 | result3;

	if (result) {
		xprintf("\n====== Result: Error  ======\r\n");
	} else {
		xprintf("\n======== Result: OK ========\r\n");
	}

	while (1) {
		if (result) {
			HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
			HAL_DelayMs(500);
		}
	}
}
//--------------------------------------------------------------------------------------------------

void SystemClock_Config(void) {
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
//--------------------------------------------------------------------------------------------------

void ClockConfig(void) {
	// interrupt vector init
	write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET =
			PM_CLOCK_APB_P_GPIO_0_M
		  | PM_CLOCK_APB_P_GPIO_1_M
		  | PM_CLOCK_APB_P_GPIO_2_M
		  | PM_CLOCK_APB_P_UART_1_M;

	PM->CLK_APB_M_SET =
			PM_CLOCK_APB_M_PAD_CONFIG_M
		  | PM_CLOCK_APB_M_WU_M
		  | PM_CLOCK_APB_M_PM_M
		  | PM_CLOCK_APB_M_EPIC_M;

	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;
}

//--------------------------------------------------------------------------------------------------

void UART_1_init(void) {
    UART_1->DIVIDER = 32000000/9600; // Baudrate 9600 setup here
    UART_1->CONTROL1 =
    		  UART_CONTROL1_TE_M
    		| UART_CONTROL1_RE_M
			| UART_CONTROL1_UE_M
			//| UART_CONTROL1_TCIE_M		// Enable interrupt here
			//| UART_CONTROL1_RXNEIE_M		// Interrupt if RX register not empty
			;
    UART_1->FLAGS = 0xFFFFFFFF;

	// interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_UART_1_INDEX;

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}
//--------------------------------------------------------------------------------------------------

void EEPROM_Init() {
    heeprom.Instance = EEPROM_REGS;
    heeprom.Mode = HAL_EEPROM_MODE_TWO_STAGE;
    heeprom.ErrorCorrection = HAL_EEPROM_ECC_ENABLE;
    heeprom.EnableInterrupt = HAL_EEPROM_SERR_DISABLE;

    HAL_EEPROM_Init(&heeprom);
    HAL_EEPROM_CalculateTimings(&heeprom, OSC_SYSTEM_VALUE);
}
//--------------------------------------------------------------------------------------------------

void GPIO_Init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    GPIO_InitStruct.Pin = STATUS_LED_PIN;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(STATUS_LED_PORT, &GPIO_InitStruct);

	// UART_1 (VCP) setup
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (8 * 2))))
			| (0b01 << (8 * 2));		// 1.8 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (9 * 2))))
			| (0b01 << (9 * 2));		// 1.9 to PAD_CONTROL = 1

	GPIO_1->DIRECTION_IN = 1 << (8);	// 1.8 make as input RX
	GPIO_1->DIRECTION_OUT = 1 << (9);	// 1.9 make as output TX
}
//--------------------------------------------------------------------------------------------------


void xputc(char c) {							// for xprintf()
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}

