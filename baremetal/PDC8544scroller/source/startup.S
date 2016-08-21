.section ".text.startup"   // from linkerScript.rpi
.global _start

/*
https://geekwentfreak-raviteja.rhcloud.com/blog/2011/01/16/writing-interrupt-routines-using-gcc-for-arm/
http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0203j/CHDEFDJG.html
- 6.2.2. The vector table, ARMv6, ARMv7-A, ARMv7-R
*/
_start:
    ldr    pc, RST_Addr
    ldr    pc, UNDEF_Addr
    ldr    pc, SWI_Addr
    ldr    pc, PREFETCH_Addr
    ldr    pc, DATA_A_Addr
    nop  // or jump to start2
    ldr    pc, IRQ_Addr
    ldr    pc, FIQ_Addr

RST_Addr:      .word  _start2
UNDEF_Addr:    .word  undef_handler
SWI_Addr:      .word  swi_handler
PREFETCH_Addr: .word  prefetch_handler
DATA_A_Addr:   .word  dataAbort_handler
IRQ_Addr:      .word  irq_handler
FIQ_Addr:      .word  fiq_handler  // or: do FIQ handler here.

_start2:
// copy from 0x8000 16 x 32bit vectors to 0x0000-0x0020
    ldr    r0, =_start  // source
    ldr    r1, =0x0000  // target, where vectors are at
    ldmia  r0!, {r2-r12} // 1st 11 words  
    stmia  r1!, {r2-r12} // 
    ldmia  r0!, {r2-r6}  // remaining 5 words
    stmia  r1!, {r2-r6}  // load instructions and addresses copied
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471h/Beiijgbc.html
// Stack pointer initialization
    ldr    r0, =0x6000000   // FIQ
    msr    CPSR_c, #0xD1      // Mode_FIQ | I_Bit | F_Bit / 0x11|0x80|0x40
    mov    SP, r0
    sub    r0, #0x10000     // IRQ
    msr    CPSR_c, #0xD2      // Mode_IRQ | I_Bit | F_Bit / 0x12|0x80|0x40
    mov    SP, r0
    sub    r0, #0x10000     // Supervisor mode
    msr    CPSR_c, #0xD3      // Mode_SVC | I_Bit | F_Bit / 0x13|0x80|0x40
    mov    SP, r0  
    b    kernel_main     
_should_main_return:
    b    _should_main_return

_initialize_bss:  // optional to C implementation which is called currently
    ldr    r0, =__bss_start__
    ldr    r1, =__bss_end__
    cmp    r0, r1
    beq    exit_initialize_bss   // nothing to initialise, exit
    mov    r2, #0
loop:
    str    r2, [r0]   
    add    r0, #4    // 4 width is word
    cmp    r0, r1  
    bne    loop
exit_initialize_bss:
    mov    pc, lr

/*  stack plan  
0x6000000 FIQ  64k
0x5FF0000 IRQ  64k
0x5FE0000 Supervisor  64k
0x5FD0000   */
