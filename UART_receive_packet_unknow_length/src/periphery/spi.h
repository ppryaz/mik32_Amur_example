#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#define SPI_CONFIG        	0X00
#define SPI_STATUS        	0X04
#define SPI_IEN           	0X08 
#define SPI_IDIS          	0X0C 
#define SPI_IMASK         	0X10 
#define SPI_ENABLE        	0X14
#define SPI_DELAY         	0X18
#define SPI_TXD           	0X1C
#define SPI_RXD           	0X20
#define SPI_SIC           	0X24
#define SPI_THRESHOLD     	0X28
#define SPI_MODULE_ID     	0XFC


#define SPI_CONFIG_MANUAL_CS_S		              14
#define SPI_CONFIG_MANUAL_CS_M		              (1 << SPI_CONFIG_MANUAL_CS_S)
//
#define SPI_CONFIG_CS_S				                  10
#define SPI_CONFIG_CS_M				                  (0XF << SPI_CONFIG_CS_S)
#define SPI_CONFIG_CS_0_M			                  (0XE << SPI_CONFIG_CS_S)
#define SPI_CONFIG_CS_1_M			                  (0XD << SPI_CONFIG_CS_S)
#define SPI_CONFIG_CS_2_M			                  (0XB << SPI_CONFIG_CS_S)
#define SPI_CONFIG_CS_3_M			                  (0X7 << SPI_CONFIG_CS_S)
#define SPI_CONFIG_CS_NONE_M		                (0XF << SPI_CONFIG_CS_S)
//
#define SPI_CONFIG_PERI_SEL_S		                9
#define SPI_CONFIG_PERI_SEL_M		                (1 << SPI_CONFIG_PERI_SEL_S)
#define SPI_CONFIG_REF_CLK_S		                8
#define SPI_CONFIG_REF_CLK_M		                (1 << SPI_CONFIG_REF_CLK_S)
//
#define SPI_CONFIG_BAUD_RATE_DIV_S		          3
#define SPI_CONFIG_BAUD_RATE_DIV_M		          (0X7 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_2_M	          (0X0 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_4_M	          (0X1 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_8_M	          (0X2 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_16_M	          (0X3 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_32_M	          (0X4 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_64_M	          (0X5 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_128_M	        (0X6 << SPI_CONFIG_BAUD_RATE_DIV_S)
#define SPI_CONFIG_BAUD_RATE_DIV_256_M	        (0X7 << SPI_CONFIG_BAUD_RATE_DIV_S)
//
#define SPI_MAXIMUM_BAUD_RATE_DIV		            256
//
#define SPI_CONFIG_CLK_PH_S				              2
#define SPI_CONFIG_CLK_PH_M				              (1 << SPI_CONFIG_CLK_PH_S)
#define SPI_CONFIG_CLK_POL_S			              1
#define SPI_CONFIG_CLK_POL_M			              (1 << SPI_CONFIG_CLK_POL_S)
#define SPI_CONFIG_MODE_SEL_S			              0
#define SPI_CONFIG_MODE_SEL_M			              (1 << SPI_CONFIG_MODE_SEL_S)
#define SPI_CONFIG_MASTER_M				              (1 << SPI_CONFIG_MODE_SEL_S)
#define SPI_CONFIG_SLAVE_M				              (0 << SPI_CONFIG_MODE_SEL_S)


#define SPI_ENABLE_CLEAR_RX_FIFO_S              3
#define SPI_ENABLE_CLEAR_RX_FIFO_M              (1 << SPI_ENABLE_CLEAR_RX_FIFO_S)
#define SPI_ENABLE_CLEAR_TX_FIFO_S              2
#define SPI_ENABLE_CLEAR_TX_FIFO_M              (1 << SPI_ENABLE_CLEAR_TX_FIFO_S)
#define SPI_ENABLE_S		                        0
#define SPI_ENABLE_M		                        (1 << SPI_ENABLE_S)


#define SPI_DELAY_BTWN_S                        16
#define SPI_DELAY_BTWN_M                        (0XFF << SPI_DELAY_BTWN_S)
#define SPI_DELAY_BTWN(V)                       (((V) << SPI_DELAY_BTWN_S) & SPI_DELAY_BTWN_M)
#define SPI_DELAY_AFTER_S                       8
#define SPI_DELAY_AFTER_M                       (0XFF << SPI_DELAY_AFTER_S)
#define SPI_DELAY_AFTER(V)                      (((V) << SPI_DELAY_AFTER_S) & SPI_DELAY_AFTER_M)
#define SPI_DELAY_INIT_S                        0
#define SPI_DELAY_INIT_M                        (0XFF << SPI_DELAY_INIT_S)
#define SPI_DELAY_INIT(V)                       (((V) << SPI_DELAY_INIT_S) & SPI_DELAY_INIT_M)


#define SPI_INT_STATUS_SPI_ACTIVE_S             15
#define SPI_INT_STATUS_SPI_ACTIVE_M             (1 << SPI_INT_STATUS_SPI_ACTIVE_S)
#define SPI_INT_STATUS_TX_FIFO_UNDERFLOW_S      6
#define SPI_INT_STATUS_TX_FIFO_UNDERFLOW_M      (1 << SPI_INT_STATUS_TX_FIFO_UNDERFLOW_S)
#define SPI_INT_STATUS_RX_OVERFLOW_S		        0
#define SPI_INT_STATUS_RX_OVERFLOW_M		        (1 << SPI_INT_STATUS_RX_OVERFLOW_S)
#define SPI_INT_STATUS_MODE_FAIL_S			        1
#define SPI_INT_STATUS_MODE_FAIL_M			        (1 << SPI_INT_STATUS_MODE_FAIL_S)
#define SPI_INT_STATUS_TX_FIFO_NOT_FULL_S	      2
#define SPI_INT_STATUS_TX_FIFO_NOT_FULL_M	      (1 << SPI_INT_STATUS_TX_FIFO_NOT_FULL_S)
#define SPI_INT_STATUS_TX_FIFO_FULL_S		        3
#define SPI_INT_STATUS_TX_FIFO_FULL_M		        (1 << SPI_INT_STATUS_TX_FIFO_FULL_S)
#define SPI_INT_STATUS_RX_FIFO_NOT_EMPTY_S	    4
#define SPI_INT_STATUS_RX_FIFO_NOT_EMPTY_M	    (1 << SPI_INT_STATUS_RX_FIFO_NOT_EMPTY_S)
#define SPI_INT_STATUS_RX_FIFO_FULL_S		        5
#define SPI_INT_STATUS_RX_FIFO_FULL_M		        (1 << SPI_INT_STATUS_RX_FIFO_FULL_S)



#ifndef _ASSEMBLER_
    #include <inttypes.h> 
    typedef struct
    {
      volatile uint32_t CONFIG;             /* Offset: 0x000 (R/W)    */
      volatile uint32_t INT_STATUS;         /* Offset: 0x004 (R/RC)  */
      volatile uint32_t INT_ENABLE;         /* Offset: 0x008 (WO) */
      volatile uint32_t INT_DISABLE;        /* Offset: 0x00C (WO)  */
      volatile uint32_t INT_MASK;           /* Offset: 0x010 (R)  */
      volatile uint32_t ENABLE;             /* Offset: 0x014 (R/W)  */
      volatile uint32_t DELAY;              /* Offset: 0x018 (R/W)  */
      volatile uint32_t TXDATA;             /* Offset: 0x01C (WO)  */
      volatile uint32_t RXDATA;             /* Offset: 0x020 (RO)  */
      volatile uint32_t SIC;                /* Offset: 0x024 (R/W)  Slave_Idle_Count */
      volatile uint32_t TX_THR;             /* Offset: 0x028 (R/W)  TX threshold */
      volatile uint32_t reserved[0X34];     /* Empty array to fill the space*/
      volatile uint32_t ID;                 /* Offset: 0x0FC (RO)  Module ID 0x01090100 */
    }SPI_TypeDef;
#endif


#endif // SPI_H_INCLUDED
