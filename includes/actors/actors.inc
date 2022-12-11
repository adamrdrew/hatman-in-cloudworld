; These are the actor type IDs that we use to 
; assign actors to their logic and what not

MAX_ACTORS = 16

.enum ActorType
    NULL    = $FF
    BOTTLE  = 1
    BULLET  = 2
    COIN    = 3
    CRAWLER = 4
    FLYER   = 5
    HEART   = 6
    WALKER  = 7
    PLAYER  = 8
.endenum

.struct Actor
    type                    .byte   ; From the enum. Is used to determine what logic to run etc
    xPos                    .byte
    yPos                    .byte
    facingRight             .byte
    hitpoints               .byte
    gravity                 .byte
    xSpeed                  .byte
    ySpeed                  .byte
    collisionPointOffset    .byte
    oamOffset               .byte
.endstruct

.include "bottle.inc"
.include "bullet.inc"
.include "coin.inc"
.include "crawler.inc"
.include "flyer.inc"
.include "heart.inc"
.include "walker.inc"
.include "player.inc"

.proc Actor_CheckCollisionPoint
    lda #0
    sta CollisionOccured 

    ; First we need to find the point in the middle of CollidingActor
    ; this is the point we are going to evaluate to see if it is
    ; within CurrentActor's box
    lda_CollidingActor Actor::xPos
    ldy #Actor::collisionPointOffset
    clc
    adc (CollidingActorPtr), y
    sta ActorCollisionPointX
    lda_CollidingActor Actor::yPos
    clc
    adc (CollidingActorPtr), y
    sta ActorCollisionPointY
    
    ; If pointx is less than actor x exit
    lda_CurrentActor Actor::xPos
    cmp ActorCollisionPointX
    bcs Exit

    ; if point y is less than actor y exit
    lda_CurrentActor Actor::yPos
    cmp ActorCollisionPointY
    bcs Exit

    ; if point x is greater than box x exit
    lda ActorCurrentBottomRightX
    cmp ActorCollisionPointX
    bcc Exit

    ; if point y is greater than box y exit 
    lda ActorCurrentBottomRightY
    cmp ActorCollisionPointY
    bcc Exit

    ; Winner, winner, chicken dinner
    lda #1
    sta CollisionOccured 

    Exit:
        rts
.endproc

.proc Actor_CheckCollisionsForCurrentActor
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)  
        bne CheckCollisionsAgainstActor
            jmp Exit
        CheckCollisionsAgainstActor:
            lda Actors+Actor::type,x

            ; Set the CollisingActor
            ; The CollidingActor is what the CurrentActor will use in its 
            ; collision logic if a collision has occured
            CreateCollidingActorPointerFromX

            lda_CollidingActor Actor::type

            ; If we are looking at our own just skip to the next iteration
            ldy #Actor::type
            cmp  (CurrentActorPtr), y
            bne :+
                jmp NextActor
            :

            ; If we are looking at NULL skip
            cmp  #ActorType::NULL
            bne :+
                jmp NextActor
            :

            ; Do the collision check
            ; If no collision was found move on to the next actor
            jsr Actor_CheckCollisionPoint
            lda CollisionOccured
            cmp #0
            beq NextActor

            lda_CurrentActor Actor::type

            ; OK if we are here then we detected a collision between CurrentActor and CollidingActor
            ; Now just route to our collider logic
            ; Notice we exit, not NextActor
            ; We can only handle one collision per actor, per frame
            ; Trying to do more would get really tricky and would cost more memory
            cmp #ActorType::PLAYER
            bne :+
                jsr Actor_Player_SpriteCollision
                jmp Exit
            :

            cmp #ActorType::BULLET
            bne :+
                jsr Actor_Bullet_SpriteCollision
                jmp Exit
            :
            cmp #ActorType::CRAWLER
            bne :+
                jsr Actor_Crawler_SpriteCollision
                jmp Exit
            :
            cmp #ActorType::WALKER
            bne :+
                jsr Actor_Walker_SpriteCollision
                jmp Exit
            :
            cmp #ActorType::FLYER
            bne :+
                jsr Actor_Flyer_SpriteCollision
                jmp Exit
            :
            cmp #ActorType::COIN
            bne :+
                jsr Actor_Coin_SpriteCollision
                jmp Exit
            :
            cmp #ActorType::BOTTLE
            bne :+
                jsr Actor_Bottle_SpriteCollision
                jmp Exit
            :
            cmp #ActorType::HEART
            bne :+
                jsr Actor_Heart_SpriteCollision
                jmp Exit
            :

            NextActor:
                txa                                 ; transfer x to a
                clc                                 ; clear carry flag
                adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
                tax                                 ; transver a back to x
                jmp IterateOverActors               ; move on to the next actor
    Exit:
        rts
.endproc

; This is the entry point into the collission system
; This is run once every game loop
; This iterates through all of the actors, setting currentActor for each
; For currentActor it sets the ActorCurrentBottomRight variables in memory
; it then calls Actor_CheckCollisionsForCurrentActor
; That method does the collission checking work
.proc Actor_CheckCollisions
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)  
        bne CheckCollisionsForActor
            jmp Exit
        CheckCollisionsForActor:
            ; Create CurrentActor pointer
            CreateCurrentActorPointerFromX
            ; Load A with CurrentActor type
            lda_CurrentActor Actor::type
            
            cmp #ActorType::NULL
            beq NextActor
            
            ;Cache Registers
            CacheRegisters

            ;Load current actor xpos into A
            lda_CurrentActor Actor::xPos
            ldy #Actor::collisionPointOffset
            ; add collisionPointOffset
            clc
            adc (CurrentActorPtr), y
            sta ActorCurrentBottomRightX
            ; Load current actor ypos into A
            lda_CurrentActor Actor::yPos
            ; collision point offset
            clc
            adc (CurrentActorPtr), y
            sta ActorCurrentBottomRightY

            ; Call the routine that will iterate over the actors
            ; and check them against this actor
            jsr Actor_CheckCollisionsForCurrentActor

            ;Fetch cached registers
            FetchCachedRegisters

            NextActor:
                ; Move on to next actor
                txa                                 ; transfer x to a
                clc                                 ; clear carry flag
                adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
                tax                                 ; transver a back to x
                jmp IterateOverActors               ; move on to the next actor
    Exit:
        rts
.endproc


; Sets all of the actors in the actor array to type NULL
; We need to do this before loading a level's actor data
.proc Actor_ClearAll
    ; Set the default OAM offset
    lda #0
    sta ActorOAMOffset

    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        beq Exit                            ; If it is, exit    
        lda #ActorType::NULL                 ; Load NULL
        sta Actors+Actor::type,x            ; Set the actor type
        txa                                 ; transfer x to a
        clc                                 ; clear carry flag
        adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
        tax                                 ; transver a back to x
        jmp IterateOverActors               ; move on to the next actor
    Exit:
    rts
.endproc

.proc Actor_LoadLevelActorData
    jsr Actor_ClearAll

    ; Pointer Offset
    ldy #0
    ; First byte of actor level data is the count of actors
    ; We load this into x
    lda (LevelActorDataPointer), y
    tax
    LoadNextActor:
        iny   
        ; Load each property of the actor
        ; Each time it is the same process
        ; Type
        lda (LevelActorDataPointer), y  ; Load the byte at the current offset
        sta NewActor+Actor::type        ; populate the new actor struct for the corresponding value
        iny                             ; move to the next byte offset
        ; xPos
        lda (LevelActorDataPointer), y
        sta NewActor+Actor::xPos
        iny
        ; yPos
        lda (LevelActorDataPointer), y
        sta NewActor+Actor::yPos
        iny
        ; facingRight
        lda (LevelActorDataPointer), y
        sta NewActor+Actor::facingRight
        iny
        ; hitpoints
        lda (LevelActorDataPointer), y
        sta NewActor+Actor::hitpoints
        ; gravity, xSpeed, and ySpeed always start at 0
        lda #0
        sta NewActor+Actor::gravity
        sta NewActor+Actor::xSpeed
        sta NewActor+Actor::ySpeed

        iny
        lda (LevelActorDataPointer), y
        sta NewActor+Actor::collisionPointOffset

        ; Set the OAM offset
        lda ActorOAMOffset
        sta NewActor+Actor::oamOffset

        ; Set up the OAM offset for the next actor
        ; by increading it by our oam byte size + 1
        ; First get the oam byte size from the config
        ;iny
        ;lda (LevelActorDataPointer), y
        ; Add the current OAM Offset
        ;clc
        ;adc ActorOAMOffset
        ;sta ActorOAMOffset
        lda NewActor+Actor::collisionPointOffset
        cmp #8
        bne :+
            clc
            lda ActorOAMOffset
            adc #4
            jmp NextLoop
        :
            clc
            lda ActorOAMOffset
            adc #16
        
        NextLoop:
            iny
            sta ActorOAMOffset
            CacheRegisters
            ; Add the actor we just created to the array
            jsr Actor_AddToArray
            FetchCachedRegisters

            ; Decrement x. If we're not equal to 0 we loop back to the top
            dex
            cpx #0
            bne LoadNextActor
    rts
.endproc








.proc Actor_RunAll
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        
        bne ProcessActor                            ; If they are equal exit
            rts
        ProcessActor:
            lda Actors+Actor::type,x            ; Grab the actor type of the actor we are looking at

            CreateCurrentActorPointerFromX

            lda_CurrentActor Actor::type

            CacheRegisters
        ; Running the actors is just a big, dumb case statement
        RunActorLogic:
            cmp #ActorType::PLAYER
            bne :+
                jsr Actor_Player_Logic
                jmp NextActor
            :
            cmp #ActorType::BULLET
            bne :+
                jsr Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::CRAWLER
            bne :+
                jsr Actor_Crawler_Logic
                jmp NextActor
            :
            cmp #ActorType::WALKER
            bne :+
                jsr Actor_Walker_Logic
                jmp NextActor
            :
            cmp #ActorType::FLYER
            bne :+
                jsr Actor_Flyer_Logic
                jmp NextActor
            :
            cmp #ActorType::COIN
            bne :+
                jsr Actor_Coin_Logic
                jmp NextActor
            :
            cmp #ActorType::BOTTLE
            bne :+
                jsr Actor_Bottle_Logic
                jmp NextActor
            :
            cmp #ActorType::HEART
            bne :+
                jsr Actor_Heart_Logic
                jmp NextActor
            :
        NextActor:
            FetchCachedRegisters
            txa                                 ; transfer x to a
            clc                                 ; clear carry flag
            adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
            tax                                 ; transver a back to x
            jmp IterateOverActors               ; move on to the next actor
    rts
.endproc

.proc Actor_LoadSpriteData
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        bne LoadCurrentActor                            ; If they are equal exit
            rts
        ; Set the current actor
        ; This is a spot in zero page where we can keep the actor
        ; being acted on
        LoadCurrentActor:
        
        CreateCurrentActorPointerFromX

        lda_CurrentActor Actor::type 


        CacheRegisters

        ; Running the actors is just a big, dumb case statement
        DrawActorLogic:
            cmp #ActorType::PLAYER
            bne :+
                jsr Actor_Player_Draw
                jmp NextActor
            :
            cmp #ActorType::BULLET
            bne :+
                jsr Actor_Bullet_Draw
                jmp NextActor
            :
            cmp #ActorType::CRAWLER
            bne :+
                jsr Actor_Crawler_Draw
                jmp NextActor
            :
            cmp #ActorType::WALKER
            bne :+
                jsr Actor_Walker_Draw
                jmp NextActor
            :
            cmp #ActorType::FLYER
            bne :+
                jsr Actor_Flyer_Draw
                jmp NextActor
            :
            cmp #ActorType::COIN
            bne :+
                jsr Actor_Coin_Draw
                jmp NextActor
            :
            cmp #ActorType::BOTTLE
            bne :+
                jsr Actor_Bottle_Draw
                jmp NextActor
            :
            cmp #ActorType::HEART
            bne :+
                jsr Actor_Heart_Draw
                jmp NextActor
            :
        NextActor:
            FetchCachedRegisters
            txa                                 ; transfer x to a
            clc                                 ; clear carry flag
            adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
            tax                                 ; transver a back to x
            jmp IterateOverActors               ; move on to the next actor
    Exit:
    rts
.endproc

; Called when an actor is deleted. 
; We need to recalculate the oam offsets
; because now we have a hole in our OAM cache
.proc Actor_RecalcOAMOffsets
    ldx #0
    stx ActorOAMOffset ; Reset the OAM Offset
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        bne ProcessActor                    ; If they are equal exit
            rts
        ProcessActor:

        lda Actors+Actor::type, x
        ; If we are looking at NULL skip
        cmp  #ActorType::NULL
        bne :+
            jmp NextLoop
        :        

        ; Set the OAM offset
        lda ActorOAMOffset
        sta Actors+Actor::oamOffset,x

        ; Calculate OAMOffset for the next actor as it
        ; is a cumulative thing. We calculate it by 
        ; looking at the collision point, from which we
        ; can infer the tile count and thus bytes used in OAM
        ldy Actors+Actor::collisionPointOffset,x
        cpy #8
        bne :+
            clc
            adc #4
            sta ActorOAMOffset
            jmp NextLoop
        :
            clc
            adc #16
            sta ActorOAMOffset

        NextLoop:
            txa                                 ; transfer x to a
            clc                                 ; clear carry flag
            adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
            tax                                 ; transver a back to x
            jmp IterateOverActors               ; move on to the next actor
    rts
.endproc

; Zeros-out OAM_CACHE 
.proc Actor_ClearOAMCache
    lda #0
    ldy #255
    CountDown:
        dey
        sta OAM_COPY, y
        cpy #0
        bne CountDown
    sta OAM_COPY, y ; catch that off by one :)
    rts
.endproc

.proc Actor_DeleteCollidingActor
    ; Mark the actor as null so it can be reused
    lda #ActorType::NULL
    sta_CollidingActor Actor::type

    ; We now have to rebuild the offsets for the living actors
    jsr Actor_RecalcOAMOffsets

    ; Clear the OAM Cache
    jsr Actor_ClearOAMCache

    ; And rebuild it
    jsr Actor_LoadSpriteData

    rts
.endproc


.proc Actor_DeleteCurrentActor
    ; Mark the actor as null so it can be reused
    lda #ActorType::NULL
    sta_CurrentActor Actor::type

    ; We now have to rebuild the offsets for the living actors
    jsr Actor_RecalcOAMOffsets

    ; Clear the OAM Cache
    jsr Actor_ClearOAMCache

    ; And rebuild it
    jsr Actor_LoadSpriteData

    rts
.endproc

.proc Actor_AddToArray
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        beq Exit                            ; If they are equal exit
        lda Actors+Actor::type,x            ; Grab the actor type of the actor we are looking at
        cmp #ActorType::NULL                 ; Is it ActorType::NULL?
        bne NextActor                       ; If not null move to next iter

        lda NewActor+Actor::type
        sta Actors+Actor::type, x
        lda NewActor+Actor::xPos
        sta Actors+Actor::xPos, x
        lda NewActor+Actor::yPos
        sta Actors+Actor::yPos, x
        lda NewActor+Actor::facingRight
        sta Actors+Actor::facingRight, x
        lda NewActor+Actor::hitpoints
        sta Actors+Actor::hitpoints, x
        lda NewActor+Actor::gravity
        sta Actors+Actor::gravity, x
        lda NewActor+Actor::xSpeed
        sta Actors+Actor::xSpeed, x
        lda NewActor+Actor::ySpeed
        sta Actors+Actor::ySpeed, x
        lda NewActor+Actor::collisionPointOffset
        sta Actors+Actor::collisionPointOffset, x
        lda NewActor+Actor::oamOffset
        sta Actors+Actor::oamOffset, x

        jmp Exit
    
        NextActor:
            txa                             ; transfer x to a
            clc                             ; clear carry flag
            adc #.sizeof(Actor)             ; Jump to the next spot in the array that marks the start of an actor
            tax                             ; transver a back to x
            jmp IterateOverActors           ; move on to the next actor
    Exit:
    rts
.endproc

