#include "vwf_common.h"

vwf_farptr_t vwf_fonts[4];

void vwf_load_font(uint8_t idx, const void * font, uint8_t bank) {
    vwf_fonts[idx].bank = bank;
    vwf_fonts[idx].ptr = (void *)font;
//    vwf_activate_font(idx);
}