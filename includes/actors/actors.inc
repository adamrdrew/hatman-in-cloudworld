.enum ActorType
    NULL    = 0
    BOTTLE  = 1
    BULLET  = 2
    COIN    = 3
    CRAWLER = 4
    FLYER   = 5
    HEART   = 6
    WALKER  = 7
.endenum

.struct Actor
    type            .byte   ; From the enum. Is used to determine what logic to run etc
    xPos            .byte
    yPos            .byte
    facingRight     .byte
    hitpoints       .byte
    hasGravity      .byte
.endstruct

.include "bottle.inc"
.include "bullet.inc"
.include "coin.inc"
.include "crawler.inc"
.include "flyer.inc"
.include "heart.inc"
.include "walker.inc"

.proc Actor_RunAll
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        beq Exit                            ; If they are equal exit
        lda Actors+Actor::type,x            ; Grab the actor type of the actor we are looking at
        RunActorLogic:
            cmp #ActorType::BULLET
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::CRAWLER
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::WALKER
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::FLYER
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::COIN
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::BOTTLE
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
            cmp #ActorType::HEART
            bne :+
                jmp Actor_Bullet_Logic
                jmp NextActor
            :
        NextActor:
            txa                                 ; transfer x to a
            clc                                 ; clear carry flag
            adc #.sizeof(Actor)                 ; Jump to the next spot in the array that marks the start of an actor
            tax                                 ; transver a back to x
            jmp IterateOverActors               ; move on to the next actor
    Exit:
    rti
.endproc

.proc Actor_AddToArray
    ldx #0
    IterateOverActors:
        cpx #MAX_ACTORS * .sizeof(Actor)    ; Compare our iterator to the total length of the actor array
        beq Exit                            ; If they are equal exit
        lda Actors+Actor::type,x            ; Grab the actor type of the actor we are looking at
        cmp ActorType::NULL                 ; Is it ActorType::NULL?
        beq AddNewActorToArray              ; We found a free spot
        NextActor:
            txa                             ; transfer x to a
            clc                             ; clear carry flag
            adc #.sizeof(Actor)             ; Jump to the next spot in the array that marks the start of an actor
            tax                             ; transver a back to x
            jmp IterateOverActors           ; move on to the next actor

    AddNewActorToArray:
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
    lda NewActor+Actor::hasGravity
    sta Actors+Actor::hasGravity, x
    Exit:
    rts
.endproc

