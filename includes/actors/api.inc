; This is the Actor API
; A collection of functions that all Actors can use at run time to do things
; like movement and collisions
;
; 


; X must be set to the x offset to check in pixels
; Y must be set to the y offset to check in pixels
; result will be placed into A as 1 or 0 
.proc Actor_TestTileCollision
        stx TileCollisionLookupXOffset      ; The x offset from ActorX where we'll look for a tile
        sty TileCollisionLookupYOffset      ; The y offset from ActorY where we'll look for a tile

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Get coords in pixel-space we'll look for collision
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;ldx #03
        lda_CurrentActor Actor::xPos                     ; Get the actor's X coord. This will be the 3rd byte in from OAM_COPY
        clc
        adc TileCollisionLookupXOffset      ; Add the offset to the actor's X coord
        sta ActorXCollisionPoint           ; Store it in a variable

        lda_CurrentActor Actor::yPos                         ; We do the same thing here for Y
        clc                                 ; Only difference is actor Y is first byte in OAM_COPY
        adc TileCollisionLookupYOffset
        sta ActorYCollisionPoint

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Convert pixel-space coords to tilemap coords
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        lda ActorXCollisionPoint           ; X coord in pixel space we want to check for collision
        lsr                                 ; logical shift 3 times to divide by 32
        lsr                                 ; We divide by 32 because there are 32 tiles per row
        lsr
        sta ActorXCollisionTileX           ; Store our tilemap-space x coord

        lda ActorYCollisionPoint           ; Do exactly the same thing for y
        lsr                                 ; We cheat a little because there are only 30 rows 
        lsr                                 ; but 32 will still give us the right answer
        lsr
        sta ActorYCollisionTileY

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ;; Find the memory address of the row that the tile we're looking for is in
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ldy #0                                          ; We're going to count up from 0 to ActorYCollisionTileY
        CopyPointer BackgroundPtr, TileLookupPointer    ; Make a copy of the global Background Pointer
        FindYOffset:
            cpy ActorYCollisionTileY
            beq ExitLoop                                ; If we've reached ActorYCollisionTileY exit the loop
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
        
        ldy ActorXCollisionTileX                       ; Store the tile X to use as an offset. 
        lda (TileLookupPointer), y                      ; Get the byte that represents our tile                
        cmp #$E0                                        ; We want to know if the tile ID is >= $E0
        bcc Exit                                        ; We treat all tiles >= $E0 as solid
            lda #1                                      ; It is, so return TRUE
            rts
        Exit:
        lda #0                                          ; It isn't, so return false
        rts
.endproc

; Called whenever the actor is moving horizontally
; We handle collission by point checking in front of us
; for a solid tile and then killing actor xSpeed if it exists
.proc Actor_HandleHorizTileCollission
    ; Set the default X speed
    ; If no collission occurs this is what we want
    ; to be applied
    lda #2
    sta_CurrentActor Actor::xSpeed 

    lda PlayerFacingRight
    cmp #TRUE
    bne Left
    Right:
        ldx #17
        ldy #8
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        lda #0
        sta_CurrentActor Actor::xSpeed    
        rts
    Left:
        ldx #254
        ldy #8
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        lda #0
        sta_CurrentActor Actor::xSpeed        
        rts
    Exit:
    rts
.endproc

.proc Actor_UpdateSpritePositionHorizontal
    lda Frame
    cmp #2
    bcc Exit

    lda_CurrentActor Actor::xSpeed
    sta ActorXSpeedCache

    lda PlayerFacingRight
    cmp #TRUE
    bne Left
    Right:
        lda #TRUE
        sta_CurrentActor Actor::facingRight
        clc
        lda_CurrentActor Actor::xPos
        adc ActorXSpeedCache
        sta_CurrentActor Actor::xPos
        jmp Exit
    Left:
        lda #FALSE
        sta_CurrentActor Actor::facingRight
        sec
        lda_CurrentActor Actor::xPos
        sbc ActorXSpeedCache
        sta_CurrentActor Actor::xPos
    Exit:
    rts        
.endproc


; Called every frame. Because of gravity we will always either 
; be moving up or down. Collisions stop Y axis movement
.proc Actor_HandleVerticalTileCollission

    ; By default gravity exerts a contant downward pressure
    ; of 1 pixel per frame
    ; The actor's movement in the Y dimension is (PlayerY + ActorGravityCache) - PlayerYMovement
    ; If a collision above is detected PlayerYMovement is set to 0
    ; If a collision below is detected ActorGravityCache is set to 0
    lda #02
    ; Set the default gravity on the actor gravity property
    sta_CurrentActor Actor::gravity
    ; Also load it into the global gravity swap space
    ; Every actor uses this swap space
    ; We only do this because we can't compare registers against eachother
    ; so we need some place to compare to
    sta ActorGravityCache

    ; Our default assumption is that the actor is not on the ground
    ; We need a gravity directed colission to tell us we are on the ground
    lda #FALSE
    sta_CurrentActor Actor::isOnGround

    lda_CurrentActor Actor::ySpeed  ; Get the ySpeed 
    cmp ActorGravityCache           ; Compare it to gravity
    bcs Up                          ; If ySpeed is great than ActorGravityCache we're going up
    Down:
        ldx #8
        ldy #18                     ; Sprite height is initial offset
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        lda #0
        sta_CurrentActor Actor::gravity ; We hit a tile below. Set gravity to 0
        lda #TRUE
        sta_CurrentActor Actor::isOnGround   
        jmp Actor_CorrectPositionAfterFall
        rts
    Up:
        ldx #8
        ldy #254
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        lda #0
        sta_CurrentActor Actor::ySpeed  ; We git a tile above. Set ySpeed to 0       
        rts
    Exit:
    rts
.endproc

; After falling, the actor may end up in a floor tile
; This is because the fall amount is more than 1px, so we may
; not detect the tile collision until we've already passed through it some
;
; This method corrects that by looping a look 1 px below the actor, and 
; if it is found move the actor's position up one px, until eventually we 
; don't detect a tile anymore
;
; This is called in Player_HandleVerticalTileCollission and is called 
; after detecting a collision
.proc Actor_CorrectPositionAfterFall
    Backoff:
    ldx #8
    ldy #17             ; Sprite height is initial offset
    jsr Actor_TestTileCollision
    cmp #FALSE
    beq ExitBackoff
        lda_CurrentActor Actor::yPos
        sec
        sbc #1
        sta_CurrentActor Actor::yPos
        jmp Backoff
    ExitBackoff:
    rts
.endproc

; This method moves the plater in accordance with both
; ActorGravityCache and the actor's upward speed (jump)
.proc Actor_UpdateSpritePositionVertical
    ; This is a debounce
    ; Because we don't have subframe movement we instead debounce frames
    ; and only move on some frames. This simulates subframe movement
    lda Frame
    cmp #2
    bcc Exit

    ; Load our gravity
    lda_CurrentActor Actor::gravity
    ; Store it in gravity swap
    sta ActorGravityCache

    ; Load the ySpeed and cache it in memory
    lda_CurrentActor Actor::ySpeed
    sta ActorYSpeedCache

    ; Get our current position
    lda_CurrentActor Actor::yPos
    clc
    ; add gravity 
    adc ActorGravityCache
    sec
    ; substract y speed
    sbc ActorYSpeedCache
    ; Update our position
    sta_CurrentActor Actor::yPos

    ; When jumping we want to reduce the actor's upward Y speed 
    ; by 1 every frame. This gives us a nice natural feeling arc
    jsr Actor_ReduceYSpeedIfGreaterThanZero
    
    Exit:
    rts
.endproc

.proc Actor_ReduceYSpeedIfGreaterThanZero
    lda_CurrentActor Actor::ySpeed
    cmp #0
    beq Exit
        tax
        dex
        txa
        sta_CurrentActor Actor::ySpeed
    Exit:
    rts
.endproc