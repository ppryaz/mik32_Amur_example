#include <gpio.h>
#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include <csr.h>
#include "mik32_hal_dac.h"
#include "uart.h"
#include "stdlib.h"
#include "stdio.h"


extern unsigned long __TEXT_START__;


DAC_HandleTypeDef hdac1;

void SystemClock_Config(void);
static void DAC_Init(void);

void main() {
	// interrupt vector init
	write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
						| PM_CLOCK_APB_P_GPIO_1_M
						| PM_CLOCK_APB_P_GPIO_2_M
						| PM_CLOCK_APB_P_UART_1_M
						;
	PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
						| PM_CLOCK_APB_M_WU_M
						| PM_CLOCK_APB_M_PM_M
						| PM_CLOCK_APB_M_EPIC_M
						;
	PM->CLK_AHB_SET |= 	  PM_CLOCK_AHB_SPIFI_M
			;

    // LEDs P0.9 init as Output
	GPIO_0->DIRECTION_OUT =  1<<(9);
    // LEDs P0.10 init as Output
	GPIO_0->DIRECTION_OUT =  1<<(10);

	// UART_1 (VCP) setup
	// 1.8 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11<<(8*2)))) | (0b01<<(8*2));
	// 1.9 to PAD_CONTROL = 1
	PAD_CONFIG->PORT_1_CFG = (PAD_CONFIG->PORT_1_CFG & (~(0b11<<(9*2)))) | (0b01<<(9*2));
	// 1.8 make as input
	GPIO_1->DIRECTION_IN =  1<<(8);
	// 1.9 make as output
	GPIO_1->DIRECTION_OUT =  1<<(9);
    UART_1->DIVIDER = 32000000/9600; // Baudrate setup here
    UART_1->CONTROL1 = UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_UE_M;
    UART_1->FLAGS = 0xFFFFFFFF;

    float f_var = 5.7;
    printf("Hello float = %f\r\n", f_var);

//   for (;;)
 //  {
//		while ((UART_1->FLAGS & UART_FLAGS_TXE_M) == 0);
//   	UART_1->TXDATA = 'X';
//   }
	//for (float f_var = 0.1; 1; f_var *= 1.01) {
	//	printf("Hello float = %f\r\n", f_var);
	//}
// Test  DAC here....

    SystemClock_Config();
    DAC_Init();
    f_var = 8.6;
    printf("After init DAC = %f\r\n", f_var);

    HAL_DAC_SetValue(&hdac1,  0x0FFF); // Max value - input 1.2 V
    HAL_DAC_SetValue(&hdac1,  4095);
    HAL_DAC_SetValue(&hdac1,  3276);
    HAL_DAC_SetValue(&hdac1,  2457);
    HAL_DAC_SetValue(&hdac1,  1638);
    HAL_DAC_SetValue(&hdac1,  819);
    HAL_DAC_SetValue(&hdac1,  0x00); // Min value - 0 V

    printf("DAC SetValue  = %d\r\n", 0x0FFF);
 /*   while (1)
    {
    	printf("Cycle while start here \r\n");
        for (uint16_t DAC_Value = 0; DAC_Value <= 0x0FFF; DAC_Value += 819)
        {
            HAL_DAC_SetValue(&hdac1, DAC_Value);

            HAL_DelayMs(50);

        }
    }
*/
}


size_t _write(int file, const void *ptr, size_t len) {
	for (int i = 0; i < len; i++) {
		while ((UART_1->FLAGS & UART_FLAGS_TXE_M) == 0)
			;
		UART_1->TXDATA = ((unsigned char*) ptr)[i];
	}
	return len;
}

void SystemClock_Config(void) {
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

static void DAC_Init(void) {
    hdac1.Instance = ANALOG_REG;

    hdac1.Instance_dac = HAL_DAC1;
    hdac1.Init.DIV = 0;
    hdac1.Init.EXTRef = DAC_EXTREF_OFF;    /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac1.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac1);
}

