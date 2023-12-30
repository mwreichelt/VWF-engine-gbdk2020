        .include        "global.s"

        .globl _set_bkg_1bpp_data

        .area _DATA

__save:
        .ds 0x01

        .area _GSINIT

        call _vwf_get_bkg_addr
        ld hl, #_vwf_render_base_address
        ld (hl), e
        inc hl
        ld (hl), d

        .area _CODE

; void vwf_memcpy(void* to, const void* from, size_t n, UBYTE bank);
_vwf_memcpy::
        ldh a, (__current_bank)
        ld  (#__save),a

        ldhl sp, #8
        ld  a, (hl-)
        ldh (__current_bank),a
        ld  (#0x2000), a

        ld a, (hl-)
        ld b, a
        ld a, (hl-)
        ld c, a
        ld a, (hl-)
        ld d, a
        ld a, (hl-)
        ld e, a
        ld a, (hl-)
        ld l, (hl)
        ld h, a

        inc b
        inc c
        jr 2$
1$:
        ld a, (de)
        ld (hl+), a
        inc de
2$:
        dec c
        jr nz, 1$
        dec b
        jr nz, 1$

        ld  a, (#__save)
        ldh (__current_bank),a
        ld  (#0x2000), a
        ret

; UBYTE vwf_read_banked_ubyte(const void * src, UBYTE bank) __preserves_regs(b, c);
_vwf_read_banked_ubyte::
        ldh a, (__current_bank)
        ld  (#__save),a

        ldhl  sp, #4
        ld  a, (hl-)
        ldh (__current_bank),a
        ld  (#0x2000), a

        ld  a, (hl-)
        ld  l, (hl)
        ld  h, a
        ld  e, (hl)

        ld  a, (#__save)
        ldh (__current_bank),a
        ld  (#0x2000), a
        ret

; void vwf_set_banked_bkg_data(UBYTE i, UBYTE l, const unsigned char* ptr, UBYTE bank);
_vwf_set_banked_data::
        ldh a, (__current_bank)
        ld  (#__save),a

        ldhl  sp, #6
        ld  a, (hl)
        ldh (__current_bank),a
        ld  (#0x2000), a

        pop bc
        call  _set_bkg_1bpp_data

        ld  a, (#__save)
        ldh (__current_bank),a
        ld  (#0x2000), a
        ld  h, b
        ld  l, c
        jp  (hl)