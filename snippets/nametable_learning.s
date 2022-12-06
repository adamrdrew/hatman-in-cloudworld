; Let's learn to load nametables and display background graphics

.include "../includes/constants.inc"
.include "../includes/header.inc"
.include "../includes/init.inc"

; $0800
.segment "CODE"

    ; This is a subroutine and it is doooooope
    ; Subroutines are scoped - meaning labels defined in subroutines
    ; are local to that subroutine
    .proc LoadPalettes
        bit PPU_STATUS   ; Resets the PPU_ADDRESS latch register
        ldx #$3F
        stx PPU_ADDR    ; Set the MSB of the PPU address we'll update to $3F
        ldx #$00
        stx PPU_ADDR    ; Set the LSB of the PPU address we'll update to $00 
        ; This is a for loop
        ; We are looping through PaletteData and reading those
        ; bytes into the PPU_DATA register
        ; Also notice that we don't have to constantly update the PPU_ADDR
        ; we're writing into. You only need to set the head and then it will increment
        ; as you write. Not bad!
        ldy #0 ; for i=0
        bit PPU_STATUS
        ReadPaletteBytes:
            ; x = PaletteData[y]
           ldx PaletteData,y
           ; ppu_data_write(x)
           stx PPU_DATA
           iny ; i++
           cpy #32 ; if y == 32
           ; break
           bne ReadPaletteBytes 
        ; rts is required at the end of a subroutine
        rts
    .endproc

    .proc LoadNametable
        bit PPU_STATUS
        ldx #$20
        stx PPU_ADDR    ; Set the MSB of the PPU address we'll update to $3F
        ldx #$00
        stx PPU_ADDR    ; Set the LSB of the PPU address we'll update to $00 

        ldy #0
        ReadNametableBytes:
            ldx BackgroundData, y
            stx PPU_DATA
            iny 
            cpy #255
            bne ReadNametableBytes
        
        rts 
    .endproc

    ; Code of PRG ROM
    Reset:
        INIT_NES

    PaintBackround:
        jsr LoadPalettes
        jsr LoadNametable

        ;Enable PPU Rendering
        lda #%10010000  ; Enable NMI interrupts from PPU. Set BG to use 2nd pattern table
        sta PPU_CTRL
        lda #%00011110
        sta PPU_MASK    ; Setting the mask ensures we show the background

    MainLoop:
        jmp MainLoop

    NMI:
        rti ; Return from Interrupt
    IRQ:
        rti ; Return from Interrupt

    ; This is an array, it just looks kinda funny
    ; It is easy to think of labels as function names, but they are not, the are just
    ; names for positions in address space. So here we are creating a label called
    ; PaletteData
    ; Then we define 16 bytes that follow it. The .byte directive simply sets
    ; the value at the current spot in address space. So what we've defined here
    ; is a sequential list of bytes with the label PaletteData assigned to the 
    ; beginning of the series. It is quite literally an array of bytes.
    ; And because we know the array's origin address (PaletteData) and its length (32)
    ; we can iterate through it :)
    PaletteData:
        .byte $30,$00,$16,$30, $0F,$00,$16,$30, $0F,$00,$16,$30, $0F,$00,$16,$30 ; Background
        .byte $0F,$00,$16,$30, $0F,$2A,$0C,$3A, $0F,$2A,$0C,$3A, $0F,$2A,$0C,$3A ; Sprites

    ;This is tile data that must be copied to the nametable
    BackgroundData:
        ;.incbin "resources/hello_hackathon.nam"
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$12,$0f,$16,$16,$19,$00,$12,$0b
        .byte $0d,$15,$0b,$1e,$12,$19,$18,$26,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$27,$0b,$0e,$0e
        .byte $1c,$0f,$21,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$e0,$e1
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$f0,$f1
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00

;Load CHAR_ROM pattern tables
.segment "CHARS"
.incbin "resources/learning.chr"

; $FFFA
.segment "VECTORS"
    ;On boot the NES jumps to $FFFA to find the addresses for the handlers
    ;This is called the VECTORS 
    .word NMI   ; address of the NMI handler
    .word Reset ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
