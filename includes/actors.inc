.enum ActorType
    NULL    = 0
    BULLET  = 1
    CRAWLER = 2
    BOUNCER = 4
    COIN    = 5
    BOTTLE  = 6
.endenum

.struct Actor
    type        .byte
    xPos        .byte
    yPos        .byte
    facingRight .byte
.endstruct