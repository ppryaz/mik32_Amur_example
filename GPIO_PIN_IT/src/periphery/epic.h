#ifndef EPIC_H_INCLUDED
#define EPIC_H_INCLUDED

#define EPIC_MASK_EDGE_SET          0x00
#define EPIC_MASK_EDGE_CLEAR        0x04
#define EPIC_MASK_LEVEL_SET         0x08
#define EPIC_MASK_LEVEL_CLEAR       0x0C    
#define EPIC_CLEAR                  0x18
#define EPIC_STATUS                 0x1C
#define EPIC_RAW_STATUS             0x20  

// Линии прерываний
#define EPIC_LINE_TIMER32_0_S            0
#define EPIC_LINE_UART_0_S               1
#define EPIC_LINE_UART_1_S               2
#define EPIC_LINE_SPI_0_S                3
#define EPIC_LINE_SPI_1_S                4
#define EPIC_LINE_GPIO_IRQ_S             5
#define EPIC_LINE_I2C_0_S                6
#define EPIC_LINE_I2C_1_S                7
#define EPIC_LINE_WDT_S                  8
#define EPIC_LINE_TIMER16_0_S            9
#define EPIC_LINE_TIMER16_1_S            10
#define EPIC_LINE_TIMER16_2_S            11
#define EPIC_LINE_TIMER32_1_S            12
#define EPIC_LINE_TIMER32_2_S            13
#define EPIC_LINE_SPIFI_S                14
#define EPIC_LINE_RTC_S                  15
#define EPIC_LINE_EEPROM_S               16
#define EPIC_LINE_WDT_DOM3_S             17
#define EPIC_LINE_WDT_SPIFI_S            18
#define EPIC_LINE_WDT_EEPROM_S           19
#define EPIC_LINE_DMA_S                  20
#define EPIC_LINE_FREQ_MON_S             21
#define EPIC_LINE_PVD_AVCC_UNDER_S       22
#define EPIC_LINE_PVD_AVCC_OVER_S        23
#define EPIC_LINE_PVD_VCC_UNDER_S        24
#define EPIC_LINE_PVD_VCC_OVER_S         25
#define EPIC_LINE_BATTERY_NON_GOOD_S     26
#define EPIC_LINE_BOR_S                  27
#define EPIC_LINE_TSENS_S                28
#define EPIC_LINE_ADC_S                  29
#define EPIC_LINE_DAC0_S                 30
#define EPIC_LINE_DAC1_S                 31
#define EPIC_LINE_M(interrupt_line)     (1 << interrupt_line)


#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {  
        volatile uint32_t MASK_EDGE_SET;        // 0x00
        volatile uint32_t MASK_EDGE_CLEAR;      // 0x04
        volatile uint32_t MASK_LEVEL_SET;       // 0x08
        volatile uint32_t MASK_LEVEL_CLEAR;     // 0x0C   
        volatile uint32_t reserved[2];          // reserved   
        volatile uint32_t CLEAR;                // 0x18
        volatile uint32_t STATUS;               // 0x1C
        volatile uint32_t RAW_STATUS;           // 0x20  
    } EPIC_TypeDef;
#endif

#endif // EPIC_H_INCLUDED
