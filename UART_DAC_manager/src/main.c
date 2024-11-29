/*
 * 	************  DAC terminal manager  ********************************************************
 *
 *  This program module controls two DAC channels by receiving UART settings from the terminal.
 *  After that, information about the sent command is output to the terminal.
 *
 * 	UART setting:	Baudrate  	9600
 * 					data bits 	8
 * 					parity		none
 * 					stop bits	1
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3
 *
 *  FORMAT command string: five numeric symbols,  12001 understand like 1200 mV and last symbol is 1 channel of DAC module.
 *
 *  Note!:  This test version of the program does not have a check for the correctness of the entered string.
 *  		Valid value of voltage is from 0  to 1200 mV, valid channel is 1 and 2.
 *  		String of a different format will lead to unpredictable behavior of the application.
 */

#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "mik32_hal_dac.h"
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include <csr.h>
#include <gpio.h>
#include <uart.h>
#include <stdio.h>
#include "stdlib.h"
#include "xprintf.h"
#include "uart_lib.h"


extern unsigned long __TEXT_START__;

#define BUFFER_SIZE 	5
#define VOLT_PER_UNIT 	0.00029296875


void ClockConfig(void);
void GPIO_init(void);
void UART_1_init(void);

DAC_HandleTypeDef hdac1;
DAC_HandleTypeDef hdac2;
static void DAC_Init(void);

uint32_t convertToDAC(uint32_t val);
void setDAC(uint8_t* buff);

uint8_t comBuffer[BUFFER_SIZE];
uint8_t buf_pointer = 0;
uint8_t DACchannel = 2;

bool flag_txc = 0;

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1<<EPIC_UART_1_INDEX))
	{
		if (UART_1->FLAGS & UART_FLAGS_TC_M)
		{
			UART_1->FLAGS = UART_FLAGS_TC_M;
			flag_txc = 1;								// Флаг мигания светодиодом
		}
		if (UART_1->FLAGS & UART_FLAGS_RXNE_M) {
			UART_1->FLAGS = UART_FLAGS_RXNE_M;

			comBuffer[buf_pointer] = UART_1->RXDATA;

			++buf_pointer;
			if (buf_pointer >= BUFFER_SIZE) {
				setDAC(comBuffer);						// Отправка команды
				buf_pointer = 0;
			}
		}
		EPIC->CLEAR = (1<<EPIC_UART_1_INDEX);
	}
}
//-------------------------------------------------------

void main() {

	ClockConfig();
	GPIO_init();
	UART_1_init();
	DAC_Init();

	while (1)
	{
		if (flag_txc)
		{
			GPIO_0->CLEAR = 1<<(9);						// Short blink led then DATA receiving :)
			for (volatile int i = 0; i < 10000; i++);
			GPIO_0->SET = 1<<(9);
			flag_txc = 0;
		}
	}
}
//-----------------------------------------------------

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
//------------------------------------------------------

void GPIO_init(void) {
	// LEDs P0.9 init as Output
	GPIO_0->DIRECTION_OUT = 1 << (9);
	// LEDs P0.10 init as Output
	GPIO_0->DIRECTION_OUT = 1 << (10);

	GPIO_0->SET = 1 << (9);
	GPIO_0->SET = 1 << (10);

	// UART_1 (VCP) setup
	// 1.8 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (8 * 2))))
			| (0b01 << (8 * 2));
	// 1.9 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (9 * 2))))
			| (0b01 << (9 * 2));
	// 1.8 make as input RX
	GPIO_1->DIRECTION_IN = 1 << (8);
	// 1.9 make as output TX
	GPIO_1->DIRECTION_OUT = 1 << (9);
}
//--------------------------------------------------------

void UART_1_init(void) {
    UART_1->DIVIDER = 32000000/9600; // Baudrate 9600 setup here
    UART_1->CONTROL1 =
    		  UART_CONTROL1_TE_M
    		| UART_CONTROL1_RE_M
			| UART_CONTROL1_UE_M
			| UART_CONTROL1_TCIE_M
			| UART_CONTROL1_RXNEIE_M
			;
    UART_1->FLAGS = 0xFFFFFFFF;

	// interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_UART_1_INDEX;

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}
//----------------------------------------------------------

static void DAC_Init(void) {
	hdac1.Instance = ANALOG_REG;

	hdac1.Instance_dac = HAL_DAC1;
	hdac1.Init.DIV = 0;
	hdac1.Init.EXTRef = DAC_EXTREF_OFF; /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
	hdac1.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

	HAL_DAC_Init(&hdac1);

	hdac2.Instance = ANALOG_REG;

	hdac2.Instance_dac = HAL_DAC2;
	hdac2.Init.DIV = 0;
	hdac2.Init.EXTRef = DAC_EXTREF_OFF; /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
	hdac2.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

	HAL_DAC_Init(&hdac2);
}
//------------------------------------------------------

uint32_t convertToDAC(uint32_t val) {
	float vpu = VOLT_PER_UNIT;
	uint32_t res = (uint32_t)((val / vpu)/10000);	// Конвертируем полученное значение в милливольтах в единицы DAC
	return (res < 4096)? res : 4095; 				// Проверяем "потолок" в единицах DAC
}
//------------------------------------------------------

void setDAC(uint8_t *buff) {
	uint16_t res;
	uint32_t tmp;

	tmp = atoi(buff);							//

	uint8_t last_pos = 4;						// Позиция в строке где хранится номер канала
	DACchannel = (uint8_t)buff[last_pos] - 48;	// Получаем номер канала

	xprintf("Input value %d/1000 ", tmp/10);

	res = convertToDAC(tmp);					// Преобразуем милливольты в единицы DAC

	if (DACchannel == 1) {
		HAL_DAC_SetValue(&hdac1, res);

	} else if (DACchannel == 2) {
		HAL_DAC_SetValue(&hdac2, res);
	}
	xprintf("- Set Channel %d - Sent to DAC %d\r\n", DACchannel, res);
}
//-------------------------------------------------------


void xputc(char c) {							// for xprintf()
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}

