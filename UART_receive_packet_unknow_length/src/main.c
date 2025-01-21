/*********  UART_receive_unknown_length project  ****************
*
*	Прием пакетов заранее неизвестной длины по UART на микроконтроллере MIK32 Amur
*
*	 В проекте так же есть следующие возможности:
*	 1) Переключение красного светодиода (PORT0_10) в основном цикле
*	 2) Переключение GreenLed (PORT0_9) по кнопке на P1.15 with interrupt
*	 3) Print to terminal virtual COM-port text "Start..."
*	 4) UART Echo 
*
*	  	MCU type: MIK32 AMUR К1948ВК018
*	 	Board: отладочная плата v0.3
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

#include "uart_lib.h"
#include "xprintf.h"

#define GPIO_X ((GPIO_TypeDef*)GPIO_0_BASE_ADDRESS )
#define MAX_UART_BUFFER_SIZE	128

extern unsigned long __TEXT_START__;

static void ClockConfig(void);
static void GPIO_Init(void);

volatile uint32_t dataBufSize;
volatile uint16_t dataBuf[MAX_UART_BUFFER_SIZE];

void trap_handler() {

	if ( EPIC->RAW_STATUS & (1<<EPIC_UART_1_INDEX)) {
			if (UART_1->FLAGS & UART_FLAGS_IDLE_M) {
				UART_1->FLAGS = UART_FLAGS_IDLE_M;

				UART_Write(UART_1, dataBuf, dataBufSize);	// Обработка полученного по UART пакета данных
				dataBufSize = 0;
			}
			if (UART_1->FLAGS & UART_FLAGS_RXNE_M) {

				UART_1->FLAGS = UART_FLAGS_RXNE_M;
				GPIO_0->OUTPUT ^= (0b1)<<(9);
				
				dataBuf[dataBufSize] = UART_1->RXDATA;	// Тут надо обработать получение данных по uart
				++dataBufSize;
			}

			EPIC->CLEAR = (1 << EPIC_UART_1_INDEX);
		}
		
	if ( EPIC->RAW_STATUS & (1 << EPIC_GPIO_IRQ_INDEX)) {
		
		if (GPIO_IRQ->INTERRUPT & (1 << 7)) {	// Ожидаем прерывания от кнопки на PORT1_15
			GPIO_0->OUTPUT ^= (0b1)<<(9);		// Переключаем светодиод на PORT0_9
			GPIO_IRQ->CLEAR = (1 << 7);
		}
		EPIC->CLEAR = (1 << EPIC_GPIO_IRQ_INDEX);
	}
}

void main() {
	ClockConfig();
	UART_Init(UART_1, 32000000/9600, UART_CONTROL1_TE_M			// Разрешить передачу TX
			| UART_CONTROL1_RE_M		// Разрешить прием RX
			| UART_CONTROL1_UE_M		// Включаем UART
			| UART_CONTROL1_IDLEIE_M	// Управление прерыванием при отсутствии входных транзакций
			| UART_CONTROL1_RXNEIE_M	// Управление прерыванием при успешном приеме данных
	, 0, 0);

	UART_1->FLAGS = 0xFFFFFFFF;

	EPIC->MASK_LEVEL_SET = 1 << EPIC_UART_1_INDEX;
	
	GPIO_Init();

    xprintf("Start... \r\n");

	while (1)
	{
		GPIO_0->OUTPUT ^= (0b1)<<(10);
		for (volatile int i = 0; i < 300000; i++);
	}
}
//---------------------------------------------------------------------------------

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
//-------------------------------------------------------------------------------------------------

static void ClockConfig(void) {
	write_csr(mtvec, &__TEXT_START__); 		// interrupt vector init

	PM->CLK_APB_P_SET	=   PM_CLOCK_APB_P_GPIO_0_M
							| PM_CLOCK_APB_P_GPIO_1_M
							| PM_CLOCK_APB_P_GPIO_2_M
							| PM_CLOCK_APB_P_GPIO_IRQ_M
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
	// LEDs init
	GPIO_0->DIRECTION_OUT =  1<<(9);
	GPIO_0->DIRECTION_OUT =  1<<(10);

	// extint on user key setup begin
	GPIO_1->DIRECTION_IN =  1<<(15);

		// Interrupt PORT1_15
	GPIO_IRQ->LINE_MUX =GPIO_IRQ_LINE_MUX(3, 7);
	GPIO_IRQ->EDGE 			|= 1 << 7;		// EDGE mode
	GPIO_IRQ->LEVEL_CLEAR 	|= 1 << 7;		// Falling edge
	GPIO_IRQ->ENABLE_SET 	|= 1 << 7;		// Enable 7th line

	// interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_GPIO_IRQ_INDEX;
}
