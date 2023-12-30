#ifndef _VWF_H_INCLUDE
#define _VWF_H_INCLUDE
#include "vwf_common.h"

#define __VWF_BANK_PREFIX(A) __bank_##A
#define TO_VWF_FARPTR(A) {.bank = (char)&(__VWF_BANK_PREFIX(A)), .ptr = (void *)&(A)}

#define RECODE_7BIT 1
#define FONT_VWF 2
#define VWF_DEFAULT_BASE_ADDRESS 0x9800
#define DEVICE_TILE_SIZE 8u

void vwf_print_shift_char(void * dest, const void * src, uint8_t bank) OLDCALL;
void vwf_swap_tiles() OLDCALL;

typedef enum {
    VWF_RENDER_BKG,
    VWF_RENDER_WIN
} vwf_reder_dest_e;

void vwf_set_destination(vwf_reder_dest_e destination);
void vwf_load_font(uint8_t idx, const void * font, uint8_t bank);
void vwf_activate_font(uint8_t idx);
uint8_t vwf_draw_text(uint8_t x, uint8_t y, uint8_t base_tile, const unsigned char * str);
uint8_t vwf_next_tile(void);

inline void vwf_set_colors(uint8_t fgcolor, uint8_t bgcolor) {
    set_1bpp_colors(fgcolor, bgcolor);
}
#endif