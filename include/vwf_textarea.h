#include "vwf_common.h"
#include <stdlib.h>

#ifndef _VWF_TEXTAREA
#define _VWF_TEXTAREA

// Defines a segment of text to display and a pointer to the next one.
typedef struct vwf_text_segment_t {
    const struct vwf_text_segment_t * next;
    const char * text;
} vwf_text_segment_t;

extern uint8_t vwf_textarea_enabled;
extern uint8_t vwf_textarea_tile_data[DEVICE_TILE_SIZE * 2];
extern uint8_t vwf_textarea_inverse_map;
extern uint8_t vwf_textarea_current_rotate;
extern uint8_t vwf_textarea_default_tile;

//For knowing how quickly to render characters
extern uint8_t vwf_textarea_characters_per_tick;
extern uint8_t vwf_textarea_animationticks_per_vwf_tick;
extern uint8_t vwf_textarea_vwf_tick_count;
extern uint8_t vwf_textarea_textfill_paused;

//For knowing the size of our textarea
extern uint8_t vwf_textarea_x;
extern uint8_t vwf_textarea_y;
extern uint8_t vwf_textarea_w;
extern uint8_t vwf_textarea_h;

//For knowing the current tile in vram
extern uint8_t vwf_textarea_start_tile;
extern uint8_t vwf_textarea_ending_tile;
extern uint8_t vwf_textarea_current_tile;
extern uint8_t * vwf_textarea_render_base_address;
extern uint8_t vwf_textarea_current_line;

//For knowing the current width index of our vram tile
extern uint8_t vwf_textarea_vram_width;
extern uint8_t vwf_textarea_vram_count;

//This variable is a mask that helps us only affect the 2bpp data in the current vram title
// that we would like to overwrite.
extern uint8_t vwf_textarea_current_mask;

//This offset represents how many pixels to the right of the current vram address we are at.
// e.g. If the previous character had three columns of pixels in the current vram tile then
// this variable would be equal to 4.
extern uint8_t vwf_textarea_current_offset;

//For knowing what we are in the middle of rendering
extern vwf_text_segment_t * vwf_textarea_current_segment;
extern uint8_t vwf_textarea_current_segment_bank;

//For tracking where in the current segment's string we are for rendering
extern uint16_t vwf_textarea_current_character_index;
extern uint8_t vwf_textarea_next_character;

//For tracking the current font to use in the textarea and what bank that font is in
extern font_desc_t vwf_textarea_current_font_desc;
extern uint8_t vwf_textarea_current_font_bank;

//For tracking where in VRAM we're writing bytes
extern uint8_t * vwf_textarea_screen_dest_ptr;

//Textarea function definitions
void vwf_textarea_print_reset(uint8_t tile);
void vwf_initialize_textarea(uint8_t xTile, uint8_t yTile, uint8_t width, uint8_t height, uint8_t vram_start_index, uint8_t vram_default_tile);
void vwf_textarea_set_text_segment(vwf_text_segment_t * first_text_segment_ptr, uint8_t text_segment_bank);
void vwf_textarea_set_text_speed(uint8_t characters_per_tick, uint8_t animationticks_per_character_tick);
uint8_t vwf_textarea_render_char(uint8_t character);
void vwf_textarea_vblank_update() NONBANKED;
void vwf_textarea_activate_font(uint8_t index);
void vwf_textarea_swap_tiles() OLDCALL;
void vwf_textarea_print_shift_char(void * dest, const void * src, uint8_t bank) OLDCALL;
uint8_t vwf_word_length(char * text_ptr) NONBANKED;

#endif