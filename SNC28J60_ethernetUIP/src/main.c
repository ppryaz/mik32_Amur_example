/*********   SNC28J60 Ethernet test uIP ****************
*
*	 В проекте реализуются следующие возможности:
*	 - Соединение MIK32 Amur с компьютером через Ethernetс помощью сетевого модуля SNC28J60 TCP/IP(Ethernet)
*	 - Ответ на запрос утилиты ping
*	 - Реализация стека протоколов TCP/IP на базе библиотеки uIP
*	 - Передача данных от компьютера к микроконтроллеру
*	 - Передача данных от микроконтроллера к компьютеру
*
*	 Прошивка выводит в терминал текущую ревизию модуля.
*
*	 Настройки внешней программы тестирования
*	 	Пример: программа Putty
*	 	Выбрать: ConnectionType->Other->Raw
*	 	При назначении статического адреса:
*	 	ip: 192.168.1.245
*	 	tcp port: 3000
*	 	Открыть соединение...
*	 	Отправить тестовую строку - "Hi MIK32"
*	 	ответ: Hello my freind!!
*
*	 Дополнительно, по прерыванию от кнопки на плате (PORT1_15)  включается зеленый тестовый светодиод
*
*	 Для подключения модуля к MIK32 Amur используется SPI_1
*	 Питание модуля SNC28J60  желательно осуществлять от отдельного источника питания (т.к. потребление до 185 мА, от микроконтроллера может не хватить)
*
*	PORT1_0 - MISO with PullUp  		-> SNC28J60 MISO
*	PORT1_1 - MOSI 						-> SNC28J60 MOSI
*	PORT1_2 - CLK						-> SNC28J60 SCK
*	PORT1_4 - SPI1_SS_Out_1 PullUp		-> SNC28J60 SS
*
*	Настройка библиотеки uIP в файлах uip-config.h, uipopt.h
*	uipopt.h - Выбор типа адреса : статический или назначаемый:  UIP_FIXEDADDR 0/1
*	Назначение статического MAC адреса, ip, port : в файле main.h
*	Обработка входящих TCP соединений происходит в функции server_appcall в файле main.c
*
**	  	MCU type: MIK32 AMUR К1948ВК018
*	 	Board: отладочная плата v0.3
*/

#include "riscv_csr_encoding.h"
#include "mik32_hal_scr1_timer.h"
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
#include "mik32_hal_spi.h"

#include "main.h"
#include "../uIP/uip.h"
#include "../uIP/uip_arp.h"
#include "enc28j60.h"


SPI_HandleTypeDef hspi1;
SCR1_TIMER_HandleTypeDef 	hscr1_timer;
uint8_t arp_timer_set;
#define TCP_MAX_CONNECTIONS 2
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

static void Scr1_Timer_Init(void);

//-----------------------------------

extern unsigned long __TEXT_START__;

//-----------------------------------------------------------------------

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1<<EPIC_GPIO_IRQ_INDEX)) 	{
		GPIO_0->OUTPUT ^= (0b1)<<(9);
		GPIO_IRQ->CLEAR = (1 << 7);
		EPIC->CLEAR = (1<<EPIC_GPIO_IRQ_INDEX);
	}
	if (EPIC->RAW_STATUS & (1<< EPIC_TIMER32_0_INDEX)) {
		arp_timer_set = 1;
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
	Scr1_Timer_Init();
	arp_timer_set = 0;

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

    xprintf("\r\nStart enc28j60 + uIP... ");
    enc28j60_init();
    xprintf("\r\nRevision: %d", enc28j60getrev());	// Read revision enc28j60 module

    //---------------------------
    uip_init(); 									// uIP initialization
    uip_arp_init();

    struct uip_eth_addr mac = MAC_ADDRESS;
    uip_setethaddr(mac);

    // Another way to set network setting here

    uip_ipaddr_t ipaddr;
    uip_ipaddr(&ipaddr, 192, 168, 1, 245);
    uip_sethostaddr(&ipaddr);

    uip_ipaddr (&ipaddr, 192,168, 1,11); 	// Filling development board gateway addresses
    uip_setdraddr (&ipaddr);  				// Set up development board gateway IP address (in fact, your router IP address)

    uip_ipaddr (&ipaddr, 255,255,255,0); 	// Fill development board web mask
    uip_setnetmask (&ipaddr); 				// Fill development board web mask


    uip_listen(HTONS(LISTENING_PORT));		// Будем слушать порт 3000

    while (1) {
    	server_loop();
    }
}
//------------------------------------------------------------------------

void server_appcall(void) {

	  if(uip_connected()) {	  }
	  if(uip_closed() ||
	     uip_aborted() ||
	     uip_timedout()) {
	    //closed();
	  }

	  if(uip_acked()) {
	    //acked();
	  }

	  if(uip_newdata()) {
	    //newdata();
	  }

	  if(uip_rexmit() || uip_newdata()||uip_acked()||uip_connected()||uip_poll()) {
		  if(strstr((unsigned char*)uip_appdata, "Hi MIK32"))  uip_send("Hello my friend!", 16);
	  }
}
//------------------------------------------------------------------------

void server_loop(void) {

	uip_len = enc28j60_packet_receive((uint8_t*) uip_buf, UIP_BUFSIZE);

		if (uip_len > 0) {
			if (BUF->type == htons(UIP_ETHTYPE_IP)) {

				uip_arp_ipin();
				uip_input();
				if (uip_len > 0) {
					uip_arp_out();

					enc28j60_packet_send((uint8_t*) uip_buf, uip_len);
				}
			} else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {

				uip_arp_arpin();
				if (uip_len > 0) {
					enc28j60_packet_send((uint8_t*) uip_buf, uip_len);
				}
			}
		}
		if (arp_timer_set) {			// ARP timer, 10 sec
			uip_arp_timer();
			arp_timer_set = 0;
		}

		for(uint16_t i = 0; i < UIP_CONNS; ++i){
			uip_periodic(i);			// Периодическая обработка буфера
		}
}
//-------------------------------------------------------------------------------------

void uip_log(char *msg) {
	xprintf(msg);
}
//-------------------------------------------------------------------------------------
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

	TIMER32_0->TOP	 		= 320000000u;					// Устанавливаем срабатывание таймера   каждую секунду
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

void SPI1_Init() {

	hspi1.Instance 			= SPI_1;
	hspi1.Init.SPI_Mode 	= HAL_SPI_MODE_MASTER;
	hspi1.Init.CLKPhase 	= SPI_PHASE_OFF;
	hspi1.Init.CLKPolarity 	= SPI_POLARITY_LOW;
	hspi1.Init.ThresholdTX 	= 4;
	hspi1.Init.BaudRateDiv 	= SPI_BAUDRATE_DIV4;
	hspi1.Init.Decoder 		= SPI_DECODER_NONE;
	hspi1.Init.ManualCS 	= SPI_MANUALCS_ON;
	hspi1.Init.ChipSelect 	= SPI_CS_0;

	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		xprintf("SPI_Init_Error\n");
	}
}
//------------------------------------------------------------------------

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
//------------------------------------------------------------------------

static void Scr1_Timer_Init(void) {
    hscr1_timer.Instance = SCR1_TIMER;

    hscr1_timer.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL; /* Источник тактирования */
    hscr1_timer.Divider = 0;                              /* Делитель частоты 10-битное число */

    HAL_SCR1_Timer_Init(&hscr1_timer);
}
//--------------------------------------------------------------------------------------------------
