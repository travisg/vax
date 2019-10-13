.text

# output a character using the psr method
.macro outchar, c
#    mtpr $\c,$35
.endm

.globl _start
_start:
    nop
    nop

    // save our bootargs
    moval   bootargs_end,%sp
    pushr   $0x1fff

    outchar 'a'

    // see if we need to be relocated
    movab   _start, %r0     // where we are
    movl    $_start, %r1     // where we want to be
    cmpl    %r0,%r1
    beql    relocated

    // compute the copy length
    subl3   %r1, $_edata, %r2

    // copy us down to the final location
1:
    movb    (%r0)+,(%r1)+
    sobgtr  %r2,1b

    // zero bss
    subl3   $_end, $_edata, %r2
1:
    movb    $0,(%r1)+
    sobgtr  %r2,1b

    // branch to our new spot
    movl    $relocated, %r0
    jmp     (%r0)

relocated:
    // set the stack to the end of us
    moval    stack_end,%sp

    outchar 'b'

    // branch into main and we're done
    calls   $0, _main
    halt

.section .data
.align 4
.globl bootargs
bootargs:
    .skip 13*4
bootargs_end:

.section .bss
.align 4
stack:
    .skip 1024
stack_end:
