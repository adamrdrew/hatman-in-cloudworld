; This is a basic NES ASM program that sets headers, handlers, clears memory, and then just hangs in a loop

;iNES Header
;https://www.nesdev.org/wiki/INES
.segment "HEADER"
.org $7FF0
    .byte $4E,$45,$53,$1A       ; 4 bytes with chars NES\n
    .byte $02                   ; 2 x 16kb PRG ROM
    .byte $01                   ; 1 x 8kb CHAR ROM
    .byte %00000000             ; Horiz mirror, no mapper, no battery
    .byte %00000000             ; No play choice, no mapper, no NES 2.0
    .byte $00                   ; No PRG RAM
    .byte $00                   ; NTSC
    .byte $00                   ; TV system, and memory options
    .byte $00,$00,$00,$00,$00   ; Padding to fill header

; $0800
.segment "CODE"
    ;Code of PRG ROM

    PPU_MASK = $2001
    PPU_ADDR = $2006
    PPU_DATA = $2007

    ;Reset code. This clears memory and registers.
    RESET:
        sei ;Disables all IRQs
        cld ;Clears decimal mode
        ldx #$FF ; Set X to hex literal FF
        txs ;Set the stack pointer to $01FF

        ; This next block sets all memory between $FF and $00 to 0
        lda #0          ; Load A with 0
        ldx #$00        ; Load X with 0. You'd think we want to start at $FF 
                        ; but we start at 0 so it will clear 0, decrement which will wrap to FF, 
                        ; and then proceed down. This prevents an off by one bug where we'd miss clearing
                        ; 0 because we branch on 0 in the loop
        ClearRAM:   ; Set a label
            sta $0000,x ; Store value in A (0) in memory address $0 + x (You can't just say x, you gotta do this weird little add) 
            sta $0100,x
            sta $0200,x
            sta $0300,x
            sta $0400,x
            sta $0500,x
            sta $0600,x
            sta $0700,x
            dex         ; x--
            bne ClearRAM


    MainLoop:
        ldx #$3F
        stx PPU_ADDR
        ldx #$00
        stx PPU_ADDR
        ldx #$2A
        stx PPU_DATA
        lda #%00011110
        sta PPU_MASK
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
    .word RESET ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
