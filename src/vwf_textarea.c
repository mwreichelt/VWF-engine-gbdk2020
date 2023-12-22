#include <stdlib.h>
#include "vwf_common.h"
#include "vwf_textarea.h"

#pragma bank 0

#if defined(NINTENDO)

//For knowing how quickly to render characters
uint8_t vwf_textarea_characters_per_tick;
uint8_t vwf_textarea_animationticks_per_vwf_tick;
uint8_t vwf_textarea_vwf_tick_count;
uint8_t vwf_textarea_textfill_paused;
uint8_t vwf_textarea_enabled;

uint8_t vwf_textarea_current_rotate;
uint8_t vwf_textarea_inverse_map;
uint8_t vwf_textarea_tile_data[DEVICE_TILE_SIZE * 2];

//For knowing the size of our textarea
uint8_t vwf_textarea_x;
uint8_t vwf_textarea_y;
uint8_t vwf_textarea_w;
uint8_t vwf_textarea_h;

//For knowing the current tile in vram
uint8_t vwf_textarea_start_tile;
uint8_t vwf_textarea_ending_tile;
uint8_t vwf_textarea_current_tile;
uint8_t * vwf_textarea_render_base_address = VWF_DEFAULT_BASE_ADDRESS;
uint8_t vwf_textarea_current_line;

//For knowing the current width index of our vram tile
uint8_t vwf_textarea_vram_width;
uint8_t vwf_textarea_vram_count;

//This variable is a mask that helps us only affect the 2bpp data in the current vram title
// that we would like to overwrite.
uint8_t vwf_textarea_current_mask;

//This offset represents how many pixels to the right of the current vram address we are at.
// e.g. If the previous character had three columns of pixels in the current vram tile then
// this variable would be equal to 4.
uint8_t vwf_textarea_current_offset = 0;

//For knowing what we are in the middle of rendering
vwf_text_segment_t * vwf_textarea_current_segment;
uint8_t vwf_textarea_current_segment_bank;

//For tracking where in the current segment's string we are for rendering
uint16_t vwf_textarea_current_character_index;
uint8_t vwf_textarea_next_character;

//For tracking the current font to use in the textarea and what bank that font is in
font_desc_t vwf_textarea_current_font_desc;
uint8_t vwf_textarea_current_font_bank;

//For tracking where in VRAM we're writing bytes
uint8_t * vwf_textarea_screen_dest_ptr;

void vwf_textarea_print_reset(uint8_t tile) {
vwf_textarea_current_tile = tile;
vwf_textarea_current_offset = 0;
vwf_textarea_swap_tiles();
vwf_textarea_swap_tiles();
}

void vwf_initialize_textarea(uint8_t xTile, uint8_t yTile, uint8_t width, uint8_t height, uint8_t vram_start_index) {
    vwf_textarea_x = xTile;
    vwf_textarea_y = yTile;
    vwf_textarea_w = width;
    vwf_textarea_h = height;
    vwf_textarea_current_line = 0;
    vwf_textarea_current_tile = vram_start_index;
    vwf_textarea_vram_count = width * height;
    //TODO: add some sort of check to make sure that start index isn't too high?
    vwf_textarea_ending_tile = vwf_textarea_vram_count + vram_start_index;
    vwf_textarea_vram_width = 0u;
    vwf_textarea_current_character_index = 0u;

    // Init the paused flag.
    vwf_textarea_textfill_paused = FALSE;

    //Initialize the vram tiles
    vwf_textarea_print_reset(vram_start_index);

    //TODO: I need to do something about the x and y variables here. Where do I get these from? Do I even need these in the textarea function?
    //This pointer is for tracking the place on the background/window map to render a character to
    vwf_textarea_screen_dest_ptr = vwf_textarea_render_base_address + (yTile + DEVICE_SCREEN_Y_OFFSET) * (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE) + ((xTile + DEVICE_SCREEN_X_OFFSET) * DEVICE_SCREEN_MAP_ENTRY_SIZE);

    //TODO: Assign the bg tiles to the vram indexes
    // Not sure I actually need to do this since it's taken care of during the vblank update function
}

void vwf_textarea_set_text_segment(vwf_text_segment_t * first_text_segment_ptr, uint8_t text_segment_bank) {
    vwf_textarea_current_segment = first_text_segment_ptr;
    vwf_textarea_current_segment_bank = text_segment_bank;
}

void vwf_textarea_set_text_speed(uint8_t characters_per_tick, uint8_t animationticks_per_character_tick) {
    vwf_textarea_characters_per_tick = characters_per_tick;
    vwf_textarea_animationticks_per_vwf_tick = animationticks_per_character_tick;
    vwf_textarea_vwf_tick_count = 0u;
}

uint8_t vwf_textarea_render_char(uint8_t character) {
    //Translate the input parameter to the font glyph offset
    uint8_t letter = vwf_read_banked_ubyte(vwf_textarea_current_font_desc.recode_table + (character & ((vwf_textarea_current_font_desc.attr & RECODE_7BIT) ? 0x7fu : 0xffu)), vwf_textarea_current_font_bank);

    //A pointer to the 2bpp data for the selected glyph.
    const uint8_t * bitmap = vwf_textarea_current_font_desc.bitmaps + (uint16_t)letter * 8;

    //Get the width for the glyph we're going to render.
    uint8_t width = vwf_read_banked_ubyte(vwf_textarea_current_font_desc.widths + letter, vwf_textarea_current_font_bank);

    uint8_t dx = (8u - vwf_textarea_current_offset);

    //This operation creates a bitmask that will allow us to only update the pixels in the current vram tile
    // that were not written by the previous render.
    // First the a full mask is shifted left by the number of columns that we are offsetting,
    // then the mask is shifted right by the offset plus the width of the glyph to write.
    vwf_textarea_current_mask = (0xffu << dx) | (0xffu >> (vwf_textarea_current_offset + width));

    vwf_textarea_current_rotate = vwf_textarea_current_offset;
    vwf_textarea_print_shift_char(vwf_textarea_tile_data, bitmap, vwf_textarea_current_font_bank);
    if ((uint8_t)(vwf_textarea_current_offset + width) > 8u) {
        vwf_textarea_current_rotate = dx | 0x80u;
        vwf_textarea_current_mask = 0xffu >> (width - dx);
        vwf_textarea_print_shift_char(vwf_textarea_tile_data + DEVICE_TILE_SIZE, bitmap, vwf_textarea_current_font_bank);
    }
    vwf_textarea_current_offset += width;

    //If we've gone past the end of our current tile,
    if (vwf_textarea_current_offset > 7u) {
        //Adjust the offset
        vwf_textarea_current_offset -= 8u;

        //Set the completed tile data
        set_bkg_1bpp_data(vwf_textarea_current_tile, (vwf_textarea_current_offset) ? 2 : 1, vwf_textarea_tile_data);

        //Go to the next tile and initialize it
        vwf_textarea_current_tile++;
        vwf_textarea_swap_tiles();

        //Return, signalling that we've crossed over into a new tile
        return TRUE;
    };
    set_bkg_1bpp_data(vwf_textarea_current_tile, 1, vwf_textarea_tile_data);
    return FALSE;
}

void vwf_textarea_vblank_update() NONBANKED {
    uint8_t save_bank = _current_bank;

    //TODO: Determine if we need to draw a new character
    if(vwf_textarea_enabled == TRUE) {
        // Check if the textfill is paused
        if (vwf_textarea_textfill_paused == FALSE) {

            //Swap to the correct bank
            //TODO: Process characters in the current segment until we have reached a renderable character
            SWITCH_ROM(vwf_textarea_current_segment_bank);

            //Get the next character in the text segment
            vwf_textarea_next_character = vwf_read_banked_ubyte(
                vwf_textarea_current_segment->text + vwf_textarea_current_character_index,
                vwf_textarea_current_segment_bank
            );

            //Render the new character
            switch (vwf_textarea_next_character) {
                //TODO: Add support for control codes
                case '\n':
                    //Increment the current line that we're on
                    vwf_textarea_current_line += 1;

                    //Recalculate the screen tile that we need to point to
                    vwf_textarea_screen_dest_ptr = vwf_textarea_render_base_address + ((vwf_textarea_y + vwf_textarea_current_line) + DEVICE_SCREEN_Y_OFFSET) * (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE) + ((vwf_textarea_x + DEVICE_SCREEN_X_OFFSET) * DEVICE_SCREEN_MAP_ENTRY_SIZE);

                    //If our offset is not 0, we need to reset the current tile and offset for a new line.
                    if (vwf_textarea_current_offset > 1 && vwf_textarea_current_offset < 8) {
                        vwf_textarea_print_reset(vwf_textarea_current_tile + 1);
                    }

                    //Consume the character so we can move along.
                    vwf_textarea_current_character_index += 1;
                    break;
                case '\0':
                    //If we hit the end of a text segment, pause the textfill until the user hits the button to advance.
                    vwf_textarea_textfill_paused = TRUE;
                    break;
                default:
                //Render the character and return true iff we cross into a new tile in vram
                if (vwf_textarea_render_char(vwf_textarea_next_character)) {
                    //We're in a new tile now, so we need to set the previous tile's address in the tile map so
                    // it appears on the screen
                    set_vram_byte(vwf_textarea_screen_dest_ptr, vwf_textarea_current_tile - 1);

                    //Then increment our vwf_textarea_screen_dest_ptr
                    vwf_textarea_screen_dest_ptr += DEVICE_SCREEN_MAP_ENTRY_SIZE;
                }

                vwf_textarea_current_character_index += 1;

                //Update the current vram tile data
                if (vwf_textarea_current_offset) {
                    set_vram_byte(vwf_textarea_screen_dest_ptr, vwf_textarea_current_tile);
                }
                break;
            }

            //TODO: Do we need to render more characters during this vblank?
        } else {
            //If we are in here, we are at a rest point and the user needs to hit the A button to unpause us
            //TODO: Do we need to change any sprites for the text box animation? Like a "Hit the button!" animation.
            //TODO: If we are paused AND the next character is \0 (i.e. we are at the end of the text segment) then
            //  spool the next text segment
            //TODO: Do we want to add an animation that has to complete before we go to the next text segment? Like of
            // the text all scrolling up out of frame?
        }
    }


    //TODO: Swap back to the original bank
    SWITCH_ROM(save_bank);
}

void vwf_textarea_activate_font(uint8_t index) {
    vwf_textarea_current_font_bank = vwf_fonts[index].bank;
    vwf_memcpy(&vwf_textarea_current_font_desc, vwf_fonts[index].ptr, sizeof(font_desc_t), vwf_textarea_current_font_bank);
}

#endif