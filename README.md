# VWF-engine-gbdk2020
A Variable Width Font engine for Nintendo Game Boy, (and maybe Sega Master System and Sega Game Gear) written for gbdk2020. Forked from untoxa/VWF which is recommended if you don't need all the extra functions here.

This library requires the latest [GBDK-2020 v.4.1.1](https://github.com/gbdk-2020/gbdk-2020/releases/latest/) and GNU Make. Windows users may use mingw or similar.

Current Features from [untoxa/VWF](http://github.com/untoxa/VWF):
 - use up to 4 fonts at the same time (look at the vwf_font.c, font format is quite obvious, font tiles are 1-bit)
 - set foreground and background colors
 - you can render text to tiles and screen, or to tiles only
 - gb version supports printing onto background as well as on window layer
 - support for control characters in text for inversing text, gotoxy or activate another font

New Features:
 - Added png2font.py (needs python3) from the [untoxa/gb-photo](http://github.com/untoxa/VWF-engine-gbdk2020) to the utils folder
   - Added a description which specifically mentions needing a png file with indexed colors to generate the font from
   - Added -v flag for verbose output when your font didn't generate correctly and you aren't sure why
   - Added -r for reverse palette order. This is useful for when you realize that you've made a .png for your font with your color indexes in the opposite order than what png2font expects. Or when you'd like to create an inverted version of your font.

Planned Features (in order of how likely I am to actually implement them):
- Support for text advancement in a player controlled text area
  - You know, like a JRPG text box
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
 

Screenshots from [untoxa/VWF](http://github.com/untoxa/VWF):

![GameBoy](/gb.png) ![GameGear](/gg.png)

![MasterSystem](/sms.png)
