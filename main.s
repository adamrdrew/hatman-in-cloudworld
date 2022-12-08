; Let's learn to load nametables and display background graphics

.include "includes/constants.inc"
.include "includes/header.inc"
.include "includes/init.inc"
.include "includes/utils.inc"

; This is where our ZeroPage and all our global variables are defined.
.include "includes/variables.inc"


.segment "CODE"

    .include "includes/controller.inc"
    .include "includes/player.inc"

    .proc LoadSprites
        ldx $00
        WhileSpriteData:
            lda SpriteData, x
            sta OAM_COPY, x
            inx
            cpx #16
            bne WhileSpriteData
        rts
    .endproc

    .proc ShowText
        bit PPU_STATUS
        SetPPUAddresByPointer DrawTextPosPtr

        ldy #0
        WhileTextByteNotZero:
            lda (DrawTextPtr), y
            cmp $0
            beq BreakLoop
            sta DrawTextAsciiCode
            ldx DrawTextAsciiCode
            lda ASCIITable, x
            sta PPU_DATA
            iny
            jmp WhileTextByteNotZero
        BreakLoop:
        rts
    .endproc

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
        INIT_VARIABLES

    PaintBackround:
        jsr LoadPalettes
        jsr LoadNametable
        jsr LoadSprites

        SetPointer DrawTextPtr, TextMessage
        SetPointer DrawTextPosPtr, $2020
        jsr ShowText

        SetPointer DrawTextPtr, FromAdamMessage
        SetPointer DrawTextPosPtr, $2060
        jsr ShowText

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
        ; Copy sprite data to OAM
        lda #$02            ; Where OAM should start copying from $02 means it will copy from $0200-02FF
        sta OAM_DMA_COPY    ; Send the source address to initial DMA copy
        
        jsr Controller_ReadButtons
        jsr Controller_ButtonHandler
        jsr Player_Step


        ManageTime:
            inc Frame
            lda Frame
            cmp #60
            bne NotFrameSixty
            inc Seconds
            lda #0
            sta Frame
            NotFrameSixty:

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
        .byte $21,$0D,$16,$20, $21,$0D,$07,$1B, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Background
        .byte $21,$0D,$16,$20, $21,$0D,$1A,$2A, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Sprites

    ;This is tile data that must be copied to the nametable
    BackgroundData:
        .incbin "maps/lvl1.nam"
        
    TextMessage:
        .byte "LIVES 03", $0
    FromAdamMessage:
        .byte "SCORE 0000", $0

    ASCIITable:
        ; Position is ASCII Code. Value at position is Tile ID
        ;     0   1   2    3   4   5    6   7   8    9   10  11
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $26,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$25,$00
        .byte $01,$02,$03, $04,$05,$06, $07,$08,$09, $0a,$00,$00
        .byte $00,$00,$00, $00,$27,$0b, $0c,$0d,$0e, $0f,$10,$11
        .byte $12,$13,$14, $15,$16,$17, $18,$19,$1a, $1b,$1c,$1d
        .byte $1e,$1f,$20, $21,$22,$24, $24,$00,$00, $00,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$00,$00
        .byte $00,$00,$00, $00,$00,$00, $00,$00,$00, $00,$00,$00

    SpriteData:
        ;       Y       Tile    Attrs       X
        .byte   $80,    $00,    %00000000,  $70
        .byte   $80,    $01,    %00000000,  $78
        .byte   $88,    $10,    %00000000,  $70
        .byte   $88,    $11,    %00000000,  $78


;Load CHAR_ROM pattern tables
.segment "CHARS"
.incbin "graphics/cloudworld.chr"

; $FFFA
.segment "VECTORS"
    ;On boot the NES jumps to $FFFA to find the addresses for the handlers
    ;This is called the VECTORS 
    .word NMI   ; address of the NMI handler
    .word Reset ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
