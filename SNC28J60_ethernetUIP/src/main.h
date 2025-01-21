#ifndef __MAIN_H
#define __MAIN_H

#include "mik32_hal_spi.h"
#include "../uIP/uipopt.h"
#include "../uIP/psock.h"
#include "enc28j60.h"

extern SPI_HandleTypeDef hspi1;

void server_loop(void);
void server_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL server_appcall
#endif /* UIP_APPCALL */

typedef struct server_state {
  struct psock p;
  char inputbuffer[10];
  char name[40];
} uip_tcp_appstate_t;


#define STATE_WAITING 0
#define STATE_OUTPUT  1

#define LISTENING_PORT 3000
#define MAC_ADDRESS {0x00, 0x08, 0xdc, 0x00, 0xab, 0xce}

// Static ip address
#define UIP_IPADDR0 192
#define UIP_IPADDR1 168
#define UIP_IPADDR2 1
#define UIP_IPADDR3 245
//
#define UIP_DRIPADDR0   192
#define UIP_DRIPADDR1   168
#define UIP_DRIPADDR2   0
#define UIP_DRIPADDR3   1

// Mask
#define UIP_NETMASK0 255
#define UIP_NETMASK1 255
#define UIP_NETMASK2 255
#define UIP_NETMASK3 0

void SystemClock_Config();
void SPI1_Init();
void TcpOnConnect(const unsigned char connectionId);

//static void handle_connection(struct httpd_state *s);

void initTimerInterrupts();		// Инициализация и настройка TIMER32_0
void enableInterrupts();		// Глобальное включение прерываний
void disableInterrupts();		// Глобальное выключение прерываний

#endif // __MAIN_H
