/*********   W5500 Ethernet test  ****************
*
*	 В проекте реализуются следующие возможности:
*	 Соединение MIK32 Amur с компьютером через Ethernetс помощью сетевого модуля W5500 TCP/IP(Ethernet)
*	 Передача данных от компьютера к микроконтроллеру
*	 Передача данных от микроконтроллера к компьютеру
*
*	 Прошивка выводит в терминал текущие сетевые настройки. В соответствие с ними задать настройки внешней программы тестирования.
*	 Пример: программа Putty
*	 Выбрать: ConnectionType->Other->Raw
*	 При назначении статического адреса:
*	 ip: 192.168.1.245
*	 tcp port: 5577
*	 И открыть соединение...
*	 Отправить тестовую строку - "Hi MIK32"
*
*	 Дополнительно, по прерыванию от кнопки на плате (PORT1_15)  включается зеленый тестовый светодиод
*
*	 Для подключения модуля к MIK32 Amur используется SPI_1
*	 Питание модуля W5500  желательно осуществлять от отдельного источника питания (т.к. потребление до 185 мА, от микроконтроллера может не хватить)
*
*	PORT1_0 - MISO with PullUp  		-> W5500 MISO
*	PORT1_1 - MOSI 						-> W5500 MOSI
*	PORT1_2 - CLK						-> W5500 SCK
*	PORT1_4 - SPI1_SS_Out_1 PullUp		-> W5500 SS
*
*	W5500 - настройки
*	wizchip_conf.h - chip setting
*	#define _WIZCHIP_  W5500   // W5100, W5100S, W5200, W5300, W5500
*
*	W5500.h : настройка портов и сокетов
*			UDP port 5578
*			TCP port 5577
*	W5500.c tWIZNETINFO -  настройка статического адреса или получения динамических настроек сети (DHCP): NETINFO_STATIC, NETINFO_DHCP
*
*	TODO: Нестабильно плохо работает получение адреса по DHCP
*
**	  	MCU type: MIK32 AMUR К1948ВК018
*	 	Board: отладочная плата v0.3
*/

#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include "timer32.h"
#include <csr.h>
#include <gpio.h>
#include "uart_lib.h"
#include "xprintf.h"
#include "string.h"

#include "Internet/DHCP/dhcp.h"
#include "Ethernet/w5500.h"
#include "Ethernet/wizchip_conf.h"




#define DEVICE_ID 1025
#define FIRMWARE_VERSION "1.0.2"


extern wiz_NetInfo gWIZNETINFO;
extern unsigned long __TEXT_START__;

SPI_HandleTypeDef hspi1;

// from CommandProtocol
uint8_t subscriberAddress[4];
uint16_t subscriberPort;

//-----------------------------------------------------------------------

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1<<EPIC_GPIO_IRQ_INDEX)) 	{
		GPIO_0->OUTPUT ^= (0b1)<<(9);
		GPIO_IRQ->CLEAR = (1 << 7);
		EPIC->CLEAR = (1<<EPIC_GPIO_IRQ_INDEX);
	}
	if (EPIC->RAW_STATUS & (1<< EPIC_TIMER32_0_INDEX)) {
		DHCP_time_handler();
		//xprintf("Start tim32_0 interrupt\r\n"); // Test tim32_0 interrupt
		TIMER32_0->INT_CLEAR = TIMER32_INT_OVERFLOW_M;
		EPIC->CLEAR = (1 << EPIC_TIMER32_0_INDEX);
	}
}

//------------------------------------------------------------------------

void main() {
	// interrupt vector init
		write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
						| PM_CLOCK_APB_P_GPIO_1_M
						| PM_CLOCK_APB_P_GPIO_2_M
						| PM_CLOCK_APB_P_GPIO_IRQ_M;
	PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
						| PM_CLOCK_APB_M_WU_M
						| PM_CLOCK_APB_M_PM_M
						| PM_CLOCK_APB_M_EPIC_M;
	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;


	SPI1_Init();
	__HAL_SPI_ENABLE(&hspi1);

	SystemClock_Config();
	UART_Init(UART_1, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

	// LEDs init
	GPIO_0->DIRECTION_OUT =  1<<(9);
	GPIO_0->DIRECTION_OUT =  1<<(10);

	// extint on user key setup begin
	GPIO_1->DIRECTION_IN =  1<<(15);

	// interrupt generation setup
	GPIO_IRQ->LINE_MUX = 3 << (7*4);	// 7th line <- Mode=3
	GPIO_IRQ->EDGE = 1 << 7;			// EDGE mode
	GPIO_IRQ->LEVEL_CLEAR = 1 << 7;		// falling edge
	GPIO_IRQ->ENABLE_SET = 1 << 7;		// enable 7th line

	// interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_GPIO_IRQ_INDEX;

	initTimerInterrupts();

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);


	W5500_Start();

    xprintf("Start... ");

    while (1) {
		W5500_ServerTCP_Step(TCP_OnReceive);
		W5500_ServerUDP_Step(UDP_OnReceive);
    }
}
//------------------------------------------------------------------------

void SystemClock_Config(void) {
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable 			= PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem 	= PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys 		= PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk 		= PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider 					= 0;
    PCC_OscInit.APBMDivider 				= 0;
    PCC_OscInit.APBPDivider 				= 0;
    PCC_OscInit.HSI32MCalibrationValue 		= 128;
    PCC_OscInit.LSI32KCalibrationValue 		= 128;
    PCC_OscInit.RTCClockSelection 			= PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection 		= PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;

    HAL_PCC_Config(&PCC_OscInit);
}
//------------------------------------------------------------------------

void initTimerInterrupts() {
	disableInterrupts(); // Перед настройкой таймера необходимо выключить прерывания

	PM->CLK_APB_M_SET |= PM_CLOCK_APB_M_TIMER32_0_M;	// Подаем тактирование на модуль TIMER32_0

	//-------- Timer32_0 Interrupt
	// сбрасываем предыдущие надстройки
	TIMER32_0->INT_MASK 	= 0;
	TIMER32_0->INT_CLEAR 	= 0x3FF;
	TIMER32_0->CONTROL 		= 0;

	TIMER32_0->TOP	 		= 32000000u;					// Устанавливаем срабатывание таймера   каждую секунду
	TIMER32_0->INT_MASK 	= TIMER32_INT_OVERFLOW_M;		// Включаем прерывание по событию переполнение

	EPIC->MASK_EDGE_CLEAR 	= 0xFF;
	EPIC->CLEAR 			= 0xFFFF;
	EPIC->MASK_EDGE_SET 	= 1 << EPIC_TIMER32_0_INDEX;

	// Global interrupt enable
	enableInterrupts();

	TIMER32_0->ENABLE = TIMER32_ENABLE_TIM_EN_M;	// Enable Timer32_0
}
//------------------------------------------------------------------------

void enableInterrupts() {
	set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}
//------------------------------------------------------------------------

void disableInterrupts() {
	clear_csr(mie, MIE_MEIE);
}
//------------------------------------------------------------------------

void SPI1_Init(void) {

	hspi1.Instance = SPI_1;
	hspi1.Init.SPI_Mode = HAL_SPI_MODE_MASTER;
	hspi1.Init.CLKPhase = SPI_PHASE_ON;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.ThresholdTX = 4;
	hspi1.Init.BaudRateDiv = SPI_BAUDRATE_DIV256;
	hspi1.Init.Decoder = SPI_DECODER_NONE;
	hspi1.Init.ManualCS = SPI_MANUALCS_ON;
	hspi1.Init.ChipSelect = SPI_CS_0;

	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		xprintf("SPI_Init_Error\n");
	}
}
//------------------------------------------------------------------------

void TCP_OnReceive(uint8_t *pData, uint16_t *pDataSize,
			uint16_t pDataSizeMax) {
		// На запрос "Hi MIK32" получаем ответ "Hi User, how are you"
		if (strstr(pData, "Hi MIK32")) {
		strcpy(pData, "Hi User, how are you");
		*pDataSize = strlen(pData);
		}
}


void UDP_OnReceive(uint8_t *pData, uint16_t *pDataSize, uint16_t pDataSizeMax, uint8_t *addr, uint16_t port) {
	// Если пришла определенная фраза, то это Broadcast запрос Поиска устройства
	if (strstr(pData, "Granit") != NULL) {
		strcpy(pData, "Granit W5500");
		*pDataSize = strlen(pData);
		return;
	}
}
//------------------------------------------------------------------------

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
