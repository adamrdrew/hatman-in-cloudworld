
.segment "CODE"

.enum ActorType
    NULL            = $FF
    BOTTLE          = 1
    BULLET          = 2
    COIN            = 3
    CRAWLER         = 4
    FLYER           = 5
    HEART           = 6
    WALKER          = 7
    PLAYER          = 8
    EXPLOSION       = 9
    START_BUTTON    = 10
.endenum

.enum ActorStates
    CREATING        = $CA
    TAKING_DAMAGE   = 1
    IDLE            = 2
    DYING           = 3
    ATTACKING       = 4
    DELETING        = 5
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
    isOnGround              .byte
    collisionPointOffset    .byte
    oamOffset               .byte
    state                   .byte
    frame                   .byte
    collides                .byte 
.endstruct