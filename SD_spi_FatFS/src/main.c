
/* ************   Пример работы драйвера SD карт через интерфейс SPI      ************************
 * ************	         Используется файловая система FatFs	  	      ************************
 *	Пример демонстирует функционал чтения, записи и удаления файлов.
 *	Также присутствует возможность вывода сервисной информации о подключенной SD карте
 *	Используемый формат фаловой системы FAT32
 *	Encoding UTF8
 *	В проекте использован модуль SPI_1
 *
 *  *******   Подключение:
 *  PORT1_0 - MISO with PullUp  		-> Card - DO
 *  PORT1_1 - MOSI 						-> Card - DI
 *  PORT1_2 - CLK						-> Card - CLK
 *  PORT1_4 - SPI1_SS_Out_1 PullUp		-> Card - CS
 *
 *  Определите следующие макросы в соответствии с именами существующих тестовых файлов на SD карте для чтения
 *  Если файлов не существует, они будут созданы под вашими названиями при включении WRITE_EXAMPLE
 *  Если файлы отсутствуют при включении DELETE_FILE_EXAMPLE, вы получите сообщение об ошибке
 *  FILE_1_EXAMPLE	"log.txt"
 *  FILE_2_EXAMPLE	"WRITE1.txt"
 *
 *  Макросы управления функциональностью примера:
 *  SHOW_SERVICE_INFO					информация о файловой системе
 *  WRITE_EXAMPLE						запись в файлы
 *  READ_EXAMPLE						чтение из файла
 *  DELETE_FILE_EXAMPLE
 *  DISPLAY_DIR
 *
 *  ----------     !!!Attention!!!		------------------------------------
 *  Для вывода русского текста в терминале следует установить кодировку UTF8
 *  Соответственно текст в файле тоже должен быть в UTF8
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3, с программатором
 */

#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_spi.h"
#include "sd.h"
#include "uart_lib.h"
#include "xprintf.h"

#include "fatfs/ff.h"
#include "fatfs/diskio.h"

#define FILE_1_EXAMPLE 	"Table.txt"
#define FILE_2_EXAMPLE	"Test1.txt"

#define READ_EXAMPLE
//#define WRITE_EXAMPLE				// !!! Write from end of files WRITE1.txt and log.txt
//#define DELETE_FILE_EXAMPLE		// !!! Delete  files WRITE1.txt and log.txt from SD card
#define SHOW_SERVICE_INFO
#define DISPLAY_DIR
#define DBG							// if your need debug

#define READ_BUFFER_LENGTH  20

SCR1_TIMER_HandleTypeDef 	hscr1_timer;
SPI_HandleTypeDef 			hspi_sd;
SD_Descriptor_t 			sd;
FATFS 						fs;

static void Scr1_Timer_Init(void);
void SystemClock_Config(void);
static void UART_1_Init(void);

static void displayDirectory(char* path);
static void displayServiceInfo();
static void fileWorking();

//--------- FatFs------------------------------
SD_Status_t SD_init();

//---------------------------------------------

int main() {
    SystemClock_Config();
    Scr1_Timer_Init();
    UART_1_Init();

    FRESULT res;

    SD_init();
    xprintf("Ready!\r\n");

    // Mount the default drive
    res = f_mount(&fs, "", 0);
    if(res != FR_OK) {
        xprintf("f_mount() failed, res = %d\r\n", res);
        return -1;
    }

    xprintf("f_mount() done!\r\n");

#ifdef DISPLAY_DIR
    displayDirectory("/"); // Put your path here
#endif

#ifdef SHOW_SERVICE_INFO
    displayServiceInfo();
#endif

    xprintf("*** Start ***\r\n");
    fileWorking();
}
//---------------------------------------------------------------------------

static void displayDirectory(char* path) {
	DIR dir;
	UINT ByteWritten;
	char string[128];
	int res;

	res = f_opendir(&dir, path);
#ifdef DBG
	if (res != FR_OK) xprintf("res = %d f_opendir\r\n", res);
#endif
	xprintf("\r\n//---   ---  Display Directory  ---   ---	//");
	xprintf("\r\n");

	if (res == FR_OK) {
		while (1) {
			FILINFO fno;
			res = f_readdir(&dir, &fno);
			if ((res != FR_OK) || (fno.fname[0] == 0))
				break;
			strcpy(string, fno.fname);
			res = f_stat(string, &fno);
			xprintf("name = %s, ", string);
			strcpy(string, path);
			xprintf("path = %s, ", string);
			xprintf("size: %u bytes\r\n", (uint32_t)fno.fsize);
		}
		xprintf("\r\n//---   --- End Display Directory ---   ---//\r\n");
	}
}
//--------------------------------------------------------------------------

static void displayServiceInfo() {
	xprintf("\r\n// ---  --- System ServiceInfo ---  --- //\r\n");
	xprintf("\r\n");

    xprintf("Volume base sector: %u\r\n", fs.volbase);
    xprintf("FAT base sector: %u\r\n", fs.fatbase);
    xprintf("Cluster size: %u\r\n", fs.csize);
    xprintf("Last data cluster: %u\r\n", fs.last_clst);
    xprintf("Root directory base sector: %u\r\n", fs.dirbase);
    xprintf("Num of FATs: %u\r\n", fs.fsize);
    xprintf("Sectors per cluster: %u\r\n", fs.free_clst);

    xprintf("\r\n// ---  --- End System ServiceInfo ---  ---// \r\n");
    xprintf("\r\n");
}
//--------------------------------------------------------------------------

static void fileWorking() {
	// Delete, create, read and write file example here
	int res;

#ifdef DELETE_FILE_EXAMPLE
    res = f_unlink(FILE_1_EXAMPLE);
    if (res != FR_OK) xprintf("Deleting a missing file failed, res = %d", res);
#endif

#ifdef WRITE_EXAMPLE
    xprintf("Writing to %s...\r\n", FILE_1_EXAMPLE);

    char writeBuff[128];
    strncpy(writeBuff, "Test write to SD card\r\n",  sizeof(writeBuff));

    FIL logFile;
    res = f_open(&logFile, FILE_1_EXAMPLE, FA_OPEN_APPEND | FA_WRITE);
    if(res != FR_OK) {
        xprintf("f_open() failed, res = %d\r\n", res);
        return;
    }
    xprintf("f_open %s...\r\n", FILE_1_EXAMPLE);

    unsigned int bytesToWrite = strlen(writeBuff);
    unsigned int bytesWritten;
    res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);
    if(res != FR_OK) {
        xprintf("f_write() failed, res = %d\r\n", res);
        //return;
    }

    res = f_close(&logFile);

    if(res != FR_OK) {
            xprintf("f_close() failed, res = %d\r\n", res);
            return;
    }
#endif

#ifdef READ_EXAMPLE
	xprintf("Reading file %s\r\n", FILE_1_EXAMPLE);
	FIL msgFile;
	res = f_open(&msgFile, FILE_1_EXAMPLE, FA_READ);
	if (res != FR_OK) {
		xprintf("f_open() failed, res = %d\r\n", res);
		//return;
	}

	char readBuff[512];
	unsigned int bytesRead;
	xprintf("\r\n``** Reading start.... `\r\n");

	while (!f_eof(&msgFile)) {
		res = f_read(&msgFile, readBuff, sizeof(readBuff) - 1, &bytesRead);
		if (res != FR_OK) {
			xprintf("f_read() failed, res = %d\r\n", res);
			return;
		}
		readBuff[bytesRead] = '\0';
		xprintf("%s", readBuff);
	}
	xprintf("\r\n```\r\n");
        res = f_close(&msgFile);
        if(res != FR_OK) {
            xprintf("f_close() failed, res = %d\r\n", res);
        }
#endif


#ifdef DELETE_FILE_EXAMPLE
	xprintf("\r\nDelete file example\r\n");

	res = f_unlink(FILE_2_EXAMPLE);
	if (res != FR_OK) {
		xprintf("Error occured when %s try delete, status %u", FILE_2_EXAMPLE,
				res);
	} else {
		xprintf(
				"The file %s was successfully deleted from the disk, status %u \r\n", FILE_2_EXAMPLE,
				res);
	}
#endif
#ifdef WRITE_EXAMPLE
    xprintf("\r\nWriting file example\r\n");

    FIL write_file;

   res = f_open(&write_file, FILE_2_EXAMPLE, FA_OPEN_APPEND | FA_WRITE);
    xprintf("%s file open status: %d\r\n", FILE_2_EXAMPLE, res);

    if (res != FR_OK) {
        while (1) {
            xprintf("Error occured with file %s, status %u\r\n", FILE_2_EXAMPLE, res);
            HAL_DelayMs(&hscr1_timer, 5000);
        }
    }
    char str[] = "Writing string to file\r\n";

   f_write(&write_file, str, strlen(str), &bytesWritten);
    xprintf("Wrote bytes: %d, Close status: %d\r\n",bytesWritten, f_close(&write_file));
#endif
#ifdef READ_EXAMPLE
    xprintf("\nReading file example\r\n");
    FIL read_file;

    res = f_open(&read_file, FILE_2_EXAMPLE, FA_READ);
    xprintf("%s file open status: %d\r\n", FILE_2_EXAMPLE, res);
    if (res != FR_OK) {
        while (1) {
            xprintf("Error occured with file %s, status %u\r\n", FILE_2_EXAMPLE, res);
            HAL_DelayMs(&hscr1_timer, 5000);
        }
    }

    xprintf("\r\n`*************``\r\n");

    while (!f_eof(&read_file)) {
		res = f_read(&read_file, readBuff, sizeof(readBuff) - 1, &bytesRead);
		if (res != FR_OK) {
			xprintf("f_read() failed, res = %d\r\n", res);
		}

		readBuff[bytesRead] = '\0';
		xprintf("%s", readBuff);
	}

    xprintf("\r\n```\r\n");
    xprintf("Close status: %d\r\n", f_close(&read_file));
#endif

}
//--------------------------------------------------------------------------
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
//--------------------------------------------------------------------------

SD_Status_t SD_init() {			//For FatFs
	SD_Status_t res;

	res = SD_Init(&sd, &hspi_sd, SPI_1, SPI_CS_0, &hscr1_timer);
	xprintf("SD card: %s\r\n", res==SD_OK ? "found" : "not found");

	return res; 	// 0 == status OK
}
//--------------------------------------------------------------------------------------------------

static void UART_1_Init(void) {

	PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_0_M
			| PM_CLOCK_APB_P_GPIO_1_M
			| PM_CLOCK_APB_P_GPIO_2_M
			| PM_CLOCK_APB_P_UART_1_M ;

	// UART_1 (VCP) setup
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (8 * 2))))
			| (0b01 << (8 * 2));					// 1.8 to PAD_CONTROL = 1

	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11 << (9 * 2))))
			| (0b01 << (9 * 2));					// 1.9 to PAD_CONTROL = 1

	GPIO_1->DIRECTION_IN = 1 << (8);				// 1.8 make as RX input
	GPIO_1->DIRECTION_OUT = 1 << (9);				// 1.9 make as TX output

	UART_1->DIVIDER = 32000000 / 9600; 				// Baudrate 9600 setup here
    UART_1->CONTROL1 =  UART_CONTROL1_TE_M
    		| UART_CONTROL1_RE_M
			| UART_CONTROL1_UE_M
    		| UART_CONTROL1_TCIE_M
    		| UART_CONTROL1_RXNEIE_M;

    UART_1->FLAGS = 0xFFFFFFFF;
}

//--------------------------------------------------------------------------------------------------

static void Scr1_Timer_Init(void) {
    hscr1_timer.Instance = SCR1_TIMER;

    hscr1_timer.ClockSource = SCR1_TIMER_CLKSRC_INTERNAL; /* Источник тактирования */
    hscr1_timer.Divider = 0;                              /* Делитель частоты 10-битное число */

    HAL_SCR1_Timer_Init(&hscr1_timer);
}
//--------------------------------------------------------------------------------------------------

void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
