; Let's learn to load nametables and display background graphics

.include "../includes/constants.inc"
.include "../includes/header.inc"
.include "../includes/init.inc"
.include "../includes/utils.inc"

.segment "ZEROPAGE"
Frame:   .res 1         ; Reserve 1 byte to store the framecounter
Seconds: .res 1         ; Reserve 1 butes to store the second counter, increments every 60 frames
BackgroundPtr: .res 2   ; Reserve 2 bytes for a pointer to a background array
BGDrawPointer: .res 2   ; Reserve 2 bytes for the pointer we use to iterate through while drawing

.segment "CODE"

    ; This is a subroutine and it is doooooope
    ; Subroutines are scoped - meaning labels defined in subroutines
    ; are local to that subroutine
    .proc LoadPalettes
        PPU_SETADDR $3F00
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
        PPU_SETADDR $2000
        
        ; Copy the BackgroundPtr so we don't stomp over it while we're iterating
        CopyPointer BackgroundPtr, BGDrawPointer

        ldy #0  ; y stores the lower bit offset into the bg data arry
        ldx #0  ; x stores the higher bit offset into the bg data array
        GetChunk:  ; We can only load 255 at a time, so we break it into 4 255 char chunks 
            ReadBytesInChunk:
                lda (BGDrawPointer), y ; Read value from address BGDrawPointer + y
                sta PPU_DATA
                iny 
                cpy #255 ; Keep iterating until we've read all 55 bytes in the chunk
                bne ReadBytesInChunk
            inc BGDrawPointer+1 ;Increate the hi-byte in the pointer address by one, which will increase our offset by 255
            inx ; increase x
            cpx #4 ; iterate for 4 chunks (4 chunks of 255b = 1kb)
            bne GetChunk
        rts 
    .endproc

    ; Code of PRG ROM
    Reset:
        INIT_NES
        lda #0
        sta Frame
        sta Seconds
        sta BackgroundPtr
        sta BackgroundData
        ; Set background pointer
        SetPointer BackgroundPtr, BackgroundData


    PaintBackround:
        jsr LoadPalettes
        jsr LoadNametable

        ;Enable PPU Rendering
        lda #%10010000  ; Enable NMI interrupts from PPU. Set BG to use 2nd pattern table
        sta PPU_CTRL
        ; Disable PPU Scrolling. This is customary to do when drawing to prevent errant scrolling
        ; PPU_SCROLL is also a latch address, so we need to set it twice
        lda #0
        sta PPU_SCROLL ; X Scrolling
        sta PPU_SCROLL ; Y Scrolling
        lda #%00011110
        sta PPU_MASK    ; Setting the mask ensures we show the background

    MainLoop:
        jmp MainLoop

    NMI:
        inc Frame
        lda Frame
        cmp #60
        bne :+
        inc Seconds
        lda #0
        sta Frame
        :
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
        .byte $0D,$00,$16,$20, $0D,$17,$1A,$2A, $0D,$1C,$3C,$20, $0D,$2D,$01,$21 ; Background
        .byte $0D,$00,$16,$20, $0D,$17,$1A,$2A, $0D,$1C,$3C,$20, $0D,$2D,$01,$21 ; Sprites

    ;This is tile data that must be copied to the nametable
    BackgroundData:
        .incbin "resources/full_screen_nametable.nam"
        

    AttributeData:
        .byte %00000000, %00000000, %10101010, %00000000, %11110000, %00000000, %00000000, %00000000
        .byte %11111111, %11111111, %11111111, %11111111, %11111111, %11111111, %11111111, %11111111

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
