#include <gbdk/platform.h>
#include <stdlib.h>
#include <string.h>

#include "vwf.h"
#include "vwf_textarea.h"
#include "vwf_font.h"
#include "vwf_font_bold.h"
#include "vwf_font_ru.h"

const vwf_text_segment_t segment_1 = { &segment_2, "This is a textarea.\nPress A to advance text!\0"};
//const vwf_text_segment_t segment_1 = { &segment_2, "without any sort of merit or plan.\0"};
const vwf_text_segment_t segment_2 = { &segment_3, "Pretty neat!\0"};
const vwf_text_segment_t segment_3 = { &segment_1, "They had !l!i!t!t!l!e! !m!o!n!e!y.\0"};
//const vwf_text_segment_t segment_3 = { &segment_1, "This is a really long piece of text without any sort of breaks or manual newlines. I think that most of the time I'll probably write text like this so I want to make sure it works. Also I want to test the length. Supercalifragilisticexpialudoucious."};


//This is for debug purposes. I'm tired of rewriting it so I'm leaving it in this time.
unsigned char ITOA_STRING[4];
void writeDebugInt(uint8_t x,uint8_t y, uint8_t value, uint8_t vram_addr) {
    uitoa(value, (char *)ITOA_STRING, 10);
    vwf_draw_text(x, y, vram_addr, ITOA_STRING);
}

void main(void) {
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0x00u);
    vwf_textarea_enabled = FALSE;
    SHOW_BKG;

    vwf_set_destination(VWF_RENDER_BKG);

    vwf_load_font(0, vwf_font, BANK(vwf_font));
    vwf_load_font(1, vwf_font_bold, BANK(vwf_font_bold));
    vwf_load_font(2, font_ru, BANK(font_ru));
    vwf_activate_font(2);

    vwf_draw_text(3, 3, 1, "Hello, world!\n\x01\x01\BOLD\x01\x00\n\x03\xffthis is a \x01\x01test\x01\x00\x03\x00\nof the new line\ncontrol symbol");
    vwf_draw_text(8, 9, vwf_next_tile(), "This is another\ntext block that is\nprinted in the other\nplace on screen");
    vwf_draw_text(12, 1, vwf_next_tile(), "The third\ntext block\nto the right\nof the first\none");

#if defined(NINTENDO)
    fill_win_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0x00u);

    vwf_set_destination(VWF_RENDER_WIN);
    vwf_draw_text(1, 0, vwf_next_tile(), "This text should be rendered\non the window layer.");

    WX_REG = 7u; WY_REG = 144;
    SHOW_WIN;
    for (uint8_t i = 143; i != 103; i--) {
        WY_REG = i;
        wait_vbl_done();
    }

    //Time to setup the textarea for rendering.

    //This is where we register the textarea vblank interrupt handler. This handler is what does the rendering of the
    // text when the device's vblank interrupt occurs.
    add_VBL(vwf_textarea_vblank_update);

    // First, we need to activate a font.
    // Second, we need to initialize the textarea by telling the library the position and dimensions of the textarea.
    //  Then, we tell the library what memory address in VRAM to start rendering. The library will use consecutive
    //   blocks of VRAM memory to render text.
    //  Finally, we tell it what VRAM tile we would like it to use to render the background of the textarea.
    //  I would recommend making this a "blank" tile as patterned tiles are untested (but potentially cool).
    vwf_textarea_activate_font(0u);
    vwf_initialize_textarea(1u, 1u, 10u, 2u, 0x82, 0x00);

    //Set next text segment for the textarea to render. We need to load the pointer to the correct text segment and
    // specify what bank that data lives in. If you have a 32kb game with MBC0 then bank will always be 0.
    vwf_textarea_current_segment = (vwf_text_segment_t *) &segment_1;
    vwf_textarea_current_segment_bank = 0;

    //With the setup done, we toggle this flag on to enable the textarea and let it know it's okay to start rendering.
    //Until this all the above setup is done, the textarea engine cannot render anything!
    vwf_textarea_enabled = TRUE;

    //What follows is a mock game loop, meant to respond to user input for the textarea.
    uint8_t prev_joypad = 0;
    uint8_t curr_joypad = 0;
    while(1) {
        prev_joypad = curr_joypad;
        curr_joypad = joypad();

        //In this example code, we're going to use the A button being pressed as our signal to the library to advance
        // the text when it's been paused.
        //We'll just look at the textarea paused flag and check that the button has been depressed this frame and not
        // on the previous frame, but in your project, you may want to do something more sophisticated.
        //Only you can know when it's safe and appropriate to advance the textarea rendering to the next bit of text!
        if(vwf_textarea_textfill_paused && (curr_joypad & J_A) && !(prev_joypad & J_A)) {
            vwf_textarea_textfill_advance = TRUE;
        }

        wait_vbl_done();
    }

#endif
}
