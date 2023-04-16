#include "vwf.h"

#define VWF_DEFAULT_BASE_ADDRESS 0x9800
#define DEVICE_TILE_SIZE 8u

vwf_farptr_t vwf_fonts[4];

uint8_t vwf_current_offset = 0;
uint8_t vwf_tile_data[DEVICE_TILE_SIZE * 2];
uint8_t vwf_current_mask;
uint8_t vwf_current_rotate;
uint8_t vwf_inverse_map = 0;
uint8_t vwf_current_tile;
uint8_t vwf_mode = VWF_MODE_PRINT;

uint8_t * vwf_render_base_address = VWF_DEFAULT_BASE_ADDRESS;

font_desc_t vwf_current_font_desc;
uint8_t vwf_current_font_bank;

void vwf_print_shift_char(void * dest, const void * src, uint8_t bank) OLDCALL;
        void vwf_memcpy(void* to, const void* from, size_t n, uint8_t bank) OLDCALL;
uint8_t vwf_read_banked_ubyte(const void * src, uint8_t bank) OLDCALL __preserves_regs(b, c) ;
        uint8_t * vwf_get_win_addr() OLDCALL __preserves_regs(b, c, h, l) ;
uint8_t * vwf_get_bkg_addr() OLDCALL __preserves_regs(b, c, h, l) ;
        void vwf_set_banked_data(uint8_t i, uint8_t l, const unsigned char* ptr, uint8_t bank) OLDCALL;
void vwf_swap_tiles() OLDCALL;


        void vwf_set_destination(vwf_reder_dest_e destination) {
    vwf_render_base_address = (destination == VWF_RENDER_BKG) ? vwf_get_bkg_addr() : vwf_get_win_addr();
}

void vwf_print_reset(uint8_t tile) {
    vwf_current_tile = tile;
    vwf_current_offset = 0;
    vwf_swap_tiles();
    vwf_swap_tiles();
}

uint8_t vwf_print_render(const unsigned char ch) {
    uint8_t letter = vwf_read_banked_ubyte(vwf_current_font_desc.recode_table + (ch & ((vwf_current_font_desc.attr & RECODE_7BIT) ? 0x7fu : 0xffu)), vwf_current_font_bank);
    const uint8_t * bitmap = vwf_current_font_desc.bitmaps + (uint16_t)letter * 8;
    if (vwf_current_font_desc.attr & FONT_VWF) {
        uint8_t width = vwf_read_banked_ubyte(vwf_current_font_desc.widths + letter, vwf_current_font_bank);
        uint8_t dx = (8u - vwf_current_offset);
        vwf_current_mask = (0xffu << dx) | (0xffu >> (vwf_current_offset + width));

        vwf_current_rotate = vwf_current_offset;
        vwf_print_shift_char(vwf_tile_data, bitmap, vwf_current_font_bank);
        if ((uint8_t)(vwf_current_offset + width) > 8u) {
            vwf_current_rotate = dx | 0x80u;
            vwf_current_mask = 0xffu >> (width - dx);
            vwf_print_shift_char(vwf_tile_data + DEVICE_TILE_SIZE, bitmap, vwf_current_font_bank);
        }
        vwf_current_offset += width;

        if (vwf_current_offset > 7u) {
            vwf_current_offset -= 8u;
            set_bkg_1bpp_data(vwf_current_tile, (vwf_current_offset) ? 2 : 1, vwf_tile_data);
            vwf_current_tile++;
            vwf_swap_tiles();
            return TRUE;
        };
        set_bkg_1bpp_data(vwf_current_tile, 1, vwf_tile_data);
        return FALSE;
    } else {
        vwf_set_banked_data(vwf_current_tile++, 1, bitmap, vwf_current_font_bank);
        vwf_current_offset = 0;
        return TRUE;
    }
}

uint8_t vwf_draw_text(uint8_t x, uint8_t y, uint8_t base_tile, const unsigned char * str) {
    static uint8_t * ui_dest_base, *ui_dest_ptr;
    static const uint8_t * ui_text_ptr;
    ui_dest_ptr = ui_dest_base = vwf_render_base_address + (y + DEVICE_SCREEN_Y_OFFSET) * (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE) + ((x + DEVICE_SCREEN_X_OFFSET) * DEVICE_SCREEN_MAP_ENTRY_SIZE);
    ui_text_ptr = str;

    vwf_print_reset(base_tile);
    while (*ui_text_ptr) {
        switch (*ui_text_ptr) {
            case 0x01:
                vwf_activate_font(*++ui_text_ptr);
                break;
            case 0x02:
                ui_dest_ptr = ui_dest_base = vwf_render_base_address + (*++ui_text_ptr + DEVICE_SCREEN_Y_OFFSET) * (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE) + ((*++ui_text_ptr + DEVICE_SCREEN_X_OFFSET) * DEVICE_SCREEN_MAP_ENTRY_SIZE);
                if (vwf_current_offset) vwf_print_reset(vwf_current_tile + 1u);
                break;
            case 0x03:
                vwf_inverse_map = *++ui_text_ptr;
                break;
            case '\n':
                ui_dest_ptr = ui_dest_base += (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE);
                if (vwf_current_offset) vwf_print_reset(vwf_current_tile + 1u);
                break;
            default:
                if (vwf_print_render(*ui_text_ptr)) {
                    if (vwf_mode & VWF_MODE_PRINT) set_vram_byte(ui_dest_ptr, vwf_current_tile - 1);
                    ui_dest_ptr += DEVICE_SCREEN_MAP_ENTRY_SIZE;
                }
                if ((vwf_current_offset) && (vwf_mode & VWF_MODE_PRINT)) set_vram_byte(ui_dest_ptr, vwf_current_tile);
                break;
        }
        ui_text_ptr++;
    }
    return vwf_next_tile() - base_tile;
}

void vwf_load_font(uint8_t idx, const void * font, uint8_t bank) {
    vwf_fonts[idx].bank = bank;
    vwf_fonts[idx].ptr = (void *)font;
    vwf_activate_font(idx);
}

void vwf_activate_font(uint8_t idx) {
    vwf_current_font_bank = vwf_fonts[idx].bank;
    vwf_memcpy(&vwf_current_font_desc, vwf_fonts[idx].ptr, sizeof(font_desc_t), vwf_current_font_bank);
}

uint8_t vwf_next_tile() {
    return (vwf_current_offset) ? vwf_current_tile + 1u : vwf_current_tile;
}

//region User controlled text area

//For knowing how quickly to render characters
uint8_t vwf_textarea_characters_per_tick;
uint8_t vwf_textarea_animationticks_per_vwf_tick;

//For knowing the size of our textarea
uint8_t vwf_textarea_x;
uint8_t vwf_textarea_y;
uint8_t vwf_textarea_w;
uint8_t vwf_textarea_h;

//For knowing the current tile in vram
uint8_t vwf_textarea_start_tile;
uint8_t vwf_textarea_ending_tile;
uint8_t vwf_textarea_current_tile;

//For knowing the current width index of our vram tile
uint8_t vwf_textarea_vram_width;
uint8_t vwf_textarea_vram_count;

//For knowing what we are in the middle of rendering
vwf_text_segment_t * vwf_textarea_current_segment;
uint8_t vwf_textarea_current_segment_bank;

//For tracking where in the current segment's string we are for rendering
uint16_t vwf_textarea_current_character_index;

void vwf_initialize_textarea(uint8_t xTile, uint8_t yTile, uint8_t width, uint8_t height, uint8_t vram_start_index) {
    vwf_textarea_x = xTile;
    vwf_textarea_y = yTile;
    vwf_textarea_w = width;
    vwf_textarea_h = height;
    vwf_textarea_current_tile = vram_start_index;
    vwf_textarea_vram_count = width * height;
    //TODO: add some sort of check to make sure that start index isn't too high?
    vwf_textarea_ending_tile = vwf_textarea_vram_count + vram_start_index;
    vwf_textarea_vram_width = 0u;
    vwf_textarea_current_character_index = 0u;

    //TODO: Initialize the vram tiles
    //TODO: Assign the bg tiles to the vram indexes
}

void vwf_set_text_segment(vwf_text_segment_t * first_text_segment_ptr, uint8_t text_segment_bank) {
    vwf_textarea_current_segment = first_text_segment_ptr;
    vwf_textarea_current_segment_bank = text_segment_bank;
}

void vwf_set_text_speed(uint8_t characters_per_tick, uint8_t animationticks_per_character_tick) {
    vwf_textarea_characters_per_tick = characters_per_tick;
    vwf_textarea_animationticks_per_vwf_tick = animationticks_per_character_tick;
}

void vwf_textarea_vblank_update() {
    //TODO: Determine if we need to draw a new character
    if(1) {
        //TODO: Process characters in the current segment until we have reached a renderable character
        //TODO: Render the new character
        //TODO: Do we need to render more characters during this vblank?
    }

    //TODO: Do we need to change any sprites for the text box animation?
}

//endregion