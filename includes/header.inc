;iNES Header
;https://www.nesdev.org/wiki/INES
.segment "HEADER"
    .byte $4E,$45,$53,$1A       ; 4 bytes with chars NES\n
    .byte $02                   ; 2 x 16kb PRG ROM
    .byte $01                   ; 1 x 8kb CHAR ROM
    .byte %00000000             ; Horiz mirror, no mapper, no battery
    .byte %00000000             ; No play choice, no mapper, no NES 2.0
    .byte $00                   ; No PRG RAM
    .byte $00                   ; NTSC
    .byte $00                   ; TV system, and memory options
    .byte $00,$00,$00,$00,$00   ; Padding to fill header