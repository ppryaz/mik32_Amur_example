/***************************************************************************************
 * 	Тестовый проект для микроконтроллера MIK32 AMUR
 *
 * 	Пример использования 3-х кнопок с прерыванием
 * 	При получении прерывания от кнопки,  по UART в терминале выводится сообщения о сработавшей кнопке
 *
 * 	Так же по прерыванию принимается любой символ  из терминала по UART,
 * 	загорается зеленый светодиод на плате (LEDs P0.9 - GREEN)
 * 	и отправляется сообщение в UART о получении команды
 *
 * 	Кнопки подключены к пинам P1.5, P1.7, P1.15
 * 	Светодиод P0.9 green (расположен на отладочной плате)
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3
 */

#include <gpio.h>
#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include <csr.h>
#include "uart_lib.h"
#include "xprintf.h"

extern unsigned long __TEXT_START__;

void ClockConfig(void);
void GPIO_Init(void);
void UART_1_Init(void);

//----------------------------------------------------------
void trap_handler() {
	if ( EPIC->RAW_STATUS & (1<<EPIC_GPIO_IRQ_INDEX))
	{
		if (GPIO_IRQ->INTERRUPT & (1<<7) )	{				// Interrupt P1.15 Board Button
			GPIO_IRQ->CLEAR = (1 << 7);						// set CLEAR  interrupt line here?
			xprintf("P1.15 button \r\n");
		}

		if (GPIO_IRQ->INTERRUPT & ( 1<<5)) {				// P1.5
			GPIO_IRQ->CLEAR = (1 << 5);
			xprintf("P1.5 button \r\n");
		}

		if (GPIO_IRQ->INTERRUPT & ( 1 << 3)) {				// P1.7
			GPIO_IRQ->CLEAR = (1 << 3);
			xprintf("P1.7 button \r\n");
		}
	}

	if (EPIC->RAW_STATUS & (1 << EPIC_UART_1_INDEX)) {
		if (UART_1->FLAGS & UART_FLAGS_RXNE_M) {

			UART_1->FLAGS = UART_FLAGS_RXNE_M;
			GPIO_0->OUTPUT ^= (0b1) << (9);					// Toggle green led on board

			xprintf("Получена команда по UART \r\n");
		}
		EPIC->CLEAR = (1 << EPIC_UART_1_INDEX);				// Очищаем прерывания от UART_1
	}
	EPIC->CLEAR = (1<<EPIC_GPIO_IRQ_INDEX);					// Очищаем все прерывания
}
//-----------------------------------------------------

void main() {
	ClockConfig();
	GPIO_Init();
	UART_1_Init();

	xprintf("Start ... \r\n");

	while (1) {

	}
}

//-------------------------------------------------------

void ClockConfig(void) {

	write_csr(mtvec, &__TEXT_START__);						// Interrupt vector init

	PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
							| PM_CLOCK_APB_P_GPIO_1_M
							| PM_CLOCK_APB_P_GPIO_2_M
							| PM_CLOCK_APB_P_GPIO_IRQ_M		// IRQ clock enable here*/
							| PM_CLOCK_APB_P_UART_1_M		// Включаем тактирование UART_1
							;
	PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
							| PM_CLOCK_APB_M_EPIC_M
							;
	PM->CLK_AHB_SET	  =   PM_CLOCK_AHB_SPIFI_M
							;

	// EPIC interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_GPIO_IRQ_INDEX;
	EPIC->MASK_LEVEL_SET = 1 << EPIC_UART_1_INDEX;

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
	set_csr(mie, MIE_MEIE);
}
//--------------------------------------------------------

void GPIO_Init(void) {

	GPIO_0->DIRECTION_OUT 	= 1 << (9); 		// LEDs P0.9 - RED - init as Output RED led
	GPIO_0->DIRECTION_OUT	= 1 << (10);		// LEDs P0.10 - GREEN  - init as Output

	GPIO_1->DIRECTION_IN 	= 1 << (5);			// P1.5 init as Input
	PAD_CONFIG->PORT_1_PUPD = (0b01) << (10); 	// PullUp


	GPIO_1->DIRECTION_IN 	= (0b1) << (7);		// P1.7 init as Input
	PAD_CONFIG->PORT_1_PUPD = (0b01) << (14); 	// PullUp

	GPIO_1->DIRECTION_IN 	= 1 << (15);		// P1.15 init as Input
	PAD_CONFIG->PORT_1_PUPD = (0b01) << (30); 	// PullUp

    // interrupt generation setup on P1.5, P1.7, P1.15
    GPIO_IRQ->LINE_MUX 		= GPIO_IRQ_LINE_MUX(2, 5)| GPIO_IRQ_LINE_MUX(7, 3) | GPIO_IRQ_LINE_MUX(3, 7);
    GPIO_IRQ->EDGE 			= 1 << 5 | 1 << 3 | 1 << 7;
    GPIO_IRQ->LEVEL_CLEAR	= 0 << 5 | 0 << 3 | 0 << 7;
    GPIO_IRQ->ENABLE_SET	= 1 << 5 | 1 << 3 | 1 << 7;
}
//---------------------------------------------------------

void UART_1_Init(void) {
	// UART_1 (VCP) setup
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (8 * 2))))
			| (0b01 << (8 * 2));					// 1.8 to PAD_CONTROL = 1

	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (9 * 2))))
			| (0b01 << (9 * 2));					// 1.9 to PAD_CONTROL = 1

	GPIO_1->DIRECTION_IN = 1 << (8);				// 1.8 make as RX input
	GPIO_1->DIRECTION_OUT = 1 << (9);				// 1.9 make as TX output

	UART_1->DIVIDER = 32000000 / 9600; 				// Baudrate 9600 setup here
    UART_1->CONTROL1 =
    UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_UE_M
    		| UART_CONTROL1_TCIE_M
    		| UART_CONTROL1_RXNEIE_M
    			;
    UART_1->FLAGS = 0xFFFFFFFF;
}

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
