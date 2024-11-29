/*
 *	Тестовый проект UART эхо
 *	Управление UART через прерывания. Каждый полученный символ обозначается миганием светодиода
 *	Используемые компоненты UART_1, GreenLED на P0.09
 *	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3
 */
#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include <csr.h>
#include <gpio.h>
#include <uart.h>
#include "uart_lib.h"
#include "xprintf.h"

extern unsigned long __TEXT_START__;

static void ClockConfig(void);
static void GPIO_Init(void);
static void UART_1_Init(void);

bool flag_txc = 0;

//--------------------------------------------------------------------------------------------------

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1<<EPIC_UART_1_INDEX)) {
		if (UART_1->FLAGS & UART_FLAGS_TC_M) {

			UART_1->FLAGS = UART_FLAGS_TC_M;
			flag_txc = 1;
		}
		if (UART_1->FLAGS & UART_FLAGS_RXNE_M) {

			UART_1->FLAGS = UART_FLAGS_RXNE_M;
			UART_1->TXDATA = UART_1->RXDATA;
		}

		EPIC->CLEAR = (1 << EPIC_UART_1_INDEX);
	}
}
//--------------------------------------------------------------------------------------------------

void main() {
	ClockConfig();
	GPIO_Init();
	UART_1_Init();

	while (1) {
		if (flag_txc) {

			GPIO_0->CLEAR = 1 << (9);
			for (volatile int i = 0; i < 10000; i++);

			GPIO_0->SET = 1 << (9);
			flag_txc = 0;
		}
	}
}
//-------------------------------------------------------------------------------------------------

static void ClockConfig(void) {
	write_csr(mtvec, &__TEXT_START__); 		// interrupt vector init

	PM->CLK_APB_P_SET	=   PM_CLOCK_APB_P_GPIO_0_M
							| PM_CLOCK_APB_P_GPIO_1_M
							| PM_CLOCK_APB_P_GPIO_2_M
							| PM_CLOCK_APB_P_UART_1_M;

	PM->CLK_APB_M_SET 	=  PM_CLOCK_APB_M_PAD_CONFIG_M
							| PM_CLOCK_APB_M_WU_M
							| PM_CLOCK_APB_M_PM_M
							| PM_CLOCK_APB_M_EPIC_M;

	PM->CLK_AHB_SET 	|= PM_CLOCK_AHB_SPIFI_M;

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
	set_csr(mie, MIE_MEIE);
}
//---------------------------------------------------------------------------------------------------

static void GPIO_Init(void) {
	GPIO_0->DIRECTION_OUT = (0b1) << (9);		// LED P0.09 set as output
	GPIO_0->DIRECTION_OUT = (0b1) << (10);		// LED P0.10 set as output

	GPIO_0->SET = (0b1) << (9);					// P0.09 set 1
	GPIO_0->SET = (0b1) << (10); 				// P0.10 set 1


	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11<<(8*2)))) | (0b01<<(8*2)); 	// P1.8 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11<<(9*2)))) | (0b01<<(9*2));	// P1.9 to PAD_CONTROL = 1

	GPIO_1->DIRECTION_IN =  1<<(8);					// 1.8 make as input
	GPIO_1->DIRECTION_OUT =  1<<(9);				// 1.9 make as output
}
//--------------------------------------------------------------------------------------------------

static void UART_1_Init(void) {
	UART_1->DIVIDER = 32000000/9600; 				// Baudrate 9600 setup
	UART_1->CONTROL1 = 	UART_CONTROL1_TE_M			// Разрешить передачу TX
						| UART_CONTROL1_RE_M		// Разрешить прием RX
						| UART_CONTROL1_UE_M		// Включаем UART
						| UART_CONTROL1_TCIE_M		// Управление прерыванием пр успешной передаче данных
						| UART_CONTROL1_RXNEIE_M;	// Управление прерыванием при успешном приеме данных

	UART_1->FLAGS = 0xFFFFFFFF;

	EPIC->MASK_LEVEL_SET = 1 << EPIC_UART_1_INDEX;
}
