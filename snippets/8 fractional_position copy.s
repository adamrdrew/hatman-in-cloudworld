; Let's learn to load nametables and display background graphics

.include "../includes/constants.inc"
.include "../includes/header.inc"
.include "../includes/init.inc"
.include "../includes/utils.inc"

.segment "ZEROPAGE"
Buttons: .res 1         ; Reserve one by to represent button state (a,b,sel,sta,u,d,l,r)
Frame:   .res 1         ; Reserve 1 byte to store the framecounter
Seconds: .res 1         ; Reserve 1 butes to store the second counter, increments every 60 frames
BackgroundPtr: .res 2   ; Reserve 2 bytes for a pointer to a background array
BGDrawPointer: .res 2   ; Reserve 2 bytes for the pointer we use to iterate through while drawing
DrawTextPtr: .res 2
DrawTextAsciiCode: .res 1
DrawTextPosPtr: .res 2

; This is how much we are moving per frame
; It is a 16 bit value. We update the low bit
; when the low bit overflows it carrys which
; updates the high bit. The high bit is what
; modulates the sprite position
PlayerXSpeed: .res 2
PlayerYSpeed: .res 2

PlayerFacingRight: .res 1
PlayerState: .res 1

MAXSPEED = 3
ACCEL = 40
DRAG = 20



.segment "CODE"

    .proc ReadButtons
            lda #1
            sta Buttons

            sta CONTROLLER_ONE  ; Controller polls for player input
            lda #0
            sta CONTROLLER_ONE  ; Controller will send that input to the NES

            LoopButtons:
                lda CONTROLLER_ONE  ; Read from the controller. We get a byte, but only the last bit 
                                    ; contains the data we want
                lsr                 ; We shift right to place the first bit we read into the
                                    ; carry flag
                rol Buttons         ; Roll the bits in Buttons left, popping the value from the carry flag 
                                    ; on the end. The carry flag will now we empty unless we reached the
                                    ; end of the input
                                    ; Remember we started with Buttons set to %00000001. That 1 bit we 
                                    ; had will be popped onto the carry flag if we filled Buttons up
                                    ; So, at the end of the rol call carry flag will be empty (not 0)
                                    ; unless we reached the end in which case our initial 1 will be there
                bcc LoopButtons     ; y so we're done
            rts
    .endproc

    .proc ButtonHandler
        CheckA:
            lda Buttons
            and #%10000000
            beq CheckB
            ; Do A Stuff
        CheckB:
            lda Buttons
            and #%01000000
            beq CheckSelect
            ; Do B Stuff
        CheckSelect:
            lda Buttons
            and #%00100000
            beq CheckStart
            ; Do Select Stuff
        CheckStart:
            lda Buttons
            and #%00010000
            beq CheckUp
            ; Do Start Stuff
        CheckUp:
            lda Buttons
            and #%00001000
            beq CheckDown
            ;Do up stuff
        CheckDown:
            lda Buttons
            and #%00000100
            beq CheckLeft
            ; Do Down Stuff
        CheckLeft:
            lda Buttons
            and #%00000010
            beq CheckRight
            lda #FALSE                  
            sta PlayerFacingRight   ; Set PlayerFacingRight to 0
            jsr IncreasePlayerXSpeed
        CheckRight:
            lda Buttons
            and #%00000001
            beq NoButtonPressed
            lda #TRUE                  ; Load a 1
            sta PlayerFacingRight   ; Set PlayerFacingRight to 1
            jsr IncreasePlayerXSpeed
            jmp Exit
        NoButtonPressed:
            lda #0
            sta PlayerXSpeed+1
            sta PlayerXSpeed
        Exit:
        rts
    .endproc 

    .proc IncreasePlayerXSpeed
        lda PlayerXSpeed        ; Load the XSpeed lo-bit
        clc                     ; Clear carry
        adc #ACCEL              ; Add ACCESS to the XSpeed lo-bit
        sta PlayerXSpeed        ; Save the updated lo-bit
        lda PlayerXSpeed+1      ; Load the XSpeed hi bit
        adc #0                  ; Add with Carry the 0 to it, which will 
                                ; increase the hi bit by the carry from the
                                ; lo bit
        cmp #MAXSPEED           ; Compare the new hi-bit valye to MAXSPEED
        bcs Exit                ; If new hi-bit >= MAXSPEED Exit
        sta PlayerXSpeed+1      ; Save the new XSpeed hi-bit
        Exit:
            rts
    .endproc


    .proc UpdatePlayerSpritePosition
        lda PlayerFacingRight
        cmp #1
        bne Left
        Right:
            clc
            ldx #03
            lda OAM_COPY, x
            adc PlayerXSpeed+1
            sta OAM_COPY, x
            clc
            ldx #07
            lda OAM_COPY, x
            adc PlayerXSpeed+1
            sta OAM_COPY, x
            clc
            ldx #11
            lda OAM_COPY, x
            adc PlayerXSpeed+1
            sta OAM_COPY, x
            clc
            ldx #15
            lda OAM_COPY, x
            adc PlayerXSpeed+1
            sta OAM_COPY, x
            jmp Exit
        Left:
            clc
            ldx #03
            lda OAM_COPY, x
            sbc PlayerXSpeed+1
            sta OAM_COPY, x
            clc
            ldx #07
            lda OAM_COPY, x
            sbc PlayerXSpeed+1
            sta OAM_COPY, x
            clc
            ldx #11
            lda OAM_COPY, x
            sbc PlayerXSpeed+1
            sta OAM_COPY, x
            clc
            ldx #15
            lda OAM_COPY, x
            sbc PlayerXSpeed+1
            sta OAM_COPY, x
            jmp Exit
        Exit:
        rts        
    .endproc

    ; This is the player state machine. Player object state is never updated anywhere else
    ; Instead we set states and react to the environment
    .proc PlayerStep
        ; We start every step assuming we're idle
        lda #STATE_IDLE
        sta PlayerState

        ; If PlayerXSpeed > 0 we're walking
        ldx PlayerXSpeed+1
        cpx #0
        beq SkipXSpeed
            lda #STATE_WALKING
            sta PlayerState
        SkipXSpeed:

        Idle:
            ldx PlayerState
            cpx #STATE_IDLE
            bne Walking
            ;Do Idle Stuff
        Walking:
            ldx PlayerState
            cpx #STATE_WALKING
            bne Exit
                ; dec PlayerXSpeed
                lda PlayerFacingRight
                cmp #01
                bne MoveLeft
                MoveRight:
                    ;clc
                    ;ldx #07
                    ;lda OAM_COPY, x
                    ;adc #4
                    ;sta OAM_COPY, x

                    ;clc
                    ;ldx #11
                    ;lda OAM_COPY, x
                    ;adc #4
                    ;sta OAM_COPY, x

                    ;clc
                    ;ldx #15
                    ;lda OAM_COPY, x
                    ;adc #4
                    ;sta OAM_COPY, x

                    jmp EndMove
                MoveLeft:
                    ;left stuff
                    clc
                    ldx #03
                    lda OAM_COPY, x
                    sbc #4
                    sta OAM_COPY, x

                    clc
                    ldx #07
                    lda OAM_COPY, x
                    sbc #4
                    sta OAM_COPY, x

                    clc
                    ldx #11
                    lda OAM_COPY, x
                    sbc #4
                    sta OAM_COPY, x

                    clc
                    ldx #15
                    lda OAM_COPY, x
                    sbc #4
                    sta OAM_COPY, x
                    jmp EndMove
                EndMove:
        Exit:
        clc
        ldx #03
        lda OAM_COPY, x
        adc PlayerXSpeed+1
        sta OAM_COPY, x
        rts
    .endproc

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
        lda #0
        sta Frame
        sta Seconds
        sta BackgroundPtr
        sta BackgroundData
        ; Set background pointer
        SetPointer BackgroundPtr, BackgroundData
        lda #1
        lda #STATE_IDLE
        sta PlayerState
        lda #1
        sta PlayerFacingRight
        lda #0
        sta PlayerXSpeed
        sta PlayerXSpeed+1
        sta PlayerYSpeed
        sta PlayerYSpeed+1

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
        
        jsr ReadButtons
        jsr ButtonHandler
        ;jsr PlayerStep
        jsr UpdatePlayerSpritePosition

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
        .incbin "resources/simple_screen.nam"
        
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
        .byte   $C0,    $00,    %00000000,  $70
        .byte   $C0,    $01,    %00000000,  $78
        .byte   $C8,    $10,    %00000000,  $70
        .byte   $C8,    $11,    %00000000,  $78


;Load CHAR_ROM pattern tables
.segment "CHARS"
.incbin "resources/hatman.chr"

; $FFFA
.segment "VECTORS"
    ;On boot the NES jumps to $FFFA to find the addresses for the handlers
    ;This is called the VECTORS 
    .word NMI   ; address of the NMI handler
    .word Reset ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
