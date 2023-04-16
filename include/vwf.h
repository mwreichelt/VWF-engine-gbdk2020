#ifndef _VWF_H_INCLUDE
#define _VWF_H_INCLUDE

#include <gbdk/platform.h>
#include <stdint.h>

#define __VWF_BANK_PREFIX(A) __bank_##A
#define TO_VWF_FARPTR(A) {.bank = (char)&(__VWF_BANK_PREFIX(A)), .ptr = (void *)&(A)}

#define RECODE_7BIT 1
#define FONT_VWF 2

#define VWF_MODE_RENDER 0
#define VWF_MODE_PRINT 1

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

typedef enum {
    VWF_RENDER_BKG,
    VWF_RENDER_WIN
} vwf_reder_dest_e;

extern vwf_farptr_t vwf_fonts[4];
extern uint8_t vwf_mode;

void vwf_set_destination(vwf_reder_dest_e destination);
void vwf_load_font(uint8_t idx, const void * font, uint8_t bank);
void vwf_activate_font(uint8_t idx);
uint8_t vwf_draw_text(uint8_t x, uint8_t y, uint8_t base_tile, const unsigned char * str);
uint8_t vwf_next_tile();

inline void vwf_set_mode(uint8_t mode) {
    vwf_mode = mode;
}
inline void vwf_set_colors(uint8_t fgcolor, uint8_t bgcolor) {
    set_1bpp_colors(fgcolor, bgcolor);
}

//region User controlled text area

// Defines a segment of text to display and a pointer to the next one.
typedef struct vwf_text_segment_t {
    vwf_farptr_t nextSegment;
    const char * text;
} vwf_text_segment_t;

//For knowing how quickly to render characters
extern uint8_t vwf_textarea_characters_per_tick;
extern uint8_t vwf_textarea_animationticks_per_character;

//For knowing the size of our textarea
extern uint8_t vwf_textarea_x;
extern uint8_t vwf_textarea_y;
extern uint8_t vwf_textarea_w;
extern uint8_t vwf_textarea_h;

//For knowing the current tile in vram
extern uint8_t vwf_textarea_tile;

//For knowing the current width index of our vram tile
extern uint8_t vwf_textarea_width;

//For knowing what we are in the middle of rendering
extern vwf_text_segment_t * vwf_textarea_current_segment;
extern uint8_t vwf_textarea_current_segment_bank;

//For tracking where in the current segment's string we are for rendering
extern uint16_t vwf_textarea_current_character_index;

//Initializes the user-defined textarea
void vwf_initialize_textarea(uint8_t xTile, uint8_t yTile, uint8_t width, uint8_t height, uint8_t vram_start_index);

//For updating the text area's rendering during the vblank interrupt
void vwf_textarea_vblank_update();

//endregion

#endif