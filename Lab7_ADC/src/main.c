#include <gpio.h>
#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <csr.h>
#include "uart.h"
#include "analog_reg.h"
#include "mik32_hal_pcc.h"
#include "stdlib.h"
#include "stdio.h"


static void SystemClock_Config(unsigned long oscillator);

extern unsigned long __TEXT_START__;


void main() {
	// interrupt vector init
	write_csr(mtvec, &__TEXT_START__);

	SystemClock_Config(PCC_OSCILLATORTYPE_OSC32K | PCC_OSCILLATORTYPE_OSC32M);

	PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
						| PM_CLOCK_APB_P_GPIO_1_M
						| PM_CLOCK_APB_P_GPIO_2_M
						| PM_CLOCK_APB_P_UART_1_M
						| PM_CLOCK_APB_P_ANALOG_REGS_M
						;
	PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
						;
	PM->CLK_AHB_SET	  =   PM_CLOCK_AHB_SPIFI_M
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
    UART_1->DIVIDER = 32000000/9600; // 3333 for 9600
    UART_1->CONTROL1 = UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_UE_M;
    UART_1->FLAGS = 0xFFFFFFFF;

	//ADC setup
	// 0.2 make as input
	GPIO_0->DIRECTION_IN =  1<<(2);
	// 0.2 to PAD_CONTROL = 3
	PAD_CONFIG->PORT_0_CFG = (PAD_CONFIG->PORT_0_CFG & (~(0b11<<(2*2)))) | (0b11<<(2*2));
	ANALOG_REG->ADC_CONFIG =
			(0b111111 << ADC_CONFIG_SAH_TIME_S) 	// max SAH time
		|	(2<<ADC_CONFIG_SEL_S)					// channel 2
		|	(1<<ADC_CONFIG_RESETN_S)				// Reset OFF
		|	(1<<ADC_CONFIG_EN_S)					// Enabled
		;

	// 0.4 make as output
	GPIO_0->DIRECTION_OUT =  1<<(4);
	GPIO_0->SET =  1<<(4);

	printf("\r\nStart\r\n");
    for (;;)
    {
    	ANALOG_REG->ADC_SINGLE = 1;
        while (!(ANALOG_REG->ADC_VALID))
            ;
    	printf("ADC_2 = %d\r\n", ANALOG_REG->ADC_VALUE);
    	GPIO_0->OUTPUT ^=  1<<(4);
    	for (volatile int i = 0; i < 1000000; i++)
    			;
    }
}



static void SystemClock_Config(unsigned long oscillator)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = oscillator;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_OSC32K;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}


size_t _write(int file, const void *ptr, size_t len)
{
	for (int i = 0; i<len; i++)
	{
		while ((UART_1->FLAGS & UART_FLAGS_TXE_M) == 0);
		UART_1->TXDATA = ((unsigned char *)ptr)[i];
	}
	return len;
}
