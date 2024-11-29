#ifndef GPIO_IRQ_H_INCLUDED
#define GPIO_IRQ_H_INCLUDED


#define GPIO_IRQ_LINE_M(line_num) (1 << (line_num))
// LINE_MUX
#define GPIO_IRQ_LINE_MUX_M(line_num) (0xF << ((line_num) << 2))
#define GPIO_IRQ_LINE_MUX(mux, line_num) (((mux) << ((line_num) << 2)) & GPIO_IRQ_LINE_MUX_M(line_num))

#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        volatile uint32_t STATE;
        volatile uint32_t LINE_MUX;
        volatile uint32_t INTERRUPT;
        volatile uint32_t ENABLE_SET;
        volatile uint32_t ENABLE_CLEAR;
        volatile uint32_t EDGE;
        volatile uint32_t LEVEL;
        volatile uint32_t LEVEL_SET;
        volatile uint32_t LEVEL_CLEAR;
        volatile uint32_t ANY_EDGE_SET;
        volatile uint32_t ANY_EDGE_CLEAR;
        volatile uint32_t CLEAR;        
    } GPIO_IRQ_TypeDef;

#endif

#endif    