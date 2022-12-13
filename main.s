.include "includes/header.inc"

.include "includes/variables.inc"


.segment "CODE"
    .include "includes/constants.inc"
    .include "includes/init.inc"
    .include "includes/utils.inc"
    .include "includes/controller.inc"
    .include "includes/actors/actors.inc"

    ; Levels
    .include "includes/levels/0_titlescreen.inc"
    .include "includes/levels/between.inc"
    .include "includes/levels/1_level.inc"
    .include "includes/levels/2_level.inc"
    
    ; Audio
    .include "lib/famistudio.s"
    .include "audio/sfx.s"
    .include "audio/titlescreen.s"
    .include "audio/song1.s"
    .include "audio/song2.s"

    .proc DrawSprites
        ; Copy sprite data to OAM
        lda #$02            ; Where OAM should start copying from $02 means it will copy from $0200-02FF
        sta OAM_DMA_COPY    ; Send the source address to initial DMA copy
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
           lda (PalettePointer),y
           ; ppu_data_write(x)
           sta PPU_DATA
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
                cpy #255 ; Keep iterating until we've read all 255 bytes in the chunk
                bne ReadBytesInChunk
            inc BGDrawPointer+1 ;Increate the hi-byte in the pointer address by one, which will increase our offset by 255
            inx ; increase x
            cpx #4 ; iterate for 4 chunks (4 chunks of 255b = 1kb)
            bne GetChunk
        rts 
    .endproc

    .proc LoadLevel
        ldx #0
        stx $2000       ; Disable NMI
        stx $2001       ; Disable rendering
        stx $4010       ; Disable DMC IRQs
        bit PPU_STATUS       ; Clear the VBlank flag as we don't know its state on boot
        lda Level
        cmp #0
        bne :+
           ; SetPointer DrawTextPtr, NoText
           ; SetPointer DrawTextPosPtr, $2020
            SetPointer BackgroundPtr, TitleScreen_BackgroundData
            SetPointer LevelActorDataPointer, TitleScreen_ActorData
            SetPointer PalettePointer, TitleScreen_PaletteData
            lda #1
            ldx #<music_data_titlescreen
            ldy #>music_data_titlescreen
            jsr famistudio_init
        :
        cmp #1
        bne :+
            ;SetPointer DrawTextPtr, NoText
           ; SetPointer DrawTextPosPtr, $2020
            SetPointer BackgroundPtr, LevelOne_BackgroundData
            SetPointer LevelActorDataPointer, LevelOne_ActorData
            SetPointer PalettePointer, LevelOne_PaletteData
            lda #1
            ldx #<music_data_song_1
            ldy #>music_data_song_1
            jsr famistudio_init
        :
        cmp #2
        bne :+
            SetPointer BackgroundPtr, Between_BackgroundData
            SetPointer LevelActorDataPointer, Between_ActorData
            SetPointer PalettePointer, Between_PaletteData
            lda #1
            ldx #<music_data_titlescreen
            ldy #>music_data_titlescreen
            jsr famistudio_init
        :
        cmp #3
        bne :+
            SetPointer BackgroundPtr, LevelTwo_BackgroundData
            SetPointer LevelActorDataPointer, LevelTwo_ActorData
            SetPointer PalettePointer, LevelTwo_PaletteData
            lda #1
            ldx #<music_data_song_2
            ldy #>music_data_song_2
            jsr famistudio_init
        :
        jsr Actor_ClearAll
        jsr Actor_ClearOAMCache
        jsr LoadPalettes
        jsr LoadNametable
        jsr Actor_LoadLevelActorData
        jsr Actor_LoadSpriteData


        ;jsr ShowText


        lda #%10010000  ; Enable NMI interrupts from PPU. Set BG to use 2nd pattern table
        sta PPU_CTRL
        ; Disable PPU Scrolling. This is customary to do when drawing to prevent errant scrolling
        ; PPU_SCROLL is also a latch address, so we need to set it twice
        lda #0
        sta PPU_SCROLL ; X Scrolling
        sta PPU_SCROLL ; Y Scrolling
        lda #%00011110
        sta PPU_MASK    ; Setting the mask ensures we show the background

        lda #0
        jsr famistudio_music_play

        rts
    .endproc

    ; Code of PRG ROM
    Reset:
        INIT_NES
        INIT_VARIABLES
  
        lda #1
        ldx #<sounds
        ldy #>sounds
        jsr famistudio_sfx_init

        lda #0
        sta Level
        jsr LoadLevel



    GameLoop:
        jsr Controller_ReadButtons
        jsr Controller_ButtonHandler
        jsr Actor_RunAll
        jsr Actor_CheckCollisions
        

        WaitForVBlank:
            lda IsDrawComplete
            cmp #FALSE
            beq WaitForVBlank
        
        lda #FALSE
        sta IsDrawComplete
        jsr famistudio_update
        jmp GameLoop

    NMI:
        ; This tripped me up for a while
        ; We can get our registers clobb
        NMI_CacheRegisters
        lda Coins
        cmp #4
        bne :+
            lda #0
            sta Coins
            sta Frame
            inc Level
            jsr LoadLevel
            NMI_FetchCachedRegisters
            ; We bail after drawing the tiles because I'm not sure we have enough time
            ; to also draw the sprites. Probably, but why risk it. Full screen draws only
            ; happen at level transitions, so why not. That said we don't set the isDrawComplete
            ; flag because we don't want anything to happen for 1 frame
            rti
        :

        ; This is our main draw code
        ; In our game all we draw are sprites, which
        ; is handled by a DMA copy from our OAM_COPY 
        ; to OAM on the PPU. We don't scroll or mess with tiles
        ; so this is really it (except for text which we need to add)
        jsr DrawSprites
        ManageTime:
            inc Frame
            lda Frame
            cmp #60
            bne :+
                lda #0
                sta Frame
            :

        SkipDraw:
            lda #TRUE
            inc IsDrawComplete
            NMI_FetchCachedRegisters
        rti ; Return from Interrupt
    
    IRQ:
        rti ; Return from Interrupt

    NoText:
        .byte $0
    TextMessage:
        .byte "LIVES 03", $0
    FromAdamMessage:
        .byte "SCORE ", $0

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
