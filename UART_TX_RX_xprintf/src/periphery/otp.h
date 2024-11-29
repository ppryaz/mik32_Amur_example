#ifndef OTP_H_INCLUDED
#define OTP_H_INCLUDED

//--------------------------
// OTP register fields
//--------------------------


#define OTP_APBNWS_S                        0
#define OTP_BSY_S                           0
#define OTP_N_RA_S                          8
#define OTP_N_RSU_S							0
#define OTP_N_RH_S                          16
#define OTP_SEL_UPP_READ                    24
#define OTP_SEL_READ_CUR                    27


/* Регистр управления OTPCON */
#define OTP_OTPCON_APBNWS_S                        0
#define OTP_OTPCON_APBNWS_M                        (0b1 << OTP_OTPCON_APBNWS_S)
#define OTP_OTPCON_MAN_WE_I_S                      1
#define OTP_OTPCON_MAN_WE_I_M                      (0b1 << OTP_OTPCON_MAN_WE_I_S)
#define OTP_OTPCON_MAN_RE_I_S                      2
#define OTP_OTPCON_MAN_RE_I_M                      (0b1 << OTP_OTPCON_MAN_RE_I_S)
#define OTP_OTPCON_MME_S                           3
#define OTP_OTPCON_MME_M                           (0b1 << OTP_OTPCON_MME_S)

/* Регистр статуса OTPSTA */
#define OTP_OTPSTA_BSY_S                           0
#define OTP_OTPSTA_BSY_M                           (0b1 << OTP_OTPSTA_BSY_S)

/* Регистр бит дешифратора OTPDEC */
#define OTP_OTPDEC_DECO_S                          0
#define OTP_OTPDEC_DECO_M                          (0x3FF << OTP_OTPDEC_DECO_S)

/* Регистр управления временными параметрами OTPADJ */
#define OTP_OTPADJ_N_RSU_S							0
#define OTP_OTPADJ_N_RSU_M							(0b111 << OTP_OTPADJ_N_RSU_S)
#define OTP_OTPADJ_N_RA_S                          8
#define OTP_OTPADJ_N_RA_M                          (0b111 << OTP_OTPADJ_N_RA_S)
#define OTP_OTPADJ_N_RH_S                          16
#define OTP_OTPADJ_N_RH_M                          (0b111 << OTP_OTPADJ_N_RH_S)
#define OTP_OTPADJ_SEL_UPP_READ_I_S                24
#define OTP_OTPADJ_SEL_UPP_READ_I_M                (0b111 << OTP_OTPADJ_SEL_UPP_READ_I_S)
#define OTP_OTPADJ_SEL_UPP_READ_I_2_0V_M           (0b000 << OTP_OTPADJ_SEL_UPP_READ_I_S)
#define OTP_OTPADJ_SEL_UPP_READ_I_2_5V_M           (0b001 << OTP_OTPADJ_SEL_UPP_READ_I_S)
#define OTP_OTPADJ_SEL_UPP_READ_I_3_0V_M           (0b011 << OTP_OTPADJ_SEL_UPP_READ_I_S)
#define OTP_OTPADJ_SEL_UPP_READ_I_VDD18_M          (0b010 << OTP_OTPADJ_SEL_UPP_READ_I_S)
#define OTP_OTPADJ_SEL_UPP_READ_I_VDD5_M           (0b110 << OTP_OTPADJ_SEL_UPP_READ_I_S)
#define OTP_OTPADJ_SEL_READ_CUR_I_S                27
#define OTP_OTPADJ_SEL_READ_CUR_I_M                (0b1 << OTP_OTPADJ_SEL_READ_CUR_I_S)
#define OTP_OTPADJ_POWER_OFF_I_S                   28
#define OTP_OTPADJ_POWER_OFF_I_M                   (0b1 << OTP_OTPADJ_POWER_OFF_I_S)

/* Регистр подстройки длительности процедуры записи 1 OTPWT1 */
#define OTP_OTPWT1_N_SU_S                          0
#define OTP_OTPWT1_N_SU_M                          (0b111 << OTP_OTPWT1_N_SU_S)
#define OTP_OTPWT1_N_H_S                           8
#define OTP_OTPWT1_N_H_M                           (0b111 << OTP_OTPWT1_N_H_S)

/* Регистр подстройки длительности процедуры записи 2 OTPWT2 */
#define OTP_OTPWT2_N_W_S                           0
#define OTP_OTPWT2_N_W_M                           (0xFFFFFF << OTP_OTPWT2_N_W_S)


#ifndef _ASSEMBLER_
    #include <inttypes.h>

    typedef struct
    {
    //--------------------------
    // OTP registers
    //--------------------------

    volatile uint32_t OTPDAT;
    volatile uint32_t OTPA;
    volatile uint32_t OTPCON;
    volatile uint32_t OTPSTA;
    volatile uint32_t OTPDEC;
    volatile uint32_t OTPADJ;
    volatile uint32_t OTPWT1;
    volatile uint32_t OTPWT2;
    

    } OTP_TypeDef;
#endif


#endif // OTP_H_INCLUDED

