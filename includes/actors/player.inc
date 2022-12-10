    ; X must be set to the x offset to check in pixels
    ; Y must be set to the y offset to check in pixels
    ; result will be placed into A as 1 or 0 
    .proc Player_TestTileCollision
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
            CopyPointer BackgroundPtr, TileLookupPointer    ; Make a copy of the global Background Pointer
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
    .proc Player_HandleHorizTileCollission
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
            jsr Player_TestTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta PlayerXSpeed        
            rts
        Left:
            ldx #254
            ldy #8
            jsr Player_TestTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta PlayerXSpeed        
            rts
        Exit:
        rts
    .endproc

    .proc Player_UpdateSpritePositionHorizontal
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


    ; Called every frame. Because of gravity we will always either 
    ; be moving up or down. Collisions stop Y axis movement
    .proc Player_HandleVerticalTileCollission

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
            ldy #18             ; Sprite height is initial offset
            jsr Player_TestTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta Gravity                     ; We hit a tile below. Set Gravity to 0
            lda #TRUE
            sta PlayerIsOnTheGround    
            jmp Player_CorrectPositionAfterFall
            rts
        Up:
            ldx #8
            ldy #254
            jsr Player_TestTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            sta PlayerYSpeed                ; We git a tile above. Set PlayerYSpeed to 0       
            rts
        Exit:
        rts
    .endproc

    ; After falling, the player may end up in a floor tile
    ; This is because the fall amount is more than 1px, so we may
    ; not detect the tile collision until we've already passed through it some
    ;
    ; This method corrects that by looping a look 1 px below the player, and 
    ; if it is found move the player's position up one px, until eventually we 
    ; don't detect a tile anymore
    ;
    ; This is called in Player_HandleVerticalTileCollission and is called 
    ; after detecting a collision
    .proc Player_CorrectPositionAfterFall
        Backoff:
        ldx #8
        ldy #17             ; Sprite height is initial offset
        jsr Player_TestTileCollision
        cmp #FALSE
        beq ExitBackoff
            dec OAM_COPY
            jmp Backoff
        ExitBackoff:
        lda OAM_COPY
        ldx #04
        sta OAM_COPY, x
        clc
        adc #08
        ldx #08
        sta OAM_COPY, x
        ldx #12
        sta OAM_COPY, x
        rts
    .endproc

    ; This method moves the plater in accordance with both
    ; Gravity and the player's upward speed (jump)
    .proc Player_UpdateSpritePositionVertical
        ; This is a debounce
        ; Because we don't have subframe movement we instead debounce frames
        ; and only move on some frames. This simulates subframe movement
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

        ; When jumping we want to reduce the player's upward Y speed 
        ; by 1 every frame. This gives us a nice natural feeling arc
        jsr ReducePlayerYSpeedIfGreaterThanZero
        
        Exit:
        rts
    .endproc

    .proc ReducePlayerYSpeedIfGreaterThanZero
        lda PlayerYSpeed
        cmp #0
        beq Exit
            dec PlayerYSpeed 
        Exit:
        rts
    .endproc

    ; Player movement is all handled by mutating OAM COPY directly
    ; At the end of the logic cycle we need to sync that back to the
    ; actor struct
    .proc Player_SyncActorPosition
        lda OAM_COPY
        sta CurrentActor+Actor::yPos
        ldx #3
        lda OAM_COPY, x
        sta CurrentActor+Actor::xPos
        rts
    .endproc

    .proc Player_Animate


    .endproc


    ; This is the player state machine. Player object state is never updated anywhere else
    ; Instead we set states and react to the environment
    .proc Actor_Player_Logic
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
        bne ExitAButtonHandler
            lda PlayerIsOnTheGround
            cmp #TRUE
            bne ExitAButtonHandler
                lda #10
                sta PlayerYSpeed 
        ExitAButtonHandler:

        ; Because of the constant downward force of gravity we are always potentially
        ; moving on the Y axis
        jsr Player_HandleVerticalTileCollission
        jsr Player_UpdateSpritePositionVertical

        Idle:
            ldx PlayerState
            cpx #STATE_IDLE
            bne Walking
            ;Do Idle Stuff
        Walking:
            ldx PlayerState
            cpx #STATE_WALKING
            bne Exit
            jsr Player_HandleHorizTileCollission
            jsr Player_UpdateSpritePositionHorizontal
        Falling:
        Jumping:
        Exit:
        jsr Player_SyncActorPosition
        rts
    .endproc

.proc Actor_Player_Draw
    ldx CurrentActor+Actor::oamOffset

    ; Tile one
    lda CurrentActor+Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$0
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda CurrentActor+Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile Two
    lda CurrentActor+Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$1
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda CurrentActor+Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    ; Tile three
    lda CurrentActor+Actor::yPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    lda #$10
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda CurrentActor+Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile four
    lda CurrentActor+Actor::yPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    lda #$11
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda CurrentActor+Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    rts
.endproc

.proc Actor_Player_SpriteCollision
    rts
.endproc


;Player_SpriteData:
    ;       Y       Tile    Attrs       X
;    .byte   $80,    $00,    %00000000,  $70
;    .byte   $80,    $01,    %00000000,  $78
;    .byte   $88,    $10,    %00000000,  $70
;    .byte   $88,    $11,    %00000000,  $78