# VWF-textarea-gbdk
A Variable Width Font engine for Nintendo Game Boy, (and maybe Sega Master System and Sega Game Gear) written for gbdk2020. Forked from untoxa/VWF which is recommended if you don't need all the extra functions here.
## Currently, this library does not work!

## Pre-Requisites

This library requires the latest [GBDK-2020 v.4.1.1](https://github.com/gbdk-2020/gbdk-2020/releases/latest/) and GNU Make. Windows users may use mingw or similar.

## Features

### Current Features from [untoxa/VWF](http://github.com/untoxa/VWF):
 - use up to 4 fonts at the same time (look at the vwf_font.c, font format is quite obvious, font tiles are 1-bit)
 - set foreground and background colors
 - you can render text to tiles and screen, or to tiles only
 - gb version supports printing onto background as well as on window layer
 - support for control characters in text for inversing text, gotoxy or activate another font

### New Features:
 - Added png2font.py (needs python3) from the [untoxa/gb-photo](http://github.com/untoxa/VWF-engine-gbdk2020) to the utils folder
   - Added a description which specifically mentions needing a png file with indexed colors to generate the font from
   - Added -v flag for verbose output when your font didn't generate correctly and you aren't sure why
   - Added -r for reverse palette order. This is useful for when you realize that you've made a .png for your font with your color indexes in the opposite order than what png2font expects. Or when you'd like to create an inverted version of your font.
 - Textarea basic printing. Define an x,y coordinate and a width and height, specify a text segment to print, and register your vblank interrupt handler to make it print one character at a time.
   - Supports manual linebreaks ("\n" characters)

### Planned Features (in order of how likely I am to actually implement them):
- Character wrapping
- Word wrapping within a text area
- Support for text advancement in a player controlled text area
    - You know, like a JRPG text box
- GG and SMS support for textareas
- Text printing speed
- Automatic pausing of printing text that is too large for the text area
- Control codes 
     - Inserting delays between characters
     - Running animations
     - Automatic line break support
- Thorough Documentation, small examples, installation instructions
- Make file targets to compile files for you to add to your project with certain features turned on or off
 - The ability to skip through text by pressing a button
 - The ability to speed up the rendering of text by pressing a button
 - Animation of text scrolling up

## Installation
 There are several ways to get the files for this library onto your computer:
1) Download the latest release for your CPU (z80 or sm83) from the releases page
2) Clone this repository and copy the files from the build/sm83 or build/z80 folder
   * Note that at the time of this writing, the z80 port does not support textareas
3) Download the library and .h files in the build/sm83 or build/z80 folder that is appropriate for your GBDK2020 project
4) Add this project as a git submodule to your gbdk2020 project's git repository

However you do it, once you have the files on your computer, you should have three header files and at least one .lib file.
The header files vwf.h, vwf_common.h, and vwf_textarea.h should be referenced as needed in your IDE and in your Makefile or equivalent.
* As a reminder, the option to include a folder with header files for lcc is: `<LCC_PATH> -I<FOLDER_PATH>`
* And the option to include a library file is like this: `<LCC_PATH> -Wl-l<LIB_FILE_PATH>`
* So, if I had my .h and .lib files in a folder named `vwf` at the same level as the Makefile, and the lcc executable was in a separate gbdk2020 folder one level up, then my lcc calls might look something like this: `../gbdk/bin/lcc -Ivwf -Wl-lvwf/vwf-textarea_full.lib`

## Screenshots

![GameBoy](/gb.png)

Screenshots from [untoxa/VWF](http://github.com/untoxa/VWF):

![GameGear](/gg.png)

![MasterSystem](/sms.png)
