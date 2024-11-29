#ifndef PVD_CONTROL_H_INCLUDED
#define PVD_CONTROL_H_INCLUDED

#define PVD_DPF_VALUE_S            0
#define PVD_DPF_VALUE_M            (0xFFFF << PVD_DPF_VALUE_S)

#define PVD_CONFIG_PD_S            0
#define PVD_CONFIG_PD_M            (1 << PVD_CONFIG_PD_S)
#define PVD_CONFIG_NRESETU_S       1
#define PVD_CONFIG_NRESETU_M       (1 << PVD_CONFIG_NRESETU_S)
#define PVD_CONFIG_NRESETO_S       2
#define PVD_CONFIG_NRESETO_M       (1 << PVD_CONFIG_NRESETO_S)
#define PVD_CONFIG_TESTMODE_S      3
#define PVD_CONFIG_TESTMODE_M      (1 << PVD_CONFIG_TESTMODE_S)
#define PVD_CONFIG_UNDER_THRESH_S  4
#define PVD_CONFIG_UNDER_THRESH_M  (0b1111 << PVD_CONFIG_UNDER_THRESH_S)
#define PVD_CONFIG_OVER_THRESH_S   8
#define PVD_CONFIG_OVER_THRESH_M   (0b1111 << PVD_CONFIG_OVER_THRESH_S)
#define PVD_CONFIG_EN_VREFCLB_S    12
#define PVD_CONFIG_EN_VREFCLB_M    (1 << PVD_CONFIG_EN_VREFCLB_S)

#define PVD_STATUS_UNDER_VALUE_S   0
#define PVD_STATUS_UNDER_VALUE_M   (1 << PVD_STATUS_UNDER_VALUE_S)
#define PVD_STATUS_OVER_VALUE_S    1
#define PVD_STATUS_OVER_VALUE_M    (1 << PVD_STATUS_OVER_VALUE_S)
#define PVD_STATUS_UNDER_FLAG_S    2
#define PVD_STATUS_UNDER_FLAG_M    (1 << PVD_STATUS_UNDER_FLAG_S)
#define PVD_STATUS_OVER_FLAG_S     3
#define PVD_STATUS_OVER_FLAG_S     (1 << PVD_STATUS_OVER_FLAG_S)

#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        volatile uint32_t       DPF_VALUE;
        volatile uint32_t       CONFIG;
        volatile uint32_t       STATUS;
    } PVD_CONTROL_TypeDef;
    
#endif

#endif // PVD_CONTROL_H_INCLUDED

