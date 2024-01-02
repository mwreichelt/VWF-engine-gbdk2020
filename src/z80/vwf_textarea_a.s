        .include        "global.s"

        .globl _vwf_textarea_current_rotate, _vwf_textarea_current_mask, _vwf_textarea_inverse_map, _vwf_textarea_tile_data, _vwf_textarea_inverse_map

        .ez80

        .area _DATA

__save:
        .ds 0x01

        .area _CODE



; void vwf_print_shift_char(void * dest, const void * src, UBYTE bank) Z88DK_CALLEE;
_vwf_textarea_print_shift_char::
        ld  a, (.MAP_FRAME1)
        ld  (#__save), a

        pop hl
        pop de
        pop bc
        dec sp
        ex (sp), hl

        ld a, h
        ld (.MAP_FRAME1), a

        ex de, hl
        ld d, b
        ld e, c

        ld b, #8
3$:
        ld a, (de)
        ld c, a
        ld a, (_vwf_textarea_inverse_map)
        xor c
        ld c, a
        inc de

        ld a, (_vwf_textarea_current_rotate)
        sla a
        jr z, 1$
        jr c, 4$
        srl a
        srl a
        jr nc, 6$
        srl c
6$:
        or a
        jr z, 1$
2$:
        srl c
        srl c
        dec a
        jr nz, 2$
        jr 1$
4$:
        srl a
        srl a
        jr nc, 7$
        sla c
7$:     or a
        jr z, 1$
5$:
        sla c
        sla c
        dec a
        jr nz, 5$
1$:
        ld a, (_vwf_textarea_current_mask)
        and (hl)
        or c
        ld (hl), a
        inc hl

        dec b
        jr nz, 3$

        ld  a, (#__save)
        ld (.MAP_FRAME1), a

        ret

_vwf_textarea_get_win_addr::
_vwf_textarea_get_bkg_addr::
        ld a, (_shadow_VDP_R2)
        rlca
        rlca
        and #0b01111000
        ld h, a
        ld l, #0
        ret

_vwf_textarea_swap_tiles::
        ld      de, #_vwf_textarea_tile_data
        ld      hl, #(_vwf_textarea_tile_data + 8)
        .rept 8
                ldi
        .endm
        ld      a, (_vwf_textarea_inverse_map)
        ld      (de), a
        ld      h, d
        ld      l, e
        inc     de
        .rept 7
                ldi
        .endm
        ret
