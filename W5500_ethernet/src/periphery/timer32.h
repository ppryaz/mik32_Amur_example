#ifndef TIMER32_H_INCLUDED
#define TIMER32_H_INCLUDED


#define TIMER32_VALUE			    0x00
#define TIMER32_TOP				    0x04
#define TIMER32_PRESCALER		    0x08
#define TIMER32_CONTROL			    0x0C
#define TIMER32_ENABLE              0x10

#define TIMER32_INT_MASK       	    0x14   
#define TIMER32_INT_CLEAR      	    0x18   
#define TIMER32_INT_FLAGS           0x1C   

#define TIMER32_CH_BASE			    0x80
#define TIMER32_CH_CONTROL_OFFSET	0x00
#define TIMER32_CH_OCR_OFFSET		0x04
#define TIMER32_CH_ICR_OFFSET		0x08
#define TIMER32_CH_SIZE			    0x10


// PRESCALER
#define TIMER32_PRESCALER_ENABLE_S	        8
#define TIMER32_PRESCALER_ENABLE_M	        (1 << TIMER32_PRESCALER_ENABLE_S)
#define TIMER32_PRESCALER_DISABLE_M	        (0 << TIMER32_PRESCALER_ENABLE_S)
//
#define TIMER32_PRESCALER_S			        0
#define TIMER32_PRESCALER_M			        (0xFF << TIMER32_PRESCALER_S)

// CONTROL
#define TIMER32_CONTROL_CLOCK_S	    	    2
#define TIMER32_CONTROL_CLOCK_M		        (0x3 << TIMER32_CONTROL_CLOCK_S)
#define TIMER32_CONTROL_CLOCK_PRESCALER_M	(0x0 << TIMER32_CONTROL_CLOCK_S)
#define TIMER32_CONTROL_CLOCK_TIM1_M		(0x1 << TIMER32_CONTROL_CLOCK_S)
#define TIMER32_CONTROL_CLOCK_TX_PIN_M		(0x2 << TIMER32_CONTROL_CLOCK_S)
#define TIMER32_CONTROL_CLOCK_TIM2_M		(0x3 << TIMER32_CONTROL_CLOCK_S)
//
#define TIMER32_CONTROL_MODE_S	        	0
#define TIMER32_CONTROL_MODE_M		        (0x3 << TIMER32_CONTROL_MODE_S)
#define TIMER32_CONTROL_MODE_UP_M	        (0 << TIMER32_CONTROL_MODE_S)
#define TIMER32_CONTROL_MODE_DOWN_M	        (1 << TIMER32_CONTROL_MODE_S)
#define TIMER32_CONTROL_MODE_BIDIR_M	    (2 << TIMER32_CONTROL_MODE_S)	

// ENABLE
#define TIMER32_ENABLE_TIM_CLR_S		    1
#define TIMER32_ENABLE_TIM_CLR_M		    (1 << TIMER32_ENABLE_TIM_CLR_S)
//
#define TIMER32_ENABLE_TIM_EN_S			    0
#define TIMER32_ENABLE_TIM_EN_M			    (1 << TIMER32_ENABLE_TIM_EN_S)

// INT_MASK, INT_CLEAR, INT_FLAGS
#define TIMER32_INT_OVERFLOW_S		        0
#define TIMER32_INT_OVERFLOW_M		        (1 << TIMER32_INT_OVERFLOW_S)
#define TIMER32_INT_UNDERFLOW_S		        1
#define TIMER32_INT_UNDERFLOW_M		        (1 << TIMER32_INT_UNDERFLOW_S)
#define TIMER32_INT_IC_S                    2 
#define TIMER32_INT_IC_M(channelIndex)      ((1 << channelIndex) << TIMER32_INT_IC_S)
#define TIMER32_INT_OC_S                    6 
#define TIMER32_INT_OC_M(channelIndex)      ((1 << channelIndex) << TIMER32_INT_OC_S)


//CH_CNTRL
#define TIMER32_CH_CNTRL_NOISE_S	        0
#define TIMER32_CH_CNTRL_NOISE_M	        (1 << TIMER32_CH_CNTRL_NOISE_S)
//
#define TIMER32_CH_CNTRL_CAPTURE_EDGE_S	    4
#define TIMER32_CH_CNTRL_CAPTURE_EDGE_M	    (1 << TIMER32_CH_CNTRL_CAPTURE_EDGE_S)
#define TIMER32_CH_CNTRL_CAPTURE_POS_M	    (0 << TIMER32_CH_CNTRL_CAPTURE_EDGE_S)
#define TIMER32_CH_CNTRL_CAPTURE_NEG_M	    (1 << TIMER32_CH_CNTRL_CAPTURE_EDGE_S)
//
#define TIMER32_CH_CNTRL_MODE_S			    5
#define TIMER32_CH_CNTRL_MODE_M			    (3 << TIMER32_CH_CNTRL_MODE_S)
#define TIMER32_CH_CNTRL_MODE_COMPARE_M	    (1 << TIMER32_CH_CNTRL_MODE_S)
#define TIMER32_CH_CNTRL_MODE_CAPTURE_M	    (2 << TIMER32_CH_CNTRL_MODE_S)
#define TIMER32_CH_CNTRL_MODE_PWM_M		    (3 << TIMER32_CH_CNTRL_MODE_S)
//
#define TIMER32_CH_CNTRL_ENABLE_S		    7
#define TIMER32_CH_CNTRL_ENABLE_M		    (1 << TIMER32_CH_CNTRL_ENABLE_S)
#define TIMER32_CH_CNTRL_DISABLE_M		    (0 << TIMER32_CH_CNTRL_ENABLE_S)
//
#define TIMER32_CH_CNTRL_INVERTED_PWM_S	    8
#define TIMER32_CH_CNTRL_INVERTED_PWM_M	    (1 << TIMER32_CH_CNTRL_INVERTED_PWM_S)
//
#define TIMER32_CH_CNTRL_DIR_S	            9
#define TIMER32_CH_CNTRL_DIR_M	            (1 << TIMER32_CH_CNTRL_DIR_S)


#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        volatile uint32_t CNTRL;
        volatile uint32_t OCR;
        volatile uint32_t ICR;
        volatile uint32_t reserved0;
    } TIMER32_CHANNEL_TypeDef;
    
    typedef struct
    {
        volatile uint32_t VALUE;        /* Offset: 0x000 (R)  */
        volatile uint32_t TOP;          /* Offset: 0x004 (R/W)  */
        volatile uint32_t PRESCALER;    /* Offset: 0x008 (R/W)  */
        volatile uint32_t CONTROL;      /* Offset: 0x00C (R/W)  */
        volatile uint32_t ENABLE;       /* Offset: 0x010 (R/W)  */
        volatile uint32_t INT_MASK;      /* Offset: 0x014 (R/W)  */
        volatile uint32_t INT_CLEAR;     /* Offset: 0x018 (R/W)  */
        volatile uint32_t INT_FLAGS;     /* Offset: 0x01C (R)  */
        
        volatile uint8_t  reserved0[0x60];
        TIMER32_CHANNEL_TypeDef CHANNELS[4];
        
    } TIMER32_TypeDef;
#endif

#endif // TIMER32_H_INCLUDED
