#include "W5500.h"
#include "mik32_hal_spi.h"
#include "socket.h"
#include "../Internet/DHCP/dhcp.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// Настройки

// Интерфейс подключения
//extern SPI_HandleTypeDef hspi1;
#define W5500_SPI	&hspi1

////////////////////////////////////////////////////////////////////////////////////////////////////

// Экземпляры Буфера данных и Структур сетевых параметров
uint8_t W5500_DATA_BUF[W5500_DATA_BUF_SIZE];
wiz_NetInfo tWIZNETINFO, gWIZNETINFO	= {
		.mac	= {0x00, 0x08, 0xdc, 0x00, 0xab, 0xce},	//NOTICE: Для каждого устройства новый (Начинать с 0x00, 0x08, чтобы знать EEPROM пустой или нет)
		.ip		= {192, 168, 1, 245},					//NOTICE: IP адрес по умолчанию должен быть единый для типа устройства
		.sn		= {255, 255, 0, 0},
		.gw		= {192, 168, 0, 1},
		.dns	= {192, 168, 0, 1},
		.dhcp	= NETINFO_STATIC // NETINFO_DHCP //  NETINFO_STATIC
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static void W5500_Select() {

	__HAL_SPI_ENABLE(&hspi1);
	HAL_SPI_CS_Enable(&hspi1, SPI_CS_0);
}

static void W5500_DeSelect() {

	HAL_SPI_CS_Disable(&hspi1);
}

static uint8_t W5500_SPI_ReadByte() {
	uint8_t dummy = 0x1;
	uint8_t data	= 0;
	HAL_SPI_Exchange(W5500_SPI,  &dummy, &data, 1, W5500_SPI_TIMEOUT);
	return data;
}

static void W5500_SPI_WriteByte(uint8_t data) {
	int8_t dummy = 0;
	HAL_SPI_Exchange(W5500_SPI, &data, &dummy, 1, W5500_SPI_TIMEOUT);
}

static void W5500_Init(void) {
	// Настройка параметров
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

#ifdef W5500_MODE_DEBUG
	wiz_NetInfo netinfo;
	uint8_t tmpstr[6] = {0, 0, 0, 0, 0, 0};

	ctlnetwork(CN_GET_NETINFO, (void*)&netinfo);	// Запрос параметров
	ctlwizchip(CW_GET_ID, (void*)tmpstr);			// Запрос идентификатора чипа

	xprintf("\n=== %s NET CONF : %s ===\r\n", (char*)tmpstr, ((netinfo.dhcp == NETINFO_DHCP) ? "DHCP" : "Static"));
	xprintf("MAC: %d:%d:%d:%d:%d:%d\r\n", netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5]);
	xprintf("SIP: %d.%d.%d.%d\r\n", netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3]);
	xprintf("GAR: %d.%d.%d.%d\r\n", netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3]);
	xprintf("SUB: %d.%d.%d.%d\r\n", netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3]);
	xprintf("DNS: %d.%d.%d.%d\r\n", netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3]);
	xprintf("===========================\r\n");
#endif
}

static void W5500_AssignIP(void) {
	getIPfromDHCP(gWIZNETINFO.ip);
	getGWfromDHCP(gWIZNETINFO.gw);
	getSNfromDHCP(gWIZNETINFO.sn);
	getDNSfromDHCP(gWIZNETINFO.dns);
	gWIZNETINFO.dhcp = NETINFO_DHCP;

	W5500_Init();
#ifdef W5500_MODE_DEBUG
	xprintf("DHCP LEASED TIME : %ld Sec.\r\n", getDHCPLeasetime());
#endif
}

static void W5500_ConflictIP(void) {
#ifdef W5500_MODE_DEBUG
	xprintf("CONFLICT IP from DHCP\n");
#endif
	// Конфликт IP адресов
	while (1) {}	// HALT
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t W5500_Start() {
#ifdef W5500_EEPROM_USE
	// Читаем настройки из EEPROM, если начало записи неверное, то записываем настройки по умолчанию
//	AT24Cxx_Read(W5500_EEPROM_DEVICE_TYPE, W5500_EEPROM_DEVICE_ADDRESS, W5500_EEPROM_MEMORY_ADDRESS, (uint8_t*)(&tWIZNETINFO), sizeof(wiz_NetInfo));
	if (tWIZNETINFO.mac[0] != 0x00 || tWIZNETINFO.mac[1] != 0x08) {
//		AT24Cxx_Write(W5500_EEPROM_DEVICE_TYPE, W5500_EEPROM_DEVICE_ADDRESS, W5500_EEPROM_MEMORY_ADDRESS, (uint8_t*)(&gWIZNETINFO), sizeof(wiz_NetInfo));
	}
	else {
		gWIZNETINFO = tWIZNETINFO;
	}
#endif

	uint8_t memsize[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
	uint8_t retryCountDHCP = 0;
	uint8_t status;

	reg_wizchip_cs_cbfunc(W5500_Select, W5500_DeSelect);
	reg_wizchip_spi_cbfunc(W5500_SPI_ReadByte, W5500_SPI_WriteByte);

	// Инициализация чипа с указанием размеров буфера сокетов
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*)memsize) == -1) {
#ifdef W5500_MODE_DEBUG
		xprintf("WIZCHIP Initialized fail.\n");
#endif
		while (1) {}	// HALT
	}

	// Проверка статуса PHY link
	do {
		if (ctlwizchip(CW_GET_PHYLINK, (void*)&status) == -1) {
#ifdef W5500_MODE_DEBUG
			xprintf("Unknown PHY Link status.\n");
#endif
		}
	}
	while (status == PHY_LINK_OFF);

	// Перед запуском DHCP должен быть установлен MAC
	setSHAR(gWIZNETINFO.mac);

	// Динамический адрес по DHCP
	if (gWIZNETINFO.dhcp == NETINFO_DHCP) {
		DHCP_init(W5500_DHCP_SOCK, W5500_DATA_BUF);
		reg_dhcp_cbfunc(W5500_AssignIP, W5500_AssignIP, W5500_ConflictIP);	// Указываем обработчики событий Получения IP адреса, �?зменения, Конфликта адресов (ip_assign, ip_update, ip_conflict)

		while (1) {
			status = DHCP_run();

			if (status == DHCP_RUNNING)	continue;
			else if (status == DHCP_IP_ASSIGN || status == DHCP_IP_CHANGED) return 1;	// If this block empty, act with default_ip_assign & default_ip_update This example calls W5500_AssignIP in the two case.
			else if (status == DHCP_IP_LEASED) return 2;	// Арендованный IP адрес
			else if (status == DHCP_STOPPED) return 3;		// Прекращена обработка протокола DHCP
			else if (status == DHCP_FAILED) {
				retryCountDHCP++;
				if (retryCountDHCP > W5500_DHCP_RETRY_MAX) {
#ifdef W5500_MODE_DEBUG
					xprintf(">> DHCP %d Failed\r\n", retryCountDHCP);
#endif
					retryCountDHCP = 0;
					DHCP_stop();	// Для запуска придется вызывать DHCP_init()
					W5500_Init();	// Применить Статические настройки IP
					return 4;
				}
			}
		}
	}

	// Статический IP адрес
	else {
		W5500_Init();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static int32_t W5500_TCP_Step(uint8_t sn, uint8_t* buf, uint16_t port, W5500_ServerTCP_OnReceive onReceive) {
	int32_t ret;
	uint16_t size = 0, sentsize = 0;
	uint8_t snSR = getSn_SR(sn);

	// Сокет закрыт
	if (snSR == SOCK_CLOSED) {
		if ((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
	}

	// Сокет открыт в режиме TCP
	else if (snSR == SOCK_INIT) {
		if ((ret = listen(sn)) != SOCK_OK) return ret;
	}

	// Сокет подключен. TCP-Сервер обработал пакет SYN от TCP-Клиента во время SOCK_LISTEN или когда команда подключения к серверу выполнена успешно
	else if (snSR == SOCK_ESTABLISHED) {
		if (getSn_IR(sn) & Sn_IR_CON) setSn_IR(sn, Sn_IR_CON);

		// Здесь не нужно проверять SOCK_BUSY
		if ((size = getSn_RX_RSR(sn)) > 0) {
			if (size > W5500_DATA_BUF_SIZE) size = W5500_DATA_BUF_SIZE;
			ret = recv(sn, buf, size);

			if (ret <= 0) return ret;	// Возможные значения (ret <= 0): SOCK_BUSY, SOCKERR_SOCKSTATUS, SOCKERR_SOCKMODE, SOCKERR_SOCKNUM, SOCKERR_DATALEN

			size = (uint16_t)ret;
			if (onReceive) onReceive(buf, &size, W5500_DATA_BUF_SIZE);

			// Если есть данные для передачи
			if (size > 0) {
				sentsize = 0;
				while (size != sentsize) {
					ret = send(sn, buf + sentsize, size - sentsize);	// Возможные значения (ret <= 0): SOCK_BUSY, SOCKERR_SOCKSTATUS, SOCKERR_TIMEOUT, SOCKERR_SOCKMODE, SOCKERR_SOCKNUM, SOCKERR_DATALEN
					if (ret < 0) {
						close(sn);
						return ret;
					}
					sentsize += ret;	// ret = 0 - это SOCK_BUSY, поэтому можно складывать
				}
			}
		}
	}

	// Сокет получил запрос на отключение (пакет FIN) от клиента. Это полузакрытое состояние, и данные могут быть переданы.
	else if (snSR == SOCK_CLOSE_WAIT) {
		if ((ret = disconnect(sn)) != SOCK_OK) return ret;
	}

	// Возможные необработанные состояния
	//SOCK_LISTEN		// Сокет в режиме TCP-сервера и ожидает запроса на подключение (SYN-пакета). Состояние изменится на SOCK_ESTALBLISHED, когда запрос на подключение будет успешно принят.
	//SOCK_SYNSENT		// Отправлен SYN
	//SOCK_SYNRECV		// Получен SYN
	//SOCK_FIN_WAIT		// Сокет закрывается, ожидает FIN
	//SOCK_CLOSING		// Сокет закрывается
	//SOCK_TIME_WAIT	// Сокет закрывается

	return SOCK_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// Адрес и Порт Подписчика на события
extern uint8_t subscriberAddress[4];
extern uint16_t subscriberPort;

static int32_t W5500_UDP_Step(uint8_t sn, uint8_t* buf, uint16_t port, W5500_ServerUDP_OnReceive onReceive) {
	int32_t ret;
	uint16_t size, sentsize;
	uint8_t destip[4];
	uint16_t destport;
	uint8_t snSR = getSn_SR(sn);

	// Сокет закрыт
	if (snSR == SOCK_CLOSED) {
		if ((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
	}

	// Сокет открыт в режиме UDP
	else if (snSR == SOCK_UDP) {
		// Отправка сообщений Подписчикам
		//UDP_InformSubscribers(buf, &size, W5500_DATA_BUF_SIZE);

		// Если есть данные для передачи
		if (size > 0) {
			sentsize = 0;
			while (sentsize != size) {
				ret = sendto(sn, buf + sentsize, size - sentsize, subscriberAddress, subscriberPort);
				if (ret < 0) return ret;

				sentsize += ret;	// ret = 0 - это SOCK_BUSY, поэтому можно складывать
			}
		}

		// Получены данные
		if ((size = getSn_RX_RSR(sn)) > 0) {
			if (size > W5500_DATA_BUF_SIZE) size = W5500_DATA_BUF_SIZE;
			ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
			if (ret <= 0) return ret;

			size = (uint16_t)ret;
			if (onReceive) onReceive(buf, &size, W5500_DATA_BUF_SIZE, destip, destport);

			// Если есть данные для передачи
			if (size > 0) {
				sentsize = 0;
				while (sentsize != size) {
					ret = sendto(sn, buf + sentsize, size - sentsize, destip, destport);
					if (ret < 0) return ret;

					sentsize += ret;	// ret = 0 - это SOCK_BUSY, поэтому можно складывать
				}
			}
		}
	}

	return SOCK_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t W5500_ServerTCP_Step(W5500_ServerTCP_OnReceive onReceive) {
	return W5500_TCP_Step(W5500_SERVER_TCP_SOCK, W5500_DATA_BUF, W5500_SERVER_TCP_PORT, onReceive);
}

int32_t W5500_ServerUDP_Step(W5500_ServerUDP_OnReceive onReceive) {
	return W5500_UDP_Step(W5500_SERVER_UDP_SOCK, W5500_DATA_BUF, W5500_SERVER_UDP_PORT, onReceive);
}

//-------------------------------------------------------

#include "w5500.h"

#define _W5500_SPI_VDM_OP_          0x00
#define _W5500_SPI_FDM_OP_LEN1_     0x01
#define _W5500_SPI_FDM_OP_LEN2_     0x02
#define _W5500_SPI_FDM_OP_LEN4_     0x03

#if   (_WIZCHIP_ == 5500)
////////////////////////////////////////////////////

uint8_t  WIZCHIP_READ(uint32_t AddrSel)
{
   uint8_t ret;
   uint8_t spi_data[3];

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

   AddrSel |= (_W5500_SPI_READ_ | _W5500_SPI_VDM_OP_);

   if(!WIZCHIP.IF.SPI._read_burst || !WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
	   WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF0000) >> 16);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x0000FF00) >>  8);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x000000FF) >>  0);
   }
   else																// burst operation
   {
		spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
		spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
		spi_data[2] = (AddrSel & 0x000000FF) >> 0;
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
   }
   ret = WIZCHIP.IF.SPI._read_byte();

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
   return ret;
}

void     WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb )
{
   uint8_t spi_data[4];

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

   AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_);

   //if(!WIZCHIP.IF.SPI._read_burst || !WIZCHIP.IF.SPI._write_burst) 	// byte operation
   if(!WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF0000) >> 16);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x0000FF00) >>  8);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x000000FF) >>  0);
		WIZCHIP.IF.SPI._write_byte(wb);
   }
   else									// burst operation
   {
		spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
		spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
		spi_data[2] = (AddrSel & 0x000000FF) >> 0;
		spi_data[3] = wb;
		WIZCHIP.IF.SPI._write_burst(spi_data, 4);
   }

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
}

void     WIZCHIP_READ_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
   uint8_t spi_data[3];
   uint16_t i;

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

   AddrSel |= (_W5500_SPI_READ_ | _W5500_SPI_VDM_OP_);

   if(!WIZCHIP.IF.SPI._read_burst || !WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF0000) >> 16);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x0000FF00) >>  8);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x000000FF) >>  0);
		for(i = 0; i < len; i++)
		   pBuf[i] = WIZCHIP.IF.SPI._read_byte();
   }
   else																// burst operation
   {
		spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
		spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
		spi_data[2] = (AddrSel & 0x000000FF) >> 0;
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
		WIZCHIP.IF.SPI._read_burst(pBuf, len);
   }

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
}

void     WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
   uint8_t spi_data[3];
   uint16_t i;

   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

   AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_);

   if(!WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF0000) >> 16);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x0000FF00) >>  8);
		WIZCHIP.IF.SPI._write_byte((AddrSel & 0x000000FF) >>  0);
		for(i = 0; i < len; i++)
			WIZCHIP.IF.SPI._write_byte(pBuf[i]);
   }
   else									// burst operation
   {
		spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
		spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
		spi_data[2] = (AddrSel & 0x000000FF) >> 0;
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
		WIZCHIP.IF.SPI._write_burst(pBuf, len);
   }

   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
}


uint16_t getSn_TX_FSR(uint8_t sn)
{
   uint16_t val=0,val1=0;

   do
   {
      val1 = WIZCHIP_READ(Sn_TX_FSR(sn));
      val1 = (val1 << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),1));
      if (val1 != 0)
      {
        val = WIZCHIP_READ(Sn_TX_FSR(sn));
        val = (val << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn),1));
      }
   }while (val != val1);
   return val;
}


uint16_t getSn_RX_RSR(uint8_t sn)
{
   uint16_t val=0,val1=0;

   do
   {
      val1 = WIZCHIP_READ(Sn_RX_RSR(sn));
      val1 = (val1 << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),1));
      if (val1 != 0)
      {
        val = WIZCHIP_READ(Sn_RX_RSR(sn));
        val = (val << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn),1));
      }
   }while (val != val1);
   return val;
}

void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
   uint16_t ptr = 0;
   uint32_t addrsel = 0;

   if(len == 0)  return;
   ptr = getSn_TX_WR(sn);
   //M20140501 : implict type casting -> explict type casting
   //addrsel = (ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
   addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
   //
   WIZCHIP_WRITE_BUF(addrsel,wizdata, len);

   ptr += len;
   setSn_TX_WR(sn,ptr);
}

void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
   uint16_t ptr = 0;
   uint32_t addrsel = 0;

   if(len == 0) return;
   ptr = getSn_RX_RD(sn);
   //M20140501 : implict type casting -> explict type casting
   //addrsel = ((ptr << 8) + (WIZCHIP_RXBUF_BLOCK(sn) << 3);
   addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_RXBUF_BLOCK(sn) << 3);
   //
   WIZCHIP_READ_BUF(addrsel, wizdata, len);
   ptr += len;

   setSn_RX_RD(sn,ptr);
}


void wiz_recv_ignore(uint8_t sn, uint16_t len)
{
   uint16_t ptr = 0;

   ptr = getSn_RX_RD(sn);
   ptr += len;
   setSn_RX_RD(sn,ptr);
}
#endif
