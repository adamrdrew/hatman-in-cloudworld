; Let's learn to load nametables and display background graphics

.include "../includes/constants.inc"
.include "../includes/header.inc"
.include "../includes/init.inc"
.include "../includes/utils.inc"

.segment "ZEROPAGE"
DebugByte: .res 1

Buttons: .res 1         ; Reserve one by to represent button state (a,b,sel,sta,u,d,l,r)
Frame:   .res 1         ; Reserve 1 byte to store the framecounter
Seconds: .res 1         ; Reserve 1 butes to store the second counter, increments every 60 frames
BackgroundPtr: .res 2   ; Reserve 2 bytes for a pointer to a background array
BGDrawPointer: .res 2   ; Reserve 2 bytes for the pointer we use to iterate through while drawing
DrawTextPtr: .res 2
DrawTextAsciiCode: .res 1
DrawTextPosPtr: .res 2

; 0 if left, 1 if right
PlayerFacingRight: .res 1
PlayerMovingDown: .res 1
; 0 if not pressed, 1 if pressed
DPADPressed: .res 1
AButtonPressed: .res 1
; See constants
PlayerState: .res 1

PlayerXSpeed: .res 1
; Player upward movement (jump)
PlayerYSpeed: .res 1

; Contant downward pressure
Gravity: .res 1

PlayerXCollisionPoint: .res 1
PlayerYCollisionPoint: .res 1
PlayerXCollisionTileX: .res 1
PlayerYCollisionTileY: .res 1

PlayerIsOnTheGround: .res 1

TileLookupPointer: .res 2

TileCollisionLookupXOffset: .res 1
TileCollisionLookupYOffset: .res 1


; The change in position per frame
MAX_Y_SPEED = 3

.segment "CODE"

    ;Called every NMI to read the buttons
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

    ; X must be set to the x offset to check in pixels
    ; Y must be set to the y offset to check in pixels
    ; result will be placed into A as 1 or 0 
    .proc TestPlayerTileCollision
            stx TileCollisionLookupXOffset      ; The x offset from PlayerX where we'll look for a tile
            sty TileCollisionLookupYOffset      ; The y offset from PlayerY where we'll look for a tile

            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            ;; Get coords in pixel-space we'll look for collision
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            ldx #03
            lda OAM_COPY, x                     ; Get the player's X coord. This will be the 3rd byte in from OAM_COPY
            clc
            adc TileCollisionLookupXOffset      ; Add the offset to the player's X coord
            sta PlayerXCollisionPoint           ; Store it in a variable

            lda OAM_COPY                        ; We do the same thing here for Y
            clc                                 ; Only difference is player Y is first byte in OAM_COPY
            adc TileCollisionLookupYOffset
            sta PlayerYCollisionPoint

            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            ;; Convert pixel-space coords to tilemap coords
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            lda PlayerXCollisionPoint           ; X coord in pixel space we want to check for collision
            lsr                                 ; logical shift 3 times to divide by 32
            lsr                                 ; We divide by 32 because there are 32 tiles per row
            lsr
            sta PlayerXCollisionTileX           ; Store our tilemap-space x coord

            lda PlayerYCollisionPoint           ; Do exactly the same thing for y
            lsr                                 ; We cheat a little because there are only 30 rows 
            lsr                                 ; but 32 will still give us the right answer
            lsr
            sta PlayerYCollisionTileY

            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            ;; Find the memory address of the row that the tile we're looking for is in
            ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
            ldy #0                                          ; We're going to count up from 0 to PlayerYCollisionTileY
            SetPointer TileLookupPointer, BackgroundData    ; Create a pointer with the address of the background data
            FindYOffset:
                cpy PlayerYCollisionTileY
                beq ExitLoop                                ; If we've reached PlayerYCollisionTileY exit the loop
                lda TileLookupPointer                       ; Store the lo bit of the tile address for our pointer
                clc                                         
                adc #32                                     ; Add 32 bytes to the pointer address to get the address of the next row
                sta TileLookupPointer
                lda TileLookupPointer+1                     ; We we need to handle carrying by adding 0 to the hi-byte 
                adc #0                                      ; of the pointer address. Notice we don't clear carry
                sta TileLookupPointer+1                     ; This will ensure our hi-byte handle the lo-byte increment correctly
                iny                                         ; Incremenet our iterator and move to the next iteration
                jmp FindYOffset
            ExitLoop:
           
            ldy PlayerXCollisionTileX                       ; Store the tile X to use as an offset. 
            lda (TileLookupPointer), y                      ; Get the byte that represents our tile                
            cmp #$E0                                        ; We want to know if the tile ID is >= $E0
            bcc Exit                                        ; We treat all tiles >= $E0 as solid
                lda #1                                      ; It is, so return TRUE
                rts
            Exit:
            lda #0                                          ; It isn't, so return false
            rts
    .endproc

    ; Called whenever the player is moving horizontally
    ; We handle collission by point checking in front of us
    ; for a solid tile and then killing PlayerXSpeed if it exists
    .proc HandleHorizTileCollission
        ; Set the default X speed
        ; If no collission occurs this is what we want
        ; to be applied
        lda #2
        sta PlayerXSpeed

        lda PlayerFacingRight
        cmp #TRUE
        bne Left
        Right:
            ldx #17
            ldy #8
            jsr TestPlayerTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta PlayerXSpeed        
            rts
        Left:
            ldx #254
            ldy #8
            jsr TestPlayerTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta PlayerXSpeed        
            rts
        Exit:
        rts
    .endproc


    ; Called every frame. Because of gravity we will always either 
    ; be moving up or down. Collisions stop Y axis movement
    .proc HandleVerticalTileCollission

        ; By default gravity exerts a contant downward pressure
        ; of 1 pixel per frame
        ; The player's movement in the Y dimension is (PlayerY + Gravity) - PlayerYMovement
        ; If a collision above is detected PlayerYMovement is set to 0
        ; If a collision below is detected Gravity is set to 0
        lda #02
        sta Gravity

        ; Our default assumption is that the player is not on the ground
        ; We need a gravity directed colission to tell us we are on the ground
        lda #FALSE
        sta PlayerIsOnTheGround

        lda PlayerYSpeed                    ; Get the PlayerYSpeed 
        cmp Gravity                         ; Compare it to gravity
        bcs Up                              ; If PlayerYSpeed is great than Gravity we're going up
        Down:
            ldx #8
            ldy #17
            jsr TestPlayerTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta Gravity                     ; We hit a tile below. Set Gravity to 0
            lda #TRUE
            sta PlayerIsOnTheGround    
            rts
        Up:
            ldx #8
            ldy #254
            jsr TestPlayerTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta PlayerYSpeed                ; We git a tile above. Set PlayerYSpeed to 0       
            rts
        Exit:
        rts
    .endproc


    ; Called every NMI to handle the buttons
    .proc ButtonHandler
        lda #FALSE
        sta DPADPressed
        sta AButtonPressed

        CheckA:
            lda Buttons
            and #%10000000
            beq CheckB
            lda #TRUE
            sta AButtonPressed
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
            lda #$AA
            sta DebugByte
            lda #FALSE                  
            sta PlayerFacingRight
            lda #TRUE
            sta DPADPressed
        CheckRight:
            lda Buttons
            and #%00000001
            beq Exit
            lda #$BB
            sta DebugByte
            lda #TRUE            
            sta PlayerFacingRight
            lda #TRUE
            sta DPADPressed
        Exit:
        rts
    .endproc 

    .proc UpdatePlayerSpritePositionHorizontal
        lda Frame
        cmp #2
        bcc Exit

        lda PlayerFacingRight
        cmp #TRUE
        bne Left
        Right:
            clc
            ldx #03
            lda OAM_COPY, x
            adc PlayerXSpeed
            sta OAM_COPY, x
            clc
            ldx #07
            lda OAM_COPY, x
            adc PlayerXSpeed
            sta OAM_COPY, x
            clc
            ldx #11
            lda OAM_COPY, x
            adc PlayerXSpeed
            sta OAM_COPY, x
            clc
            ldx #15
            lda OAM_COPY, x
            adc PlayerXSpeed
            sta OAM_COPY, x
            jmp Exit
        Left:
            sec
            ldx #03
            lda OAM_COPY, x
            sbc PlayerXSpeed
            sta OAM_COPY, x
            sec
            ldx #07
            lda OAM_COPY, x
            sbc PlayerXSpeed
            sta OAM_COPY, x
            sec
            ldx #11
            lda OAM_COPY, x
            sbc PlayerXSpeed
            sta OAM_COPY, x
            sec
            ldx #15
            lda OAM_COPY, x
            sbc PlayerXSpeed
            sta OAM_COPY, x
            jmp Exit
        Exit:
        rts        
    .endproc

    .proc UpdatePlayerSpritePositionVertical
            lda Frame
            cmp #2
            bcc Exit

            clc
            lda OAM_COPY
            adc Gravity
            sec
            sbc PlayerYSpeed
            sta OAM_COPY

            clc
            ldx #04
            lda OAM_COPY, x
            adc Gravity
            sec
            sbc PlayerYSpeed
            sta OAM_COPY, x

            clc
            ldx #8
            lda OAM_COPY, x
            adc Gravity
            sec
            sbc PlayerYSpeed
            sta OAM_COPY, x

            clc
            ldx #12
            lda OAM_COPY, x
            adc Gravity
            sec
            sbc PlayerYSpeed
            sta OAM_COPY, x

            lda PlayerYSpeed
            cmp #0
            beq Exit
                dec PlayerYSpeed 
   
            Exit:
            rts
    .endproc

    ; This is the player state machine. Player object state is never updated anywhere else
    ; Instead we set states and react to the environment
    .proc PlayerStep
        ; We start every step assuming we're idle
        lda #STATE_IDLE
        sta PlayerState

        ; If DPAD is pressed we're walking
        lda DPADPressed
        cmp #TRUE
        bne DontSetWalkState
            lda #STATE_WALKING
            sta PlayerState
        DontSetWalkState:

        lda AButtonPressed
        cmp #TRUE
        bne PlayerCannotJump
            lda PlayerIsOnTheGround
            cmp #TRUE
            bne PlayerCannotJump
                lda #10
                sta PlayerYSpeed 




        PlayerCannotJump:
        ; Because of the constant downward force of gravity we are always potentially
        ; moving on the Y axis
        jsr HandleVerticalTileCollission
        jsr UpdatePlayerSpritePositionVertical

        Idle:
            ldx PlayerState
            cpx #STATE_IDLE
            bne Walking
            ;Do Idle Stuff
        Walking:
            ldx PlayerState
            cpx #STATE_WALKING
            bne Exit
            jsr HandleHorizTileCollission
            jsr UpdatePlayerSpritePositionHorizontal
        Falling:
        Jumping:
        Exit:
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
        lda #STATE_IDLE
        sta PlayerState
        lda #TRUE
        sta PlayerFacingRight
        lda #FALSE
        sta DPADPressed
        sta AButtonPressed
        lda #1
        sta Gravity
        lda #0
        sta PlayerYSpeed
        sta PlayerXSpeed
        sta PlayerIsOnTheGround

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
        jsr PlayerStep


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
        .incbin "resources/lvl1.nam"
        
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
.incbin "resources/cloudworld.chr"

; $FFFA
.segment "VECTORS"
    ;On boot the NES jumps to $FFFA to find the addresses for the handlers
    ;This is called the VECTORS 
    .word NMI   ; address of the NMI handler
    .word Reset ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
