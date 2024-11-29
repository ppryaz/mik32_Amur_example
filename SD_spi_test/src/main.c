
/* ************   Пример работы драйвера SD карт через интерфейс SPI      ************************
 * ************	         Используется файловая система MIK32FAT	  	      ************************
 *	Пример демонстирует функционал чтения, записи и удаления файлов.
 *	Также присутствует возможность вывода сервисной информации о подключенной SD карте
 *	Используемый формат фаловой системы FAT32
 *	В проекте использован модуль SPI_1
 *
 *  *******   Подключение:
 *  PORT1_0 - MISO with PullUp  		-> Card - DO
 *  PORT1_1 - MOSI 						-> Card - DI
 *  PORT1_2 - CLK						-> Card - CLK
 *  PORT1_4 - SPI1_SS_Out_1 PullUp		-> Card - CS
 *
 *  Макросы управления функциональностью примера:
 *  SHOW_SERVICE_INFO
 *  WRITE_EXAMPLE
 *  READ_EXAMPLE
 *  DELETE_FILE_EXAMPLE
 *
 * 	MCU type: MIK32 AMUR К1948ВК018
 * 	Board: отладочная плата v0.3, с программатором
 */

#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_spi.h"
#include "sd.h"
#include "mik32fat.h"
#include "uart_lib.h"
#include "xprintf.h"


#define READ_EXAMPLE
#define WRITE_EXAMPLE
#define DELETE_FILE_EXAMPLE
#define SHOW_SERVICE_INFO

#define READ_BUFFER_LENGTH  20

SCR1_TIMER_HandleTypeDef 	hscr1_timer;
SPI_HandleTypeDef 			hspi_sd;
SD_Descriptor_t 			sd;
FAT_Descriptor_t 			fs;

static void Scr1_Timer_Init(void);
void SystemClock_Config(void);
static void UART_1_Init(void);
static void SD_FS_Config();

int main() {
    SystemClock_Config();
    Scr1_Timer_Init();
    UART_1_Init();

    xprintf("*** Start ***\r\n");

    SD_FS_Config();
    FAT_Status_t res;

#ifdef DELETE_FILE_EXAMPLE
	xprintf("\r\nDelete file example\r\n");

	res = MIK32FAT_Delete(&fs, "TESTS/WRITE1.TXT");
	if (res != FAT_OK) {
		xprintf("Error occured when TESTS/WRITE1.TXT try delete, status %u",
				res);
	} else {
		xprintf(
				"The file TESTS/WRITE1.TXT was successfully deleted from the disk, status %u \r\n",
				res);
	}
#endif
#ifdef WRITE_EXAMPLE
    xprintf("\r\nWriting file example\r\n");

    FAT_File_t write_file;

    res = MIK32FAT_FileOpen(&write_file, &fs, "TESTS/WRITE1.TXT", 'W');
    xprintf("TESTS/WRITE1.TXT file open status: %d\r\n", res);

    if (res != FAT_OK) {
        while (1) {
            xprintf("Error occured with file TESTS/WRITE1.TXT, status %u\r\n", res);
            HAL_DelayMs(&hscr1_timer, 5000);
        }
    }
    char str[] = "Writing string to file\r\n";

    xprintf("Wrote bytes: %d  ", MIK32FAT_WriteFile(&write_file, str, strlen(str)));
    xprintf("Close status: %d\r\n", MIK32FAT_FileClose(&write_file));
#endif
#ifdef READ_EXAMPLE
    xprintf("\nReading file example\r\n");
    FAT_File_t read_file;

    res = MIK32FAT_FileOpen(&read_file, &fs, "TESTS/WRITE1.TXT", 'R');
    xprintf("TESTS/WRITE1.TXT file open status: %d\r\n", res);
    if (res != FAT_OK) {
        while (1) {
            xprintf("Error occured with file TESTS/WRITE1.TXT, status %u\r\n", res);
            HAL_DelayMs(&hscr1_timer, 5000);
        }
    }
    static char read_buffer[READ_BUFFER_LENGTH];
    uint8_t i = read_file.len / (READ_BUFFER_LENGTH-1);

    if (read_file.len % (READ_BUFFER_LENGTH-1) != 0) i += 1;
    uint32_t bytes_read;

    xprintf("Text:\r\n");

    while (i > 0) {
        bytes_read = MIK32FAT_ReadFile(&read_file, read_buffer, READ_BUFFER_LENGTH-1);

        if (bytes_read == 0) {
            xprintf("Error occured while file reading, stop.\r\n");
            break;
        } else {
            /* Вставить символ возврата каретки для корректной печати */
            read_buffer[bytes_read] = '\0';
            xprintf("%s", read_buffer);
        }
        i -= 1;
    }
    xprintf("Close status: %d\r\n", MIK32FAT_FileClose(&read_file));
#endif

}
//---------------------------------------------------------------------------

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

static void SD_FS_Config() {
    SD_Status_t res;

    res = SD_Init(&sd, &hspi_sd, SPI_1, SPI_CS_0, &hscr1_timer);
    xprintf("SD card: %s\r\n", res==SD_OK ? "found" : "not found");

    if (res != SD_OK) {
        while (1);	 // If SD not found, go into endless loop
    }

    xprintf("Type: ");
    switch (sd.type) {
        case SDv1: xprintf("SDv1"); break;
        case SDv2: xprintf("SDv2"); break;
        case SDHC: xprintf("SDHC"); break;
        case MMC: xprintf("MMC"); break;
        default: xprintf("Unknown");
    }
    xprintf("\r\n");

    // Инициализация файловой системы
    FAT_Status_t fs_res;
    fs_res = MIK32FAT_Init(&fs, &sd);

    xprintf("FS initialization: %s", fs_res==FAT_OK ? "ok\n" : "failed, ");
    if (fs_res != FAT_OK) {

    	switch (fs_res) {
            case FAT_DiskError: xprintf("disk error"); break;
            case FAT_DiskNForm: xprintf("disk not mount"); break;
            default: xprintf("unknown error"); break;
        }

        xprintf("\r\n");
        while(1);
    }
    xprintf("\r\n");

#ifdef SHOW_SERVICE_INFO
    xprintf("FS startaddr: %u\r\n", fs.fs_begin);
    xprintf("First FAT1 startaddr: %u\r\n", fs.fat1_begin);
    xprintf("First FAT2 startaddr: %u\r\n", fs.fat2_begin);
    xprintf("First data cluster: %u\r\n", fs.data_region_begin);
    xprintf("FAT length: %u\r\n", fs.param.fat_length);
    xprintf("Num of FATs: %u\r\n", fs.param.num_of_fats);
    xprintf("Sectors per cluster: %u\r\n", fs.param.sec_per_clust);
#endif
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
