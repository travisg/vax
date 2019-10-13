#include "mtpr.h"

extern unsigned int bootargs[13];

void putchar_mtfr(int c) {
    // wait until ready
    while ((mfpr(PR_TXCS) & 0x80) == 0)
        ;

    // output char
    mtpr((char)c, PR_TXDB);
}

// todo: move to .S file
unsigned int rom_putchar_addr = 0x20040068;
asm volatile("\
    .globl rom_putchar;\
    .type rom_putchar@function;\
    rom_putchar:; \
      .word 0x0004;\
      movl  4(%ap), %r2;\
      jsb   *rom_putchar_addr;\
      ret\
");
extern "C" int rom_putchar(int c);

// select the above routine
int (*putchar_func)(int c) = &rom_putchar;

int putchar(int c) {
    char ch = c;
    if (ch == '\n') {
        putchar_func('\r');
    }

    putchar_func(ch);
    return ch;
}

void putstr(const char *str) {
    while (*str) {
        putchar(*str++);
    }
}

int puts(const char *str) {
    putstr(str);
    putchar('\n');
    return 0;
}

template <typename T>
void puthex(T val) {
    static const char hexarray[] = "0123456789abcdef";

    putchar('0');
    putchar('x');

    for (int i = 0; i < sizeof(val) * 2; i++) {
        putchar(hexarray[(val & 0xf0000000) >> 28]);
        val <<= 4;
    }
}

extern "C"
void _main(void) {
    putchar('c');

    puts("hello world");

    putstr("SID ");
    puthex(mfpr(PR_SID));
    putchar('\n');

    putstr("SIE ");
    puthex(*(unsigned int *)0x20040004); // SIE address on a lot of machines
    putchar('\n');

    for (int i = 0; i < 13; i++) {
        puthex(bootargs[i]);
        putchar('\n');
    }


    for (;;) {
        asm volatile("halt");
    }
}

