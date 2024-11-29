/*
 * Пример эхо по UART in cycle while() {}
 * No HAL
 * Базовый пример, рассчитан на работу на плате с  программатором
 * mik32 Amur K1948BK018
 *
 */
#include <gpio.h>
#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <csr.h>
#include "uart.h"
#include "uart_lib.h"
#include "stdio.h"

/* Value must be less than 255 */
#define BUFFER_LENGTH   50

void ClockInit();
void UART_1_Init();

extern unsigned long __TEXT_START__;

void main() {

	ClockInit();
	UART_1_Init();

	printf("Run application...\r\n");

	while (1) {
		if (UART_1->FLAGS & UART_FLAGS_RXNE_M) {
			UART_1->FLAGS = UART_FLAGS_RXNE_M;
			UART_1->TXDATA = UART_1->RXDATA;
		}
	}

}
//---------------------------------------------------------------------

void ClockInit() {
	// interrupt vector init
		write_csr(mtvec, &__TEXT_START__);

		PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
							| PM_CLOCK_APB_P_GPIO_1_M
							| PM_CLOCK_APB_P_GPIO_2_M
							| PM_CLOCK_APB_P_UART_1_M
							;
		PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
							| PM_CLOCK_APB_M_WU_M
							| PM_CLOCK_APB_M_PM_M
							| PM_CLOCK_APB_M_EPIC_M
							;
		PM->CLK_AHB_SET |= 	  PM_CLOCK_AHB_SPIFI_M
				;
}
//----------------------------------------------------------------------

void UART_1_Init() {

	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11<<(8*2)))) | (0b01<<(8*2)); // 1.8 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11<<(9*2)))) | (0b01<<(9*2)); // 1.9 to PAD_CONTROL = 1

	GPIO_1->DIRECTION_IN =  1<<(8); 		// 1.8 make as input
	GPIO_1->DIRECTION_OUT =  1<<(9); 		// 1.9 make as output

	UART_1->DIVIDER = 32000000/9600; 		// Baudrate setup here
	UART_1->CONTROL1 = UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_UE_M;
	UART_1->FLAGS = 0xFFFFFFFF;
}
//-----------------------------------------------------------------------

size_t _write(int file, const void *ptr, size_t len) {
	for (int i = 0; i < len; i++) {
		while ((UART_1->FLAGS & UART_FLAGS_TXE_M) == 0)
			;
		UART_1->TXDATA = ((unsigned char*) ptr)[i];
	}
	return len;
}

