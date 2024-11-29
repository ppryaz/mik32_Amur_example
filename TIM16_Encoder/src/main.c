/*
 *  Тестовый проект Timer16 в режиме Encoder
 *
 *  Функионал проекта:
 *  1) При вращении энкодера в терминал выводится текущее состояние счетчика (положение энкодера)
 *  2) Нажатие на кнопку энкодера переключает зеленый светодиод на отладочной плате (используется прерывание по кнопке)
 *
 * 	Подключение:
 * 	Подключить GND и + энкодера к питанию
 * 	SW кнопка энкодера 	- к P1.15
 * 	CLK на энкодере 	- к P0.05
 * 	DT на энкодере		- к P0.06
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
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

#include "mik32_hal_pcc.h"
#include "mik32_hal_timer16.h"

#include "uart_lib.h"
#include "xprintf.h"

extern unsigned long __TEXT_START__;

Timer16_HandleTypeDef htimer16_0;

uint16_t periodEncoder;

static void ClockConfig(void);
static void GPIO_Init(void);
static void UART_1_Init(void);
static void Timer16_0_Init(void);

//-------------------------------------------------------------------------------------------------

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1<<EPIC_GPIO_IRQ_INDEX))
	{
		GPIO_0->OUTPUT ^= (0b1)<<(9);
		GPIO_IRQ->CLEAR = (1 << 7);
		EPIC->CLEAR = (1<<EPIC_GPIO_IRQ_INDEX);
	}
}
//--------------------------------------------------------------------------------------------------

void main() {

	ClockConfig();
	GPIO_Init();
	UART_1_Init();

	periodEncoder = 50; 											// Диапазон значений энкодера (от 0 до 49)

    xprintf("\r...Start... \r\n");


    PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << 2 * TIMER16_0_IN1); // Включить вывод Input1 для Timer16_0, Port0.5
	PAD_CONFIG->PORT_0_CFG |= (PORT_AS_TIMER << 2 * TIMER16_0_IN2); // Включить вывод Input2 для Timer16_0, Port0.6

	Timer16_0_Init();
	HAL_Timer16_Encoder_Start(&htimer16_0, periodEncoder);

	while (1) {
		xprintf("Counter = %d\r\n", HAL_Timer16_GetCounterValue(&htimer16_0)); // Вывод значения счетчика

		GPIO_0->OUTPUT ^= 1 << (10);
		for (volatile int i = 0; i < 300000; i++);
	}
}
//--------------------------------------------------------------------------------------------------

static void ClockConfig(void) {
	write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
						| PM_CLOCK_APB_P_GPIO_1_M
						| PM_CLOCK_APB_P_GPIO_2_M
						| PM_CLOCK_APB_P_GPIO_IRQ_M
						| PM_CLOCK_APB_P_UART_1_M
						| PM_CLOCK_APB_P_TIMER16_1_M;

	PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
						| PM_CLOCK_APB_M_WU_M
						| PM_CLOCK_APB_M_PM_M
						| PM_CLOCK_APB_M_EPIC_M;

	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;					// enable spifi

	set_csr(mstatus, MSTATUS_MIE);								// global interrupt enable
	set_csr(mie, MIE_MEIE);
}
//--------------------------------------------------------------------------------------------------

static void GPIO_Init(void) {
	GPIO_0->DIRECTION_OUT = 1 << (10);				// Led Pin 0.09
	GPIO_0->DIRECTION_OUT = (0b1) << (9);			// Led Pin 0.10


	GPIO_1->DIRECTION_IN =  1<<(15);				// externsl interrupt on user key setup begin P1.15

	// interrupt generation setup
	GPIO_IRQ->LINE_MUX = GPIO_IRQ_LINE_MUX(3, 7);	// 7th line <- Mode=3
	GPIO_IRQ->EDGE = 1 << 7;						// EDGE mode
	GPIO_IRQ->LEVEL_CLEAR = 1 << 7;					// falling edge
	GPIO_IRQ->ENABLE_SET = 1 << 7;					// enable 7th line


	EPIC->MASK_LEVEL_SET = 1 << EPIC_GPIO_IRQ_INDEX; // interrupt reception setup
}
//--------------------------------------------------------------------------------------------------

static void UART_1_Init(void) {
	//UART_Init(UART_1, 3333, UART_CONTROL1_TE_M|UART_CONTROL1_RE_M|UART_CONTROL1_UE_M|UART_CONTROL1_M_8BIT_M, 0 ,0);
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
//--------------------------------------------------------------------------------------------------

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
//--------------------------------------------------------------------------------------------------

static void Timer16_0_Init(void)
{
    htimer16_0.Instance = TIMER16_0;

    /* Настройка тактирования */
    htimer16_0.Clock.Source = TIMER16_SOURCE_INTERNAL_SYSTEM;
    htimer16_0.CountMode = TIMER16_COUNTMODE_INTERNAL;  /* При тактировании от Input1 не имеет значения */
    htimer16_0.Clock.Prescaler = TIMER16_PRESCALER_1;
    htimer16_0.ActiveEdge = TIMER16_ACTIVEEDGE_RISING;  /* Выбирается при тактированиии от Input1 */

    /* Настрйока режима обновления регистра ARR и CMP */
    htimer16_0.Preload = TIMER16_PRELOAD_AFTERWRITE;

    /* Настройка триггера */
    htimer16_0.Trigger.Source = TIMER16_TRIGGER_TIM1_GPIO1_9;
    htimer16_0.Trigger.ActiveEdge = TIMER16_TRIGGER_ACTIVEEDGE_SOFTWARE;    /* При использовании триггера значение доложно быть отлично от software */
    htimer16_0.Trigger.TimeOut = TIMER16_TIMEOUT_DISABLE;   /* Разрешить повторное срабатывание триггера */

    /* Настройки фильтра */
    htimer16_0.Filter.ExternalClock = TIMER16_FILTER_8CLOCK;
    htimer16_0.Filter.Trigger = TIMER16_FILTER_8CLOCK;

    /* Настройка режима энкодера */
    htimer16_0.EncoderMode = TIMER16_ENCODER_ENABLE;

    HAL_Timer16_Init(&htimer16_0);
}


