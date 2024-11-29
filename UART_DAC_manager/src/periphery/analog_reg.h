#ifndef ANALOG_REG_H_INCLUDED
#define ANALOG_REG_H_INCLUDED

#define DAC_CFG_EN18_S          0
#define DAC_CFG_EN18_M          (1 << DAC_CFG_EN18_S)
#define DAC_CFG_RESN_S          1
#define DAC_CFG_RESN_M          (1 << DAC_CFG_RESN_S)
#define DAC_CFG_DIV_S           2
#define DAC_CFG_DIV_M           (0xFF << DAC_CFG_DIV_S)
#define DAC_CFG_DIV(v)          (((v) << DAC_CFG_DIV_S) & DAC_CFG_DIV_M)
#define DAC_CFG_EXTEN_S         10
#define DAC_CFG_EXTEN_M         (1 << DAC_CFG_EXTEN_S)
#define DAC_EXTPAD_EN_S         11
#define DAC_EXTPAD_EN_M         (1 << DAC_EXTPAD_EN_S)
#define DAC_EMPTY_READ_S        13 //Need New Name



#define TSENS_CFG_NPD_S             0
#define TSENS_CFG_NPD_M             (1 << TSENS_CFG_NPD_S)
#define TSENS_CFG_NPD_CLK_S         1
#define TSENS_CFG_NPD_CLK_M         (1 << TSENS_CFG_NPD_CLK_S)
#define TSENS_CFG_NRST_S            2
#define TSENS_CFG_NRST_M            (1 << TSENS_CFG_NRST_S)
#define TSENS_CFG_CLK_MUX_S         3
#define TSENS_CFG_CLK_MUX_M         (0b111 << TSENS_CFG_CLK_MUX_S)
#define TSENS_CFG_DIV_S             6
#define TSENS_CFG_DIV_M             (0x3FF << TSENS_CFG_DIV_S)

#define TSENS_TRESHOLD_HI_S    0
#define TSENS_TRESHOLD_HI_M    (0x3FF << TSENS_TRESHOLD_HI_S)
#define TSENS_TRESHOLD_LOW_S   10
#define TSENS_TRESHOLD_LOW_M   (0x3FF << TSENS_TRESHOLD_LOW_S)

#define TSENS_IRQ_EOC_MASK_S    0
#define TSENS_IRQ_EOC_MASK_M    (1 << TSENS_IRQ_EOC_MASK_S)
#define TSENS_IRQ_HI_MASK_S     1
#define TSENS_IRQ_HI_MASK_M     (1 << TSENS_IRQ_HI_MASK_S)
#define TSENS_IRQ_LOW_MASK_S    2
#define TSENS_IRQ_LOW_MASK_M    (1 << TSENS_IRQ_LOW_MASK_S)
#define TSENS_IRQ_EOC_IRQ_S     3
#define TSENS_IRQ_EOC_IRQ_M     (1 << TSENS_IRQ_EOC_IRQ_S)
#define TSENS_IRQ_HI_IRQ_S      4
#define TSENS_IRQ_HI_IRQ_M      (1 << TSENS_IRQ_HI_IRQ_S)
#define TSENS_IRQ_LOW_IRQ_S     5
#define TSENS_IRQ_LOW_IRQ_M     (1 << TSENS_IRQ_LOW_IRQ_S)

#define TSENS_CLEAR_IRQ_EOC_CLEAR_S     0
#define TSENS_CLEAR_IRQ_EOC_CLEAR_M     (1 << TSENS_CLEAR_IRQ_EOC_CLEAR_S)
#define TSENS_CLEAR_IRQ_HI_CLEAR_S      1
#define TSENS_CLEAR_IRQ_HI_CLEAR_M      (1 << TSENS_CLEAR_IRQ_HI_CLEAR_S)
#define TSENS_CLEAR_IRQ_LOW_CLEAR_S     2
#define TSENS_CLEAR_IRQ_LOW_CLEAR_M     (1 << TSENS_CLEAR_IRQ_LOW_CLEAR_S)

#define TSENS_VALUE_EOC_S               10
#define TSENS_VALUE_EOC_M               (1 << TSENS_VALUE_EOC_S)
#define TSENS_VALUE_VALUE_S             0
#define TSENS_VALUE_VALUE_M             (0x3FF << TSENS_VALUE_VALUE_S)
#define TSENS_CELSIUS_TO_VALUE(i)       (uint32_t)(4096/((10.3*622)/(i+273.15)-10.3 +1))
#define TSENS_VALUE_TO_CELSIUS(v)       ((640660*(v))/(40960+93*(v))*10-27315) // Значение температуры в 100 раз больше


#define ADC_CONFIG_EN_S                0
#define ADC_CONFIG_EN_M                (1 << ADC_CONFIG_EN_S)
#define ADC_CONFIG_RESETN_S            1
#define ADC_CONFIG_RESETN_M            (1 << ADC_CONFIG_RESETN_S)
#define ADC_CONFIG_EXTREF_S            2
#define ADC_CONFIG_EXTREF_M            (1 << ADC_CONFIG_EXTREF_S)
#define ADC_CONFIG_EXTPAD_EN_S         3
#define ADC_CONFIG_EXTPAD_EN_M         (1 << ADC_CONFIG_EXTPAD_EN_S)
#define ADC_CONFIG_SEL_S               4
#define ADC_CONFIG_SEL_M               (0b111 << ADC_CONFIG_SEL_S)
#define ADC_CONFIG_SAH_TIME_S          8
#define ADC_CONFIG_SAH_TIME_M          (0x3F << ADC_CONFIG_SAH_TIME_S)

#define REF_CLB_VCOEF_S         0
#define REF_CLB_VCOEF_M         (0xF << REF_CLB_VCOEF_S)
#define REF_CLB_ICOEF_S         4
#define REF_CLB_ICOEF_M         (0xF << REF_CLB_ICOEF_S)
#define REF_CLB_EN_S            8
#define REF_CLB_EN_M            (1 << REF_CLB_EN_S)

#define TEST_DIG_MUX_COV_LDO_M  (0 << 9)
#define TEST_DIG_MUX_BATON_M    (1 << 9)
#define TEST_DIG_MUX_BOR_M      (2 << 9)
#define TEST_DIG_MUX_PGL_BU_M   (3 << 9)
#define TEST_DIG_MUX_POR_M      (4 << 9)
#define TEST_DIG_MUX_BGO_M      (5 << 9)
#define TEST_DIG_MUX_PGL_M      (6 << 9)

#define TEST_DIG_MUX_OSC_CLK32M_M     (12 << 9)
#define TEST_DIG_MUX_OSC_CLK32K_M     (13 << 9)
#define TEST_DIG_MUX_RC_CLK32M_M      (14 << 9)
#define TEST_DIG_MUX_RC_CLK32K_M      (15 << 9)


#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        volatile uint32_t CFG;
        volatile uint32_t VALUE;        
    } DAC_TypeDef;
    
    
    typedef struct
    {
        volatile DAC_TypeDef    DAC0;       
        volatile DAC_TypeDef    DAC1;   
        volatile uint32_t       PVD_DPF_VALUE;
        volatile uint32_t       PVD_CONFIG;
        volatile uint32_t       PVD_STATUS;
        volatile uint32_t       TSENS_CFG;
        volatile uint32_t       TSENS_THRESHOLD;
        volatile uint32_t       TSENS_IRQ;
        volatile uint32_t       TSENS_CLEAR_IRQ;
        volatile uint32_t       TSENS_VALUE;
        volatile uint32_t       TSENS_SINGLE;
        volatile uint32_t       TSENS_CONTINUOUS;
        volatile uint32_t       REFV_CONFIG;
        volatile uint32_t       ADC_CONFIG;
        volatile uint32_t       ADC_CONTINUOUS;
        volatile uint32_t       ADC_SINGLE;
        volatile uint32_t       ADC_VALID;
        volatile uint32_t       ADC_VALUE;
        volatile uint32_t       TEST_MUX;
        
        
    } ANALOG_REG_TypeDef;
    
#endif

#endif // ANALOG_REG_H_INCLUDED

    