#ifndef VWF_COMMON
#define VWF_COMMON

#include <gbdk/platform.h>
#include <stdint.h>

#define __VWF_BANK_PREFIX(A) __bank_##A
#define TO_VWF_FARPTR(A) {.bank = (char)&(__VWF_BANK_PREFIX(A)), .ptr = (void *)&(A)}

extern void vwf_memcpy(void* to, const void* from, size_t n, uint8_t bank) OLDCALL;
extern uint8_t vwf_read_banked_ubyte(const void * src, uint8_t bank) OLDCALL __preserves_regs(b, c) ;
extern uint8_t * vwf_get_win_addr() OLDCALL __preserves_regs(b, c, h, l) ;
extern uint8_t * vwf_get_bkg_addr() OLDCALL __preserves_regs(b, c, h, l) ;
extern void vwf_set_banked_data(uint8_t i, uint8_t l, const unsigned char* ptr, uint8_t bank) OLDCALL;

#define RECODE_7BIT 1
#define FONT_VWF 2
#define VWF_DEFAULT_BASE_ADDRESS 0x9800
#define DEVICE_TILE_SIZE 8u

typedef struct vwf_farptr_t {
    UINT8 bank;
    void * ptr;
} vwf_farptr_t;

typedef struct font_desc_t {
    uint8_t attr;
    const uint8_t * recode_table;
    const uint8_t * widths;
    const uint8_t * bitmaps;
} font_desc_t;

extern vwf_farptr_t vwf_fonts[4];

#endif