; These are the actor type IDs that we use to 
; assign actors to their logic and what not

MAX_ACTORS = 10

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
    type            .byte   ; From the enum. Is used to determine what logic to run etc
    xPos            .byte
    yPos            .byte
    facingRight     .byte
    hitpoints       .byte
    gravity         .byte
    xSpeed          .byte
    ySpeed          .byte
    oamOffset       .byte
.endstruct

.include "bottle.inc"
.include "bullet.inc"
.include "coin.inc"
.include "crawler.inc"
.include "flyer.inc"
.include "heart.inc"
.include "walker.inc"
.include "player.inc"

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
        lda ActorType::NULL                 ; Load NULL
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

        ; Set the OAM offset
        lda ActorOAMOffset
        sta NewActor+Actor::oamOffset

        ; Set up the OAM offset for the next actor
        ; by increading it by our oam byte size + 1
        ; First get the oam byte size from the config
        iny
        lda (LevelActorDataPointer), y
        ; Add the current OAM Offset
        clc
        adc ActorOAMOffset
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

            ; Set the current actor
            ; This is a spot in zero page where we can keep the actor
            ; being acted on
            lda Actors+Actor::type, x
            sta CurrentActor+Actor::type
            lda Actors+Actor::xPos, x
            sta CurrentActor+Actor::xPos
            lda Actors+Actor::yPos, x
            sta CurrentActor+Actor::yPos
            lda Actors+Actor::facingRight, x
            sta CurrentActor+Actor::facingRight
            lda Actors+Actor::hitpoints, x
            sta CurrentActor+Actor::hitpoints
            lda Actors+Actor::gravity, x
            sta CurrentActor+Actor::gravity
            lda Actors+Actor::xSpeed, x
            sta CurrentActor+Actor::xSpeed
            lda Actors+Actor::ySpeed, x
            sta CurrentActor+Actor::ySpeed 
            lda Actors+Actor::oamOffset, x
            sta CurrentActor+Actor::oamOffset

            lda CurrentActor+Actor::type

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
        lda Actors+Actor::type, x
        sta CurrentActor+Actor::type
        lda Actors+Actor::xPos, x
        sta CurrentActor+Actor::xPos
        lda Actors+Actor::yPos, x
        sta CurrentActor+Actor::yPos
        lda Actors+Actor::facingRight, x
        sta CurrentActor+Actor::facingRight
        lda Actors+Actor::hitpoints, x
        sta CurrentActor+Actor::hitpoints
        lda Actors+Actor::gravity, x
        sta CurrentActor+Actor::gravity
        lda Actors+Actor::xSpeed, x
        sta CurrentActor+Actor::xSpeed
        lda Actors+Actor::ySpeed, x
        sta CurrentActor+Actor::ySpeed 
        lda Actors+Actor::oamOffset, x
        sta CurrentActor+Actor::oamOffset

        lda CurrentActor+Actor::type

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



.proc Actor_AddToArray
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        beq Exit                            ; If they are equal exit
        lda Actors+Actor::type,x            ; Grab the actor type of the actor we are looking at
        cmp ActorType::NULL                 ; Is it ActorType::NULL?
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

