#ifndef WU_H_INCLUDED
#define WU_H_INCLUDED

#define WU_SYS_Mask_OFFSET              0x00
#define WU_SYS_Level_OFFSET             0x04
#define WU_SYS_PowerOff_OFFSET          0x08
#define WU_PowerSwitch_On_Bat_OFFSET    0x0C
#define WU_Clocks_OFFSET                0x10
#define WU_RTC_Control_OFFSET           0x14
#define WU_BOOT                         0x28


// SYS_MASK
#define WU_MASK_WAKEUP_RTC_S                0
#define WU_MASK_WAKEUP_RTC_M                (1 << WU_MASK_WAKEUP_RTC_S)
#define WU_MASK_WAKEUP_EXT_S                1
#define WU_MASK_WAKEUP_EXT_M                (1 << WU_MASK_WAKEUP_EXT_S)
#define WU_MASK_SYS_RESET_LDO_S             2
#define WU_MASK_SYS_RESET_LDO_M             (1 << WU_MASK_SYS_RESET_LDO_S)
#define WU_MASK_SYS_RESET_VOLTMON_S         3
#define WU_MASK_SYS_RESET_VOLTMON_M         (1 << WU_MASK_SYS_RESET_VOLTMON_S)
#define WU_MASK_SYS_RESET_BOR_S             4
#define WU_MASK_SYS_RESET_BOR_M             (1 << WU_MASK_SYS_RESET_BOR_S)
#define WU_MASK_BU_RESET_BOR_S              5
#define WU_MASK_BU_RESET_BOR_M              (1 << WU_MASK_BU_RESET_BOR_S)

// SYS_LEVEL
#define WU_SYS_LEVEL_RTC_ALARM_S              0
#define WU_SYS_LEVEL_RTC_ALARM_M              (0x1 << WU_SYS_LEVEL_RTC_ALARM_S)
#define WU_SYS_LEVEL_EXT_LINE_S               1
#define WU_SYS_LEVEL_EXT_LINE_M               (0x1 << WU_SYS_LEVEL_EXT_LINE_S)

// SYS_POWEROFF
#define WU_SYS_POWEROFF_S                  0
#define WU_SYS_POWEROFF_M                  (1 << WU_SYS_POWEROFF_S)

// POWER_SWITCH
#define WU_POWER_SWITCH_EN_S                0
#define WU_POWER_SWITCH_EN_M                (1 << WU_POWER_SWITCH_EN_S)
#define WU_POWER_SWITCH_CONTROL_S           1
#define WU_POWER_SWITCH_CONTROL_M           (1 << WU_POWER_SWITCH_CONTROL_S)
#define WU_POWER_SWITCH_CONTROL_VCC_M       (0 << WU_POWER_SWITCH_CONTROL_S)
#define WU_POWER_SWITCH_CONTROL_VCC_BU_M    (1 << WU_POWER_SWITCH_CONTROL_S)
#define WU_POWER_SWITCH_BATT_GOOD_S         2
#define WU_POWER_SWITCH_BATT_GOOD_M         (1 << WU_POWER_SWITCH_BATT_GOOD_S)

//CLOCKS_BU
#define WU_CLOCKS_BU_OSC32K_EN_S           0
#define WU_CLOCKS_BU_OSC32K_EN_M           (1 << WU_CLOCKS_BU_OSC32K_EN_S)
#define WU_CLOCKS_BU_LSI32K_EN_S           1
#define WU_CLOCKS_BU_LSI32K_EN_M           (1 << WU_CLOCKS_BU_LSI32K_EN_S)
#define WU_CLOCKS_BU_ADJ_LSI32K_S          2
#define WU_CLOCKS_BU_ADJ_LSI32K_M          (0xFF << WU_CLOCKS_BU_ADJ_LSI32K_S)
#define WU_CLOCKS_BU_ADJ_LSI32K(v)         (((v) << WU_CLOCKS_BU_ADJ_LSI32K_S) & WU_CLOCKS_BU_ADJ_LSI32K_M)
#define WU_CLOCKS_BU_RTC_CLK_MUX_S         10
#define WU_CLOCKS_BU_RTC_CLK_MUX_M         (0x3 << WU_CLOCKS_BU_RTC_CLK_MUX_S)
#define WU_CLOCKS_BU_RTC_CLK_MUX_LSI32K_M  (0x1 << WU_CLOCKS_BU_RTC_CLK_MUX_S)
#define WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M  (0x2 << WU_CLOCKS_BU_RTC_CLK_MUX_S)
#define WU_CLOCKS_BU_OSC32K_SM_S           14
#define WU_CLOCKS_BU_OSC32K_SM_M           (1 << WU_CLOCKS_BU_OSC32K_SM_S)  

// CLOCKS_SYS
#define WU_CLOCKS_SYS_OSC32M_EN_S                   0
#define WU_CLOCKS_SYS_OSC32M_EN_M                   (1 << WU_CLOCKS_SYS_OSC32M_EN_S)
#define WU_CLOCKS_SYS_HSI32M_EN_S                   1
#define WU_CLOCKS_SYS_HSI32M_EN_M                   (1 << WU_CLOCKS_SYS_HSI32M_EN_S)
#define WU_CLOCKS_SYS_ADJ_HSI32M_S                  2
#define WU_CLOCKS_SYS_ADJ_HSI32M_M                  (0xFF << WU_CLOCKS_SYS_ADJ_HSI32M_S)
#define WU_CLOCKS_SYS_ADJ_HSI32M(v)                 (((v) << WU_CLOCKS_SYS_ADJ_HSI32M_S) & WU_CLOCKS_SYS_ADJ_HSI32M_M)
#define WU_CLOCKS_SYS_FORCE_32K_CLK_S               10
#define WU_CLOCKS_SYS_FORCE_32K_CLK_M               (0b11 << WU_CLOCKS_SYS_FORCE_32K_CLK_S)
#define WU_CLOCKS_SYS_FORCE_32K_CLK_OSC32K_M        (0b01 << WU_CLOCKS_SYS_FORCE_32K_CLK_S)
#define WU_CLOCKS_SYS_FORCE_32K_CLK_LSI32K_M        (0b10 << WU_CLOCKS_SYS_FORCE_32K_CLK_S)

// RTC_CONRTOL
#define WU_RTC_CONTROL_RESET_S                  0
#define WU_RTC_CONTROL_RESET_SET_M              (0 << WU_RTC_CONTROL_RESET_S)
#define WU_RTC_CONTROL_RESET_CLEAR_M            (1 << WU_RTC_CONTROL_RESET_S)

// STOP
#define WU_STOP_S                  0
#define WU_STOP_M                  (1 << WU_STOP_S)

    
#ifndef _ASSEMBLER_
    #include <inttypes.h> 
    typedef struct
    {   
       
        volatile uint32_t SYS_MASK;  
        volatile uint32_t SYS_LEVEL;         
        volatile uint32_t SYS_POWEROFF;         
        volatile uint32_t POWER_SWITCH;
        volatile uint32_t CLOCKS_BU;  
        volatile uint32_t CLOCKS_SYS;  
        volatile uint32_t RTC_CONRTOL;
        volatile uint32_t STOP;
    } WU_TypeDef;
#endif

#endif 

