#include "riscv_csr_encoding.h"
#include "mcu32_memory_map.h"
#include <pad_config.h>
#include <gpio.h>
#include "mik32_hal_spifi.h"
#include "mik32_hal_pcc.h"
#include "csr.h"

void SystemClock_Config();
void SPIFI_Config();

extern unsigned long __TEXT_START__;

void trap_handler() {


}

void main() {
	// interrupt vector init
	write_csr(mtvec, &__TEXT_START__);

	PM->CLK_APB_P_SET =   PM_CLOCK_APB_P_GPIO_0_M
						| PM_CLOCK_APB_P_UART_0_M;
	PM->CLK_APB_M_SET = PM_CLOCK_APB_M_PAD_CONFIG_M;
	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;

	// pull up D2 and D3 on SPIFI (WP and HOLD disable)
	PAD_CONFIG->PORT_2_CFG = 0x00000555;
	PAD_CONFIG->PORT_2_PUPD = 0x0000500;

    // LEDs init
	GPIO_0->DIRECTION_OUT =  1<<9;
	GPIO_0->DIRECTION_OUT =  1<<10;

	// led on shortly
	GPIO_0->CLEAR = (0b1)<<9;
	GPIO_0->SET = (0b1)<<10;
	for (volatile int i = 0; i < 100000; i++);
	GPIO_0->SET = (0b1)<<9;
	for (volatile int i = 0; i < 100000; i++);
	GPIO_0->CLEAR = (0b1)<<10;
	for (volatile int i = 0; i < 100000; i++);
	GPIO_0->SET = (0b1)<<10;

    // LED DEinit
	GPIO_0->DIRECTION_IN =  1<<9;
	GPIO_0->DIRECTION_IN =  1<<10;

	// Конфигурация кэш памяти
	// SPIFI переключаем в 4 битный режим
    SPIFI_Config();

	/* Загрузка из внешней flash по SPIFI */
    asm volatile( "la ra, 0x80000000\n\t"
                    "jalr ra"
                );

	while (1)
	{
	}
}


SPIFI_CommandTypeDef spifi_read_sreg;
SPIFI_CommandTypeDef spifi_write_sreg;
SPIFI_CommandTypeDef spifi_write_sreg12;

SPIFI_CommandTypeDef spifi_wait_busy;

typedef enum
{
    SREG1 = 0x05,
    SREG2 = 0x35,
} SREG_Num;

SPIFI_CommandTypeDef spifi_write_enable;
SPIFI_CommandTypeDef spifi_fast_read_quad_io_init;

SPIFI_MemoryCommandTypeDef spifi_memory_read;
SPIFI_MemoryCommandTypeDef spifi_quad_memory_read;
SPIFI_MemoryCommandTypeDef spifi_quad_addr_memory_read;

SPIFI_MemoryCommandTypeDef spifi_fast_read_dual;
SPIFI_MemoryCommandTypeDef spifi_fast_read_dual_io;

SPIFI_MemoryModeConfig_HandleTypeDef spifi_mem_config;
SPIFI_HandleTypeDef spifi;

uint8_t read_sreg(SREG_Num sreg_num)
{
    uint8_t value[1] = {0};
    spifi_read_sreg.OpCode = sreg_num;
    HAL_SPIFI_SendCommand(&spifi, &spifi_read_sreg, 0, 1, value);
    return value[0];
}

void write_enable()
{
    HAL_SPIFI_SendCommand(&spifi, &spifi_write_enable, 0, 0, 0);
}

void wait_busy()
{
    while (read_sreg(SREG1) & 1)
        ;
}


void SPIFI_Config()
{
    spifi.Instance = SPIFI_CONFIG;

    SPIFI_CONFIG->STAT |= SPIFI_CONFIG_STAT_INTRQ_M |
                          SPIFI_CONFIG_STAT_RESET_M;

    SPIFI_CONFIG->ADDR = 0x00;
    SPIFI_CONFIG->IDATA = 0x00;
    SPIFI_CONFIG->CLIMIT = 0x00000000;

    spifi_read_sreg.Direction = SPIFI_DIRECTION_INPUT;
    spifi_read_sreg.InterimLength = 0;
    spifi_read_sreg.FieldForm = SPIFI_FIELDFORM_ALL_SERIAL;
    spifi_read_sreg.FrameForm = SPIFI_FRAMEFORM_OPCODE;
    spifi_read_sreg.OpCode = 0x05;

    // uint8_t sreg2 = read_sreg(SREG2);
    //xprintf("sreg2 read = 0x%02x\n", sreg2);

    spifi_write_enable.Direction = SPIFI_DIRECTION_INPUT;
    spifi_write_enable.InterimLength = 0;
    spifi_write_enable.FieldForm = SPIFI_FIELDFORM_ALL_SERIAL;
    spifi_write_enable.FrameForm = SPIFI_FRAMEFORM_OPCODE;
    spifi_write_enable.OpCode = 0x06;


    spifi_write_sreg.Direction = SPIFI_DIRECTION_OUTPUT;
    spifi_write_sreg.InterimLength = 0;
    spifi_write_sreg.FieldForm = SPIFI_FIELDFORM_ALL_SERIAL;
    spifi_write_sreg.FrameForm = SPIFI_FRAMEFORM_OPCODE;
    spifi_write_sreg.OpCode = 0x31;

    spifi_write_sreg12.Direction = SPIFI_DIRECTION_OUTPUT;
    spifi_write_sreg12.InterimLength = 0;
    spifi_write_sreg12.FieldForm = SPIFI_FIELDFORM_ALL_SERIAL;
    spifi_write_sreg12.FrameForm = SPIFI_FRAMEFORM_OPCODE;
    spifi_write_sreg12.OpCode = 0x01;

    uint8_t sreg1 = 0x0;
    uint8_t sreg2 = 0x0;

    sreg1 = read_sreg(SREG1);
    //xprintf("sreg1 read = 0x%02x\n", sreg1);
    sreg2 = read_sreg(SREG2);
    //xprintf("sreg2 read = 0x%02x\n", sreg2);

    write_enable();
    uint8_t sreg12_mod[2] = {sreg1, sreg2 | (1 << 1)};
    HAL_SPIFI_SendCommand(&spifi, &spifi_write_sreg12, 0, 2, sreg12_mod);
    wait_busy();

    sreg1 = read_sreg(SREG1);
    //xprintf("sreg1 read = 0x%02x\n", sreg1);
    sreg2 = read_sreg(SREG2);
    //xprintf("sreg2 read = 0x%02x\n", sreg2);


    spifi_memory_read.InterimLength = 0;
    spifi_memory_read.FieldForm = SPIFI_FIELDFORM_ALL_SERIAL;
    spifi_memory_read.FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR;
    spifi_memory_read.InterimData = 0;
    spifi_memory_read.OpCode = 0x03;

    spifi_quad_memory_read.InterimLength = 1;
    spifi_quad_memory_read.FieldForm = SPIFI_FIELDFORM_DATA_PARALLEL;
    spifi_quad_memory_read.FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR;
    spifi_quad_memory_read.InterimData = 0;
    spifi_quad_memory_read.OpCode = 0x6B;

    spifi_quad_addr_memory_read.InterimLength = 3;
    spifi_quad_addr_memory_read.FieldForm = SPIFI_FIELDFORM_COMMAND_SERIAL;
    spifi_quad_addr_memory_read.FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR;
    spifi_quad_addr_memory_read.InterimData = 0;
    spifi_quad_addr_memory_read.OpCode = 0xEB;


    spifi_fast_read_dual.InterimLength = 1;
    spifi_fast_read_dual.FieldForm = SPIFI_FIELDFORM_DATA_PARALLEL;
    spifi_fast_read_dual.FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR;
    spifi_fast_read_dual.InterimData = 0;
    spifi_fast_read_dual.OpCode = 0x3B;

    spifi_fast_read_dual_io.InterimLength = 1;
    spifi_fast_read_dual_io.FieldForm = SPIFI_FIELDFORM_COMMAND_SERIAL;
    spifi_fast_read_dual_io.FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR;
    spifi_fast_read_dual_io.InterimData = 0;
    spifi_fast_read_dual_io.OpCode = 0xBB;


    spifi_fast_read_quad_io_init.InterimLength = 1;
    spifi_fast_read_quad_io_init.Direction = SPIFI_DIRECTION_INPUT;
    spifi_fast_read_quad_io_init.FieldForm = SPIFI_FIELDFORM_DATA_PARALLEL;
    spifi_fast_read_quad_io_init.FrameForm = SPIFI_FRAMEFORM_OPCODE_3ADDR;
    //spifi_fast_read_quad_io_init.InterimData = 0x00;
    spifi_fast_read_quad_io_init.OpCode = 0x6B;


    uint8_t buf[1] = {0};
    // HAL_SPIFI_SendCommand(&spifi, &spifi_fast_read_quad_io_init, 0, 1, buf);
    HAL_SPIFI_SendCommand(&spifi, &spifi_fast_read_quad_io_init, 0, 1, buf);
    //xprintf("0 byte read = 0x%02x\n", buf[0]);
    HAL_SPIFI_SendCommand(&spifi, &spifi_fast_read_quad_io_init, 1, 1, buf);
    //xprintf("1 byte read = 0x%02x\n", buf[0]);
    HAL_SPIFI_SendCommand(&spifi, &spifi_fast_read_quad_io_init, 2, 1, buf);
    //xprintf("2 byte read = 0x%02x\n", buf[0]);
    HAL_SPIFI_SendCommand(&spifi, &spifi_fast_read_quad_io_init, 3, 1, buf);
    //xprintf("3 byte read = 0x%02x\n", buf[0]);


    spifi_mem_config.CacheEnable = SPIFI_CACHE_ENABLE;
    spifi_mem_config.CacheLimit = 0x90000000;
    spifi_mem_config.Instance = SPIFI_CONFIG;
    spifi_mem_config.Command = spifi_quad_addr_memory_read;
    HAL_SPIFI_MemoryMode_Init(&spifi_mem_config);
}

