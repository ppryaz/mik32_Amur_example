#ifndef __MAIN_H
#define __MAIN_H

#include "mik32_hal_spi.h"

SPI_HandleTypeDef hspi1;

void SystemClock_Config();
void SPI1_Init();
void TcpOnConnect(const unsigned char connectionId);
void TCP_OnReceive(uint8_t* pData, uint16_t* pDataSize, uint16_t pDataSizeMax);
void UDP_OnReceive(uint8_t* pData, uint16_t* pDataSize, uint16_t pDataSizeMax, uint8_t* addr, uint16_t port);

void initTimerInterrupts();		// Инициализация и настройка TIMER32_0
void enableInterrupts();		// Глобальное включение прерываний
void disableInterrupts();		// Глобальное выключение прерываний

#endif // __MAIN_H
