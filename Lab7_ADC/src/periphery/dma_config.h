#ifndef DMA_MC_H_INCLUDED
#define DMA_MC_H_INCLUDED

#define DMA_CHANNEL_COUNT                   4


#define DMA_CH_CFG_ENABLE_S                 0
#define DMA_CH_CFG_ENABLE_M                 (1 << DMA_CH_CFG_ENABLE_S)  
#define DMA_CH_CFG_DISABLE_M                (0 << DMA_CH_CFG_ENABLE_S)  

#define DMA_CH_CFG_PRIOR_S                  1
#define DMA_CH_CFG_PRIOR_M                  (0X3 << DMA_CH_CFG_PRIOR_S)
#define DMA_CH_CFG_PRIOR_LOW_M              (0B00 << DMA_CH_CFG_PRIOR_S)
#define DMA_CH_CFG_PRIOR_MEDIUM_M           (0B01 << DMA_CH_CFG_PRIOR_S)
#define DMA_CH_CFG_PRIOR_HIGH_M             (0B10 << DMA_CH_CFG_PRIOR_S)
#define DMA_CH_CFG_PRIOR_VERY_HIGH_M        (0B11 << DMA_CH_CFG_PRIOR_S)

#define DMA_CH_CFG_READ_MODE_S              3
#define DMA_CH_CFG_READ_MODE_MEMORY_M       (1 << DMA_CH_CFG_READ_MODE_S)
#define DMA_CH_CFG_READ_MODE_PERIPHERY_M    (0 << DMA_CH_CFG_READ_MODE_S)

#define DMA_CH_CFG_WRITE_MODE_S             4
#define DMA_CH_CFG_WRITE_MODE_MEMORY_M      (1 << DMA_CH_CFG_WRITE_MODE_S)
#define DMA_CH_CFG_WRITE_MODE_PERIPHERY_M   (0 << DMA_CH_CFG_WRITE_MODE_S)

#define DMA_CH_CFG_READ_INCREMENT_S         5
#define DMA_CH_CFG_READ_INCREMENT_M         (1 << DMA_CH_CFG_READ_INCREMENT_S)
#define DMA_CH_CFG_READ_NO_INCREMENT_M      (0 << DMA_CH_CFG_READ_INCREMENT_S)

#define DMA_CH_CFG_WRITE_INCREMENT_S        6
#define DMA_CH_CFG_WRITE_INCREMENT_M        (1 << DMA_CH_CFG_WRITE_INCREMENT_S)
#define DMA_CH_CFG_WRITE_NO_INCREMENT_M     (0 << DMA_CH_CFG_WRITE_INCREMENT_S)

#define DMA_CH_CFG_READ_SIZE_S              7
#define DMA_CH_CFG_READ_SIZE_BYTE_M         (0B00 << DMA_CH_CFG_READ_SIZE_S)      // байт
#define DMA_CH_CFG_READ_SIZE_2BYTE_M        (0B01 << DMA_CH_CFG_READ_SIZE_S)     // полуслово
#define DMA_CH_CFG_READ_SIZE_4BYTE_M        (0B10 << DMA_CH_CFG_READ_SIZE_S)     // слово
#define DMA_CH_CFG_READ_SIZE_REZ_M          (0B11 << DMA_CH_CFG_READ_SIZE_S)      // резерв

#define DMA_CH_CFG_WRITE_SIZE_S             9
#define DMA_CH_CFG_WRITE_SIZE_BYTE_M        (0B00 << DMA_CH_CFG_WRITE_SIZE_S)  // байт
#define DMA_CH_CFG_WRITE_SIZE_2BYTE_M       (0B01 << DMA_CH_CFG_WRITE_SIZE_S)  // полуслово
#define DMA_CH_CFG_WRITE_SIZE_4BYTE_M       (0B10 << DMA_CH_CFG_WRITE_SIZE_S)  // слово
#define DMA_CH_CFG_WRITE_SIZE_REZ_M         (0B11 << DMA_CH_CFG_WRITE_SIZE_S)  // резерв

#define DMA_CH_CFG_READ_BURST_SIZE_S        11    //Кол-во байт пакетной передачи:   2^Read_burst_size
#define DMA_CH_CFG_WRITE_BURST_SIZE_S       14    //Кол-во байт пакетной передачи:   2^Write_burst_size

#define DMA_CH_CFG_READ_REQUETS_S               17    // выбор канала чтения 
#define DMA_CH_CFG_READ_REQUETS_M               (0xF << DMA_CH_CFG_READ_REQUETS_S)
#define DMA_CH_CFG_READ_REQUETS(v)              (((v) << DMA_CH_CFG_READ_REQUETS_S) & DMA_CH_CFG_READ_REQUETS_M)

#define DMA_CH_CFG_WRITE_REQUETS_S              21    // выбор канала записи 
#define DMA_CH_CFG_WRITE_REQUETS_M              (0xF << DMA_CH_CFG_WRITE_REQUETS_S)
#define DMA_CH_CFG_WRITE_REQUETS(v)             (((v) << DMA_CH_CFG_WRITE_REQUETS_S) & DMA_CH_CFG_WRITE_REQUETS_M)


#define DMA_CH_CFG_READ_ACK_EN_S               25
#define DMA_CH_CFG_READ_ACK_EN_M               (1 << DMA_CH_CFG_READ_ACK_EN_S)
#define DMA_CH_CFG_WRITE_ACK_EN_S              26
#define DMA_CH_CFG_WRITE_ACK_EN_M              (1 << DMA_CH_CFG_WRITE_ACK_EN_S)
#define DMA_CH_CFG_IRQ_EN_S                 27
#define DMA_CH_CFG_IRQ_EN_M                 (1 << DMA_CH_CFG_IRQ_EN_S)

#define DMA_CHANNEL_M                       ((1 << DMA_CHANNEL_COUNT) - 1) 

#define DMA_STATUS_READY_S                  0
#define DMA_STATUS_READY_M                  (DMA_CHANNEL_M << DMA_STATUS_READY_S)
#define DMA_STATUS_READY(i)                 ((1 << (DMA_STATUS_READY_S + (i))) & DMA_STATUS_READY_M)
#define DMA_STATUS_CHANNEL_IRQ_S            1 * DMA_CHANNEL_COUNT
#define DMA_STATUS_CHANNEL_IRQ_M            (DMA_CHANNEL_M << DMA_STATUS_CHANNEL_IRQ_S)
#define DMA_STATUS_CHANNEL_BUS_ERROR_S      2 * DMA_CHANNEL_COUNT
#define DMA_STATUS_CHANNEL_BUS_ERROR_M      (DMA_CHANNEL_M << DMA_STATUS_CHANNEL_BUS_ERROR_S)

#define DMA_CONFIG_CLEAR_LOCAL_IRQ_S       0
#define DMA_CONFIG_CLEAR_LOCAL_IRQ_M       (DMA_CHANNEL_M << DMA_CONFIG_CLEAR_LOCAL_IRQ_S)
#define DMA_CONFIG_CLEAR_LOCAL_IRQ(i)      ((1 << (DMA_CONTROL_CLEAR_LOCAL_IRQ_S + (i))) & DMA_CONTROL_CLEAR_LOCAL_IRQ_M)
#define DMA_CONFIG_CLEAR_GLOBAL_IRQ_S      (DMA_CHANNEL_COUNT + 0)
#define DMA_CONFIG_CLEAR_GLOBAL_IRQ_M      (1 << DMA_CONFIG_CLEAR_GLOBAL_IRQ_S)
#define DMA_CONFIG_CLEAR_ERROR_IRQ_S       (DMA_CHANNEL_COUNT + 1)
#define DMA_CONFIG_CLEAR_ERROR_IRQ_M       (1 << DMA_CONFIG_CLEAR_ERROR_IRQ_S)
#define DMA_CONFIG_GLOBAL_IRQ_ENA_S        (DMA_CHANNEL_COUNT + 2)
#define DMA_CONFIG_GLOBAL_IRQ_ENA_M        (1 << DMA_CONFIG_GLOBAL_IRQ_ENA_S)
#define DMA_CONFIG_ERROR_IRQ_ENA_S         (DMA_CHANNEL_COUNT + 3)
#define DMA_CONFIG_ERROR_IRQ_ENA_M         (1 << DMA_CONFIG_ERROR_IRQ_ENA_S)
#define DMA_CONFIG_CURRENT_VALUE_S         (DMA_CHANNEL_COUNT + 4)
#define DMA_CONFIG_CURRENT_VALUE_M         (1 << DMA_CONFIG_CURRENT_VALUE_S)

#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {

        volatile uint32_t DST;      // 0x00
        volatile uint32_t SRC;      // 0x04
        volatile uint32_t LEN;      // 0x08        
        volatile uint32_t CFG;      // 0x0c
    } DMA_CHANNEL_TypeDef;
    
    typedef struct
    { 
        DMA_CHANNEL_TypeDef CHANNELS[DMA_CHANNEL_COUNT];
        volatile uint32_t CONFIG_STATUS;    // 0x40 
    } DMA_CONFIG_TypeDef;
#endif


#endif //
