.enum ActorType
    NULL    = 0
    BULLET  = 1
    CRAWLER = 2
    FLYER   = 3
    BOUNCER = 4
.endenum

.struct Actor
    type        .byte
    xPos        .byte
    yPos        .byte
    facingRight .byte
.endstruct