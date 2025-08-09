#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/syscall.h"
#include <stdint.h>

uint32_t syscall_ret = 0; /* referenced from assembly stub */

/* handler signature: regparm(3) -> args passed in EAX, EDX, ECX */
uint32_t syscall_handler_regparm(uint32_t num, uint32_t arg1, uint32_t arg2)
    __attribute__((regparm(3)));

uint32_t syscall_handler_regparm(uint32_t num, uint32_t arg1, uint32_t arg2) {
    switch (num) {
        case SYS_WRITE: {
            /* arg1 = pointer to buffer, arg2 = length */
            const char *buf = (const char *)arg1;
            uint32_t len = arg2;
            /* naive write to VGA buffer */
            volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
            static uint32_t cursor = 80*8; /* start somewhere; static preserves across calls */
            for (uint32_t i = 0; i < len; ++i) {
                char c = buf[i];
                if (!c) break;
                if (c == '\n') {
                    /* move to next line */
                    cursor = ((cursor/80)+1) * 80;
                } else {
                    vga[cursor++] = (uint16_t)c | (0x07 << 8);
                }
                if (cursor >= 80*25) cursor = 80*24;
            }
            return len;
        }
        case SYS_GET_TICKS: {
            return get_ticks();
        }
        default:
            return (uint32_t)-1;
    }
}
