        .include        "global.s"

        .globl _set_tile_1bpp_data, __current_1bpp_colors

        .ez80

        .area _DATA

__save:
        .ds 0x01

        .area _GSINIT

        call _vwf_get_bkg_addr
        ld (_vwf_render_base_address), hl

        .area _CODE

; void vwf_memcpy(void* to, const void* from, size_t n, UBYTE bank) Z88DK_CALLEE;
_vwf_memcpy::
        pop iy
        pop de
        pop hl
        pop bc
        dec sp
        ex (sp), iy

        ld  a, (.MAP_FRAME1)
        ld  (#__save), a
        ld  a, iyh
        ld (.MAP_FRAME1), a

        ldir

        ld  a, (#__save)
        ld  (.MAP_FRAME1),a
        ret

; UBYTE vwf_read_banked_ubyte(const void * src, UBYTE bank) Z88DK_CALLEE;
_vwf_read_banked_ubyte::
        ld  a, (.MAP_FRAME1)
        ld  (#__save), a

        pop hl
        pop de
        dec sp
        ex (sp), hl

        ld a, h
        ld (.MAP_FRAME1), a

        ex de, hl
        ld l, (hl)

        ld  a, (#__save)
        ld (.MAP_FRAME1), a
        ret

; void vwf_set_banked_bkg_data(UBYTE i, UBYTE l, const unsigned char* ptr, UBYTE bank) Z88DK_CALLEE;
_vwf_set_banked_data::
        ld  a, (.MAP_FRAME1)
        ld  (#__save), a

        pop hl
        pop de
        pop bc
        dec sp
        ex (sp), hl

        ld a, h
        ld (.MAP_FRAME1), a

        ld hl, (__current_1bpp_colors)
        push hl
        push bc
        ld h, #0
        ld l, d
        push hl
        ld l, e
        push hl

        call  _set_tile_1bpp_data

        ld  a, (#__save)
        ld (.MAP_FRAME1), a
        ret