#include "riscv_csr_encoding.h"
#include "scr1_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <power_manager.h>
#include "pad_config.h"
#include <gpio_irq.h>
#include <epic.h>
#include <csr.h>
#include <gpio.h>

#include "mik32_hal_rtc.h"

#include "uart_lib.h"
#include "xprintf.h"

#define GPIO_X ((GPIO_TypeDef*)GPIO_0_BASE_ADDRESS )

extern unsigned long __TEXT_START__;

extern int OLED_1in3_test(void);

RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef LastTime = { 0 };
RTC_TimeTypeDef CurrentTime = { 0 };
RTC_DateTypeDef CurrentDate = { 0 };

void SystemClock_Config(void);
static void RTC_Init(void);

void trap_handler() {
	if ( EPIC->RAW_STATUS & (1 << EPIC_GPIO_IRQ_INDEX)) {
		GPIO_0->OUTPUT ^= (0b1) << (9);
		GPIO_IRQ->CLEAR = (1 << 7);
		EPIC->CLEAR = (1 << EPIC_GPIO_IRQ_INDEX);
	}
}

void main() {
	// interrupt vector init
	write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_0_M | PM_CLOCK_APB_P_GPIO_1_M
			| PM_CLOCK_APB_P_GPIO_2_M | PM_CLOCK_APB_P_GPIO_IRQ_M;
	PM->CLK_APB_M_SET = PM_CLOCK_APB_M_PAD_CONFIG_M | PM_CLOCK_APB_M_WU_M
			| PM_CLOCK_APB_M_PM_M | PM_CLOCK_APB_M_EPIC_M;
	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;

	UART_Init(UART_1, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

	// LEDs init
	GPIO_0->DIRECTION_OUT = 1 << (9);
	GPIO_0->DIRECTION_OUT = 1 << (10);

	// OLED test run
	//OLED_1in3_test();

	// extint on user key setup begin
	GPIO_1->DIRECTION_IN = 1 << (15);

	// interrupt generation setup
	GPIO_IRQ->LINE_MUX = 3 << (7 * 4);	// 7th line <- Mode=3
	GPIO_IRQ->EDGE = 1 << 7;			// EDGE mode
	GPIO_IRQ->LEVEL_CLEAR = 1 << 7;		// falling edge
	GPIO_IRQ->ENABLE_SET = 1 << 7;		// enable 7th line

	// interrupt reception setup
	EPIC->MASK_LEVEL_SET = 1 << EPIC_GPIO_IRQ_INDEX;

	// global interrupt enable
	set_csr(mstatus, MSTATUS_MIE);
	set_csr(mie, MIE_MEIE);

	xprintf("Test RTC\r\n");
	RTC_Init();

	LastTime = HAL_RTC_GetTime(&hrtc);

	while (1) {
		CurrentTime = HAL_RTC_GetTime(&hrtc);

		if (CurrentTime.Seconds != LastTime.Seconds) {
			LastTime = CurrentTime;
			CurrentDate = HAL_RTC_GetDate(&hrtc);
			xprintf("\n%d century ", CurrentDate.Century);
			xprintf("%d.%d.%d ", CurrentDate.Day, CurrentDate.Month,
					CurrentDate.Year);

			CurrentTime = HAL_RTC_GetTime(&hrtc);
			switch (CurrentTime.Dow) {
			case 1:
				xprintf("Monday\n");
				break;
			case 2:
				xprintf("TUESDAY\n");
				break;
			case 3:
				xprintf("WEDNESDAY\n");
				break;
			case 4:
				xprintf("THURSDAY\n");
				break;
			case 5:
				xprintf("Friday\n");
				break;
			case 6:
				xprintf("SATURDAY\n");
				break;
			case 7:
				xprintf("SUNDAY\n");
				break;
			}
			xprintf("\r%d:%d:%d.%d\n\r", CurrentTime.Hours, CurrentTime.Minutes,
					CurrentTime.Seconds, hrtc.Instance->TOS);
		}

		if (HAL_RTC_GetAlrmFlag(&hrtc)) {

			xprintf("\nAlarm!\n");

			HAL_RTC_AlarmDisable(&hrtc);
			HAL_RTC_ClearAlrmFlag(&hrtc);
		}

		GPIO_0->OUTPUT ^= (0b1) << (10);
		for (volatile int i = 0; i < 300000; i++)
			;
	}
}

static void RTC_Init(void)
{
    __HAL_PCC_RTC_CLK_ENABLE();

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    hrtc.Instance = RTC;

    /* Выключение RTC для записи даты и времени */
    HAL_RTC_Disable(&hrtc);

    /* Установка даты и времени RTC */
    sTime.Dow = RTC_WEEKDAY_FRIDAY;
    sTime.Hours = 11;
    sTime.Minutes = 48;
    sTime.Seconds = 0;
    HAL_RTC_SetTime(&hrtc, &sTime);

    sDate.Century = 21;
    sDate.Day = 25;
    sDate.Month = RTC_MONTH_OCTOBER;
    sDate.Year = 24;
    HAL_RTC_SetDate(&hrtc, &sDate);

    /* Включение будильника. Настройка даты и времени будильника */
    sAlarm.AlarmTime.Dow = RTC_WEEKDAY_TUESDAY;
    sAlarm.AlarmTime.Hours = 20;
    sAlarm.AlarmTime.Minutes = 30;
    sAlarm.AlarmTime.Seconds = 5;

    sAlarm.AlarmDate.Century = 21;
    sAlarm.AlarmDate.Day = 19;
    sAlarm.AlarmDate.Month = RTC_MONTH_JULY;
    sAlarm.AlarmDate.Year = 22;

    sAlarm.MaskAlarmTime = RTC_TALRM_CDOW_M | RTC_TALRM_CH_M | RTC_TALRM_CM_M | RTC_TALRM_CS_M;
    sAlarm.MaskAlarmDate = RTC_DALRM_CC_M | RTC_DALRM_CD_M | RTC_DALRM_CM_M | RTC_DALRM_CY_M;

    HAL_RTC_SetAlarm(&hrtc, &sAlarm);

    HAL_RTC_Enable(&hrtc);
}


void xputc(char c) {
	UART_WriteByte(UART_1, c);
	UART_WaitTransmission(UART_1);
}
