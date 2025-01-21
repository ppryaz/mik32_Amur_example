#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED

//--------------------------
// EEPROM register fields
//--------------------------

//EEDAT
//EEA
#define EEPROM_EEA_ADDR_S               2
#define EEPROM_EEA_ADDR_M               (0b7FF << EEPROM_EEA_ADDR_S)
#define EEPROM_EEA_ADDR(v)              (((v) << EEPROM_EEA_ADDR_S) & EEPROM_EEA_ADDR_M)
//EECON
#define EEPROM_EECON_EX_S               0
#define EEPROM_EECON_EX_M               (1 << EEPROM_EECON_EX_S)
#define EEPROM_EECON_OP_S               1
#define EEPROM_EECON_OP_M               (0b11 << EEPROM_EECON_OP_S)
#define EEPROM_EECON_OP(v)              (((v) << EEPROM_EECON_OP_S) & EEPROM_EECON_OP_M)
#define EEPROM_EECON_WRBEH_S            3
#define EEPROM_EECON_WRBEH_M            (0b11 << EEPROM_EECON_WRBEH_S)
#define EEPROM_EECON_WRBEH(v)           (((v) << EEPROM_EECON_WRBEH_S) & EEPROM_EECON_WRBEH_M)
#define EEPROM_EECON_APBNWS_S           5
#define EEPROM_EECON_APBNWS_M           (1 << EEPROM_EECON_APBNWS_S)
#define EEPROM_EECON_DISECC_S           6
#define EEPROM_EECON_DISECC_M           (1 << EEPROM_EECON_DISECC_S)
#define EEPROM_EECON_BWE_S              7
#define EEPROM_EECON_BWE_M              (1 << EEPROM_EECON_BWE_S)
#define EEPROM_EECON_IESERR_S           8
#define EEPROM_EECON_IESERR_M           (1 << EEPROM_EECON_IESERR_S)
//EESTA
#define EEPROM_EESTA_BSY_S              0
#define EEPROM_EESTA_BSY_M              (1 << EEPROM_EESTA_BSY_S)
#define EEPROM_EESTA_SERR_S             1
#define EEPROM_EESTA_SERR_M             (1 << EEPROM_EESTA_SERR_S)
//EERB
#define EEPROM_EERB_CORRECT_S           0
#define EEPROM_EERB_CORRECT_M           (0b111111 << EEPROM_EERB_CORRECT_S)
//EEADJ
#define EEPROM_EEADJ_OSCX2_S            0
#define EEPROM_EEADJ_OSCX2_M            (1 << EEPROM_EEADJ_OSCX2_S)
#define EEPROM_EEADJ_HIVCTL_S           0
#define EEPROM_EEADJ_HIVCTL_M           (0b111 << EEPROM_EEADJ_HIVCTL_S)
#define EEPROM_EEADJ_PLUSIMIX_S         0
#define EEPROM_EEADJ_PLUSIMIX_M         (1 << EEPROM_EEADJ_PLUSIMIX_S)
#define EEPROM_EEADJ_VBOOSTCTL_S        0
#define EEPROM_EEADJ_VBOOSTCTL_M        (0b11 << EEPROM_EEADJ_VBOOSTCTL_S)
#define EEPROM_EEADJ_MINUSIMIX_S        0
#define EEPROM_EEADJ_MINUSIMIX_M        (1 << EEPROM_EEADJ_MINUSIMIX_S)
#define EEPROM_EEADJ_DUMMYCTL_S         0
#define EEPROM_EEADJ_DUMMYCTL_M         (0b11 << EEPROM_EEADJ_DUMMYCTL_S)
#define EEPROM_EEADJ_VBGCTL_S           0
#define EEPROM_EEADJ_VBGCTL_M           (0b111 << EEPROM_EEADJ_VBGCTL_S)
#define EEPROM_EEADJ_REFCTL_S           0
#define EEPROM_EEADJ_REFCTL_M           (0b111 << EEPROM_EEADJ_REFCTL_S)
#define EEPROM_EEADJ_VCGCTL_S           0
#define EEPROM_EEADJ_VCGCTL_M           (0b111 << EEPROM_EEADJ_VCGCTL_S)
#define EEPROM_EEADJ_CGSTRICT_S         0
#define EEPROM_EEADJ_CGSTRICT_M         (1 << EEPROM_EEADJ_CGSTRICT_S)
#define EEPROM_EEADJ_VPPCTL_S           0
#define EEPROM_EEADJ_VPPCTL_M           (0b11 << EEPROM_EEADJ_VPPCTL_S)
#define EEPROM_EEADJ_STOPEE_S           0
#define EEPROM_EEADJ_STOPEE_M           (1 << EEPROM_EEADJ_STOPEE_S)
#define EEPROM_EEADJ_IDDQEN_S           0
#define EEPROM_EEADJ_IDDQEN_M           (1 << EEPROM_EEADJ_IDDQEN_S)
//NCYCRL
#define EEPROM_NCYCRL_N_LD_S            0
#define EEPROM_NCYCRL_N_LD_M            (0xFF << EEPROM_NCYCRL_N_LD_S)
#define EEPROM_NCYCRL_N_R_1_S           8
#define EEPROM_NCYCRL_N_R_1_M           (0xFF << EEPROM_NCYCRL_N_R_1_S)
#define EEPROM_NCYCRL_N_R_2_S           16
#define EEPROM_NCYCRL_N_R_2_M           (0xFF << EEPROM_NCYCRL_N_R_2_S)
//NCYCEP1
#define EEPROM_NCYCEP1_N_EP_1_S         0
#define EEPROM_NCYCEP1_N_EP_1_M         (0x3FFFF << EEPROM_NCYCEP1_N_EP_1_S)
//NCYCEP2
#define EEPROM_NCYCEP2_N_EP_2_S         0
#define EEPROM_NCYCEP2_N_EP_2_M         (0x7FF << EEPROM_NCYCEP2_N_EP_2_S)

//--------------------------
// EEPROM codes
//--------------------------

#define EEPROM_EECON_OP_RD      0
#define EEPROM_EECON_OP_ER      1
#define EEPROM_EECON_OP_PR      2
#define EEPROM_WRBEH_SINGLE     0
#define EEPROM_WRBEH_EVEN       1
#define EEPROM_WRBEH_ODD        2
#define EEPROM_WRBEH_GLOB       3


#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
        volatile uint32_t EEDAT;
        volatile uint32_t EEA;
        volatile uint32_t EECON;
        volatile uint32_t EESTA;
        volatile uint32_t EERB;
        volatile uint32_t EEADJ;
        volatile uint32_t NCYCRL;
        volatile uint32_t NCYCEP1;
        volatile uint32_t NCYCEP2;

    } EEPROM_REGS_TypeDef;
#endif


#endif // EEPROM_H_INCLUDED

