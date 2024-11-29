#ifndef PAD_CONFIG_H_INCLUDED
#define PAD_CONFIG_H_INCLUDED

#define PAD_CONFIG_PIN_M(pin) (0b11 << ((pin) << 1))
#define PAD_CONFIG_PIN(pin, value) ((value) << ((pin) << 1))

#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        volatile uint32_t PORT_0_CFG; 
        volatile uint32_t PORT_0_DS; 
        volatile uint32_t PORT_0_PUPD;
        
        volatile uint32_t PORT_1_CFG; 
        volatile uint32_t PORT_1_DS; 
        volatile uint32_t PORT_1_PUPD; 
        
        volatile uint32_t PORT_2_CFG; 
        volatile uint32_t PORT_2_DS; 
        volatile uint32_t PORT_2_PUPD;         
    } PAD_CONFIG_TypeDef;

#endif
    
    
    
#endif // PAD_CONFIG_H_INCLUDED