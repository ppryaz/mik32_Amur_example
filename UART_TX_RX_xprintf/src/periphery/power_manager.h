#ifndef PM_H_INCLUDED
#define PM_H_INCLUDED

#define PM_DIV_AHB_OFFSET           0x00
#define PM_DIV_APB_M_OFFSET         0x04
#define PM_DIV_APB_P_OFFSET         0x08
#define PM_CLK_AHB_SET_OFFSET       0x0C
#define PM_CLK_AHB_CLEAR_OFFSET     0x10
#define PM_CLK_APB_M_SET_OFFSET     0x14
#define PM_CLK_APB_M_CLEAR_OFFSET   0x18
#define PM_CLK_APB_P_SET_OFFSET     0x1C
#define PM_CLK_APB_P_CLEAR_OFFSET   0x20
#define PM_AHB_CLK_MUX_OFFSET       0x24
#define PM_WDT_CLK_MUX_OFFSET       0x28
#define PM_CPU_RTC_CLK_MUX_OFFSET   0x2C
#define PM_TIMER_CFG_OFFSET         0x30
#define PM_FREQ_MASK_OFFSET         0x34
#define PM_FREQ_STATUS_OFFSET       0x38
#define PM_SLEEP_MODE_OFFSET        0x3C


#define PM_AHB_CLK_MUX_S             0
#define PM_AHB_CLK_MUX_M            (0x3 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_OSC32M_M     (0x0 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_HSI32M_M     (0x1 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_OSC32K_M     (0x2 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_LSI32K_M     (0x3 << PM_AHB_CLK_MUX_S)
#define PM_AHB_FORCE_MUX_S           2
#define PM_AHB_FORCE_MUX_M          (1 << PM_AHB_FORCE_MUX_S)
#define PM_AHB_FORCE_MUX_FIXED      (1 << PM_AHB_FORCE_MUX_S)
#define PM_AHB_FORCE_MUX_UNFIXED    (0 << PM_AHB_FORCE_MUX_S)

#define PM_WDT_CLK_MUX_S            0
#define PM_WDT_CLK_MUX_M            (3 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_OSC32M_M     (0 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_HSI32M_M     (1 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_OSC32K_M     (2 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_LSI32K_M     (3 << PM_WDT_CLK_MUX_S)

#define PM_CPU_RTC_CLK_MUX_S             0
#define PM_CPU_RTC_CLK_MUX_M            (1 << PM_CPU_RTC_CLK_MUX_S)
#define PM_CPU_RTC_CLK_MUX_OSC32K_M     (0 << PM_CPU_RTC_CLK_MUX_S)
#define PM_CPU_RTC_CLK_MUX_LSI32K_M     (1 << PM_CPU_RTC_CLK_MUX_S)

#define PM_TIMER_CFG_MUX_TIMER32_0_S    0
#define PM_TIMER_CFG_MUX_TIMER32_1_S    3
#define PM_TIMER_CFG_MUX_TIMER32_2_S    6
#define PM_TIMER_CFG_MUX_TIMER32_TIM1_SYS_CLK_M(timer_index)  (0b00 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER32_TIM1_HCLK_M(timer_index)     (0b01 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER32_TIM2_OSC32K_M(timer_index)   (0b01 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER32_TIM2_LSI32K_M(timer_index)   (0b10 << (timer_index))

#define PM_TIMER_CFG_MUX_TIMER16_0_S    9
#define PM_TIMER_CFG_MUX_TIMER16_1_S    12
#define PM_TIMER_CFG_MUX_TIMER16_2_S    15
#define PM_TIMER_CFG_MUX_TIMER16_SYS_CLK_M(timer_index)     (0x0 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_HCLK_M(timer_index)     (0x1 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_OSC32M_M(timer_index)     (0x2 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_HSI32M_M(timer_index)     (0x3 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_OSC32K_M(timer_index)     (0x4 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_LSI32K_M(timer_index)     (0x5 << (timer_index))

#define PM_TIMER_CFG_MUX_TIMER_M(timer_index)     (0x7 << (timer_index))

#define PM_FREQ_MASK_FORCE_DIV_S        4
#define PM_FREQ_MASK_FORCE_DIV_M        (1 << PM_FREQ_MASK_FORCE_DIV_S)
#define PM_FREQ_MASK_OSC32M_S           3
#define PM_FREQ_MASK_OSC32M_M           (1 << PM_FREQ_MASK_OSC32M_S)
#define PM_FREQ_MASK_HSI32M_S           2
#define PM_FREQ_MASK_HSI32M_M           (1 << PM_FREQ_MASK_HSI32M_S)
#define PM_FREQ_MASK_OSC32K_S           1
#define PM_FREQ_MASK_OSC32K_M           (1 << PM_FREQ_MASK_OSC32K_S)
#define PM_FREQ_MASK_LSI32K_S           0
#define PM_FREQ_MASK_LSI32K_M           (1 << PM_FREQ_MASK_LSI32K_S)

#define PM_FREQ_STATUS_OSC32M_S         3
#define PM_FREQ_STATUS_OSC32M_M         (1 << PM_FREQ_STATUS_OSC32M_S)
#define PM_FREQ_STATUS_HSI32M_S         2
#define PM_FREQ_STATUS_HSI32M_M         (1 << PM_FREQ_STATUS_HSI32M_S)
#define PM_FREQ_STATUS_OSC32K_S         1
#define PM_FREQ_STATUS_OSC32K_M         (1 << PM_FREQ_STATUS_OSC32K_S)
#define PM_FREQ_STATUS_LSI32K_S         0
#define PM_FREQ_STATUS_LSI32K_M         (1 << PM_FREQ_STATUS_LSI32K_S)

#define PM_SLEEP_MODE_S     0
#define PM_SLEEP_MODE_M    (1 << PM_SLEEP_MODE_S)

#ifndef _ASSEMBLER_
    #include <inttypes.h> 
    typedef struct
    {   
       
      
        volatile uint32_t DIV_AHB;         
        volatile uint32_t DIV_APB_M;         
        volatile uint32_t DIV_APB_P;         
        volatile uint32_t CLK_AHB_SET;
        volatile uint32_t CLK_AHB_CLEAR;         
        volatile uint32_t CLK_APB_M_SET;
        volatile uint32_t CLK_APB_M_CLEAR;         
        volatile uint32_t CLK_APB_P_SET;
        volatile uint32_t CLK_APB_P_CLEAR;         
        volatile uint32_t AHB_CLK_MUX;
        volatile uint32_t WDT_CLK_MUX;    
        volatile uint32_t CPU_RTC_CLK_MUX;   
        volatile uint32_t TIMER_CFG;  
        volatile uint32_t FREQ_MASK;  
        volatile uint32_t FREQ_STATUS;  
        volatile uint32_t SLEEP_MODE;  

    } PM_TypeDef;
#endif

#endif 

