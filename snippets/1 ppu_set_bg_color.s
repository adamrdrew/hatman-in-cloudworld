; This program shows basic PPU usage. It sets the background color to lime green

.include "../includes/constants.inc"
.include "../includes/header.inc"
.include "../includes/init.inc"

; $0800
.segment "CODE"
    ; Code of PRG ROM
    Reset:
        INIT_NES

    PaintBackround:
        bit PPU_STATUS   ; Resets the PPU_ADDRESS latch register
        ldx #$3F
        stx PPU_ADDR    ; Set the MSB of the PPU address we'll update to $3F
        ldx #$00
        stx PPU_ADDR    ; Set the LSB of the PPU address we'll update to $00 
        ldx #$2A
        stx PPU_DATA    ; Wriote $2A to $3F00 via the PPU_DATA register
        lda #%00011110
        sta PPU_MASK    ; Setting the mask ensures we show the background

    MainLoop:
        jmp MainLoop

    NMI:
        rti ; Return from Interrupt
    IRQ:
        rti ; Return from Interrupt

; $FFFA
.segment "VECTORS"
    ;On boot the NES jumps to $FFFA to find the addresses for the handlers
    ;This is called the VECTORS 
    .word NMI   ; address of the NMI handler
    .word Reset ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
