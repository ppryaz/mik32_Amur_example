
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)

ENTRY(_start)


MEMORY {
  rom (RX): ORIGIN = 0x80000000, LENGTH = 8M
  ram (RWX): ORIGIN = 0x02000000, LENGTH = 16K
}

STACK_SIZE = 1024;
HEAP_SIZE = 1024*4;

CL_SIZE = 16;

SECTIONS {
    .text ORIGIN(rom) : {
        PROVIDE(__TEXT_START__ = .);
        *crt0.o(.text .text.*)
        *(.text.smallsysteminit)
        *(.text.SmallSystemInit)
        . = ORIGIN(rom) + 0xC0;
        KEEP(*crt0.o(.trap_text))

        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)      
        . = ALIGN(CL_SIZE);
        PROVIDE(__TEXT_END__ = .);
    } >rom 

    .data : 
    AT( __TEXT_END__ ) {
        PROVIDE(__DATA_START__ = .);
        _gp = .;
        *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.data .data.*)
        . = ALIGN(CL_SIZE);
    } >ram
    
    __DATA_IMAGE_START__ = LOADADDR(.data);
    __DATA_IMAGE_END__ = LOADADDR(.data) + SIZEOF(.data);
    ASSERT(__DATA_IMAGE_END__ < ORIGIN(rom) + LENGTH(rom), "Data image overflows rom section")

    /* thread-local data segment */
    .tdata : {
        PROVIDE(_tls_data = .);
        PROVIDE(_tdata_begin = .);
        *(.tdata .tdata.*)
        PROVIDE(_tdata_end = .);
        . = ALIGN(CL_SIZE);
    } >ram

    .tbss : {
        PROVIDE(__BSS_START__ = .);
        *(.tbss .tbss.*)
        . = ALIGN(CL_SIZE);
        PROVIDE(_tbss_end = .);
    } >ram

    /* bss segment */
    .sbss : {
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.scommon)
    } >ram

    .bss : {
        *(.bss .bss.*)
        . = ALIGN(CL_SIZE);
        PROVIDE(__BSS_END__ = .);
    } >ram

    .my_heap : {
        PROVIDE(__heap_start = .);
        . += HEAP_SIZE;
        . = ALIGN(CL_SIZE);
        PROVIDE(__heap_end = .);
    } >ram

    _end = .;
    PROVIDE(__end = .);

    /* End of uninitalized data segement */

    .stack ORIGIN(ram) + LENGTH(ram) - STACK_SIZE : {
        FILL(0);
        PROVIDE(__STACK_START__ = .);
        . += STACK_SIZE;
        PROVIDE(__C_STACK_TOP__ = .);
        PROVIDE(__STACK_END__ = .);
    } >ram

    /DISCARD/ : {
        *(.eh_frame .eh_frame.*)
    }
}
