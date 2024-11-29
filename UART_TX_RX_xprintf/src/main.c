/*
 * Эхо UART посимвольно, плюс вывод отладочной информации с помощью xprintf
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
#include <gpio.h>
#include <uart.h>

extern unsigned long __TEXT_START__;

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1 << EPIC_GPIO_IRQ_INDEX)) {
		GPIO_0->OUTPUT ^= (0b1) << (9);
		GPIO_IRQ->CLEAR = (1 << 7);
		EPIC->CLEAR = (1 << EPIC_GPIO_IRQ_INDEX);
	}
}

void main() {

	// interrupt vector init
	write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_0_M | PM_CLOCK_APB_P_GPIO_1_M
			| PM_CLOCK_APB_P_GPIO_2_M | PM_CLOCK_APB_P_UART_1_M;
	PM->CLK_APB_M_SET = PM_CLOCK_APB_M_PAD_CONFIG_M | PM_CLOCK_APB_M_WU_M
			| PM_CLOCK_APB_M_PM_M | PM_CLOCK_APB_M_EPIC_M;
	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;

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
	// 1.8 make as input
	GPIO_1->DIRECTION_IN = 1 << (8);
	// 1.9 make as output
	GPIO_1->DIRECTION_OUT = 1 << (9);
	UART_1->DIVIDER = 32000000 / 9600; // Baudrate 9600 setup here
	UART_1->CONTROL1 =
	UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_UE_M
	//		| UART_CONTROL1_TCIE_M
	//		| UART_CONTROL1_RXNEIE_M
			;
	UART_1->FLAGS = 0xFFFFFFFF;

	// interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_UART_1_INDEX;

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
	set_csr(mie, MIE_MEIE);


	xprintf("Test");

	while (1) {
		GPIO_0->OUTPUT ^= (0b1) << (10);  		// изменяем состояние светодиода
		for (volatile int i = 0; i < 300000; i++)
			;	// пауза
		if (UART_1->FLAGS & UART_FLAGS_RXNE_M)		// получены ли данные?
		{
			UART_1->FLAGS = UART_FLAGS_RXNE_M; // сбрасываем флаг принятия данных
			UART_1->TXDATA = UART_1->RXDATA; // отправляем полученное обратно в UART
		}

	}
}

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}

