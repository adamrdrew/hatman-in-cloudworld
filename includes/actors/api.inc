; This is the Actor API
; A collection of functions that all Actors can use at run time to do things
; like movement and collisions
;
 
; This table maps a pixel to the tile that would contain it
; there are 32 possible pixel values here, meaning it is usable 
; for either horixontal (32 wide) or vertical (30 tall) dimensions
; Simply look at the offset in the table that matches your pixel
; and read the value and that will be the tile ID.
; For example
PixelSpaceToTileSpaceLookupTable:
.byte $00, $00, $00, $00, $00, $00, $00, $00    
.byte $01, $01, $01, $01, $01, $01, $01, $01    
.byte $02, $02, $02, $02, $02, $02, $02, $02
.byte $03, $03, $03, $03, $03, $03, $03, $03
.byte $04, $04, $04, $04, $04, $04, $04, $04
.byte $05, $05, $05, $05, $05, $05, $05, $05
.byte $06, $06, $06, $06, $06, $06, $06, $06
.byte $07, $07, $07, $07, $07, $07, $07, $07
.byte $08, $08, $08, $08, $08, $08, $08, $08
.byte $09, $09, $09, $09, $09, $09, $09, $09
.byte $0A, $0A, $0A, $0A, $0A, $0A, $0A, $0A
.byte $0B, $0B, $0B, $0B, $0B, $0B, $0B, $0B
.byte $0C, $0C, $0C, $0C, $0C, $0C, $0C, $0C
.byte $0D, $0D, $0D, $0D, $0D, $0D, $0D, $0D
.byte $0E, $0E, $0E, $0E, $0E, $0E, $0E, $0E
.byte $0F, $0F, $0F, $0F, $0F, $0F, $0F, $0F
.byte $10, $10, $10, $10, $10, $10, $10, $10
.byte $11, $11, $11, $11, $11, $11, $11, $11
.byte $12, $12, $12, $12, $12, $12, $12, $12
.byte $13, $13, $13, $13, $13, $13, $13, $13  
.byte $14, $14, $14, $14, $14, $14, $14, $14
.byte $15, $15, $15, $15, $15, $15, $15, $15
.byte $16, $16, $16, $16, $16, $16, $16, $16
.byte $17, $17, $17, $17, $17, $17, $17, $17
.byte $18, $18, $18, $18, $18, $18, $18, $18
.byte $19, $19, $19, $19, $19, $19, $19, $19
.byte $1A, $1A, $1A, $1A, $1A, $1A, $1A, $1A
.byte $1B, $1B, $1B, $1B, $1B, $1B, $1B, $1B
.byte $1C, $1C, $1C, $1C, $1C, $1C, $1C, $1C
.byte $1D, $1D, $1D, $1D, $1D, $1D, $1D, $1D
.byte $1E, $1E, $1E, $1E, $1E, $1E, $1E, $1E
.byte $1F, $1F, $1F, $1F, $1F, $1F, $1F, $1F
.byte $20, $20, $20, $20, $20, $20, $20, $20

TileCollisionYLoBytes:
.byte $00, $20, $40, $60, $80, $A0, $C0, $E0
.byte $00, $20, $40, $60, $80, $A0, $C0, $E0
.byte $00, $20, $40, $60, $80, $A0, $C0, $E0
.byte $00, $20, $40, $60, $80, $A0, $C0, $E0

TileCollisionYHiBytes:
.byte $00, $00, $00, $00,  $00, $00, $00, $00
.byte $01, $01, $01, $01,  $01, $01, $01, $01
.byte $02, $02, $02, $02,  $02, $02, $02, $02
.byte $03, $03, $03, $03,  $03, $03, $03, $03

; This is the actor to tile collision checking code 
; it is well documented because it is tricky and I don't want to 
; forget how it works!
;
; args:
; X must be set to the x offset to check in pixels
; Y must be set to the y offset to check in pixels
; result will be placed into A as 1 or 0 
.proc Actor_TestTileCollision
        ; How many pixels away from the Actor's x and y we want to check for a collision at
        stx TileCollision_PixelOffsetX      
        sty TileCollision_PixelOffsetY      


        ; Our first step is to add the pixel offsets to the Actor's x and y so we have the 
        ; coordinates that we want to check for the collision at 
        lda_CurrentActor Actor::xPos                
        clc
        ; Add the Actor::xPos to the offset
        adc TileCollision_PixelOffsetX
        ; X coordinate in pixel space we want to check for a tile collision
        sta TileCollision_PixelSpaceX         

        lda_CurrentActor Actor::yPos
        clc
        ; Add the Actor::yPos to the offset                           
        adc TileCollision_PixelOffsetY
        ; Y coordinate in pixel space we want to check for a tile collision
        sta TileCollision_PixelSpaceY


        ; Our next step is to convert the pixel space coordinates to tile space coordinates
        ; this is done via a lookup table to save time
        ; The screen is 224x220 pixels, 32x30 tiles, with 8x8 pixels per tile
        ; Offsets into the table represent pixels. So if your pixel is 110 you look
        ; 110 bytes into the table. The value you read at that offset is the tile coordinate that
        ; corresponds to that pixel.
        ; So tile coordinate TileN = PixelToTileLookupTable, PixelN

        ; Take the y position in pixel space
        ldy TileCollision_PixelSpaceY
        ; Find TileY = PixelToTileLookupTable, PixelY
        lda PixelSpaceToTileSpaceLookupTable, y
        ; Store the tile space Y coordinate for the pixel
        sta TileCollision_TileSpaceY

        ; Do the same thing for X
        ldy TileCollision_PixelSpaceX
        ; TileX = PixelToTileLookupTable, PixelX
        lda PixelSpaceToTileSpaceLookupTable, y
        ; Store the tile space X coordinate for the pixel
        sta TileCollision_TileSpaceX

        ; Our final task to to use our tile space coordinates to look into the nametable
        ; and find the tile that resides there, so we can determine if it has collision
        ; properties
        
        ; First grab a copy of the background pointer. We do this because we are 
        ; going to mutate the pointer address to find our colliding tile, so
        ; we want to work on a copy
        CopyPointer BackgroundPtr, TileCollision_NametablePointer
        
        ; The problem we face here is that the nametable is 930 bytes, so
        ; we can't simply multiply TileSpace_X x TileSpace_Y and get the offset
        ; as the result wouldn't fit in a 8 bit value. However, we can get tricky
        ; and do 16 bit math with the help of 2 more lookup tables in order to
        ; change the address the pointer is pointing to from the start of the 
        ; nametable to our colliding tile

        ; Grab our tilespace Y coordinate
        ; This represents what row we are on in the nametable
        ; each row contains 32 tiles
        ; meaning each row represents an offset of TileSpace Y * 32
        ; Y pos 20 for example would be offset 20 * 32 = 640
        ; But we can't store 640 in a byte!
        ; True, but we can perform 16 bit math if we do it in 2 steps
        ;   1. Add multiples of 255 to the high byte
        ;   2. Add multiples of 32 to the lo byte
        ; For example, if we want to find the Y offset into the nametable for
        ; Y (row) 20 we can find the pixel offset of 640 by doing the following
        ;   1. Look up in TileCollisionYHiBytes, TileY and find out we need to add 2 x 255 or $02 to the hi byte
        ;   2. Look up in TileCollisionYLoBytes, TileY and find out we need to add $80 or 128 to the low byte
        ; The result means we add $0280 to the pointer ($80 to the lo byte and $02 to the high byte) to
        ; achieve an offset by a 16 bit value!
        ldy TileCollision_TileSpaceY 
        clc
        lda TileCollision_NametablePointer
        ; Find our offset for multiples of 32 and move the pointer by that much
        adc TileCollisionYLoBytes, y
        sta TileCollision_NametablePointer

        lda TileCollision_NametablePointer+1
        adc #0
        ; Find our offset for multiples of 255 and move our pointer by that much
        adc TileCollisionYHiBytes, y
        sta TileCollision_NametablePointer+1

        ; OK, we have now moved our pointer into the nametable to the position of the start 
        ; of the row the tile we want is on. Now we just need to add the tile x position to the pointer
        ; to get the rest of the way. The X position just represents a simple 8 bit offset so no need for
        ; lookups
        ldy TileCollision_TileSpaceX
        ; The value we find at this offset will be thhe ID of the tile at that spot in the nametable
        lda (TileCollision_NametablePointer), y               
        ; In this game we treat any tile with an Id higher that $E0 as solid.                    
        cmp #$E0                                        
        bcc Exit                                        
            lda #1                                      
            rts
        Exit:
        lda #0                                          
        rts
.endproc

.proc Actor_TakeDamageAndMaybeDie
    lda_CollidingActor Actor::type
    cmp #ActorType::BULLET
    ;bne Exit
    ;    lda_CurrentActor Actor::hitpoints
    ;    sec
    ;    sbc #1
    ;    cmp #0
    ;    bne :+

            jsr Actor_ExplosionNew
            jsr Actor_DeleteCurrentActor
            jsr Actor_DeleteCollidingActor
            lda #02
            ldx #FAMISTUDIO_SFX_CH3
            jsr famistudio_sfx_play
            rts
    ;    :
    ;    sta_CurrentActor Actor::hitpoints
    ;Exit:
    ;rts
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

    lda_CurrentActor Actor::facingRight
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

    lda_CurrentActor Actor::facingRight
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