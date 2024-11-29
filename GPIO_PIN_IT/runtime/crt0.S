//
#define EXCEPTION_STACK_SPACE 32*4
#define EXCEPTION_SAVED_REGISTERS 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31

.globl _start, main
.weak SmallSystemInit, SystemInit

.globl trap_entry
.globl trap_handler
.globl raw_trap_handler
.weak trap_handler, raw_trap_handler

.altmacro
.macro memcpy src_beg, src_end, dst, tmp_reg
    LOCAL memcpy_1, memcpy_2
    j    memcpy_2
memcpy_1:
    lw   \tmp_reg, (\src_beg)
    sw   \tmp_reg, (\dst)
    add  \src_beg, \src_beg, 4
    add  \dst, \dst, 4
memcpy_2:
    bltu \src_beg, \src_end, memcpy_1
.endm

.macro memset dst_beg, dst_end, val_reg
    LOCAL memset_1, memset_2
    j    memset_2
memset_1:
    sw   \val_reg, (\dst_beg)
    add  \dst_beg, \dst_beg, 4
memset_2:
    bltu \dst_beg, \dst_end, memset_1
.endm


# la uses PC relative addressing (auipc instruction)
# Explicit absolut addressing with lui instruction is used 
# to allow startup code to be executed from any PC address
# (la instruction equivalents are left in comments)
    
.macro la_abs reg, address
    lui \reg, %hi(\address)
    addi \reg, \reg, %lo(\address)
.endm

.macro jalr_abs return_reg, address
    lui \return_reg, %hi(\address)
    jalr \return_reg, %lo(\address)(\return_reg)
.endm


.text

_start:
    
    # Init stack and global pointer
    #
    la_abs  sp, __C_STACK_TOP__
    la_abs  gp, _gp
    
    # Init data
    #
    la_abs  a1, __DATA_IMAGE_START__
    la_abs  a2, __DATA_IMAGE_END__
    la_abs  a3, __DATA_START__
    memcpy  a1, a2, a3, t0
    
    # Clear bss
    #
    la_abs  a1, __BSS_START__
    la_abs  a2, __BSS_END__
    memset a1, a2, zero
    
    jalr_abs ra, SmallSystemInit
    jalr_abs ra, SystemInit
    jalr_abs ra, main
1:  wfi
    j 1b

    
// Actions before main: none by default
// (weak symbol here - may be redefined)
SmallSystemInit:
SystemInit:
    ret

.section .trap_text, "ax"
// .org should be consistent with
// default mtvec value (set in scr1_arch_description.svh)
//.org 0xC0
trap_entry:
    j raw_trap_handler

raw_trap_handler:
    // Save registers
    addi    sp, sp, -(EXCEPTION_STACK_SPACE)
    .irp index, EXCEPTION_SAVED_REGISTERS
        sw      x\index, 4*index(sp)
    .endr
    
    // Call handler 
    la      ra, trap_handler
    jalr    ra
    
    // restore registers
    .irp index, EXCEPTION_SAVED_REGISTERS
        lw      x\index, 4*index(sp)
    .endr
    addi    sp, sp, EXCEPTION_STACK_SPACE
    mret

// Default handler: infinit loop
// (weak symbol here - may be redefined)
trap_handler:
1:  j       1b
