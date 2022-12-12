LevelTwo_PaletteData:
    .byte $0F,$05,$1C,$3C, $0F,$29,$15,$30, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Background
    .byte $0F,$0D,$16,$20, $31,$18,$38,$28, $31,$0F,$0A,$30, $21,$0D,$27,$07 ; Sprites

;This is tile data that must be copied to the nametable
LevelTwo_BackgroundData:
    .incbin "maps/lvl2.nam"

LevelTwo_ActorData:
    ; First byte of actor data is always actor count
    .byte 10
    ;      ActorType    xPos    yPos    FacingRight     hitpoints   col point   oam byte count (4 per tile)
    ;Player
    .byte   $08,        3,    20,    $01,            $03,        $08,        $10


    ; Enemies
    .byte   $07,        24,    16,    $01,            $04,        $08,        $10
    .byte   $04,        12,      18,    $01,            $04,        $08,        $10
    .byte   $04,        26,      11,    $01,            $04,        $08,        $10
    .byte   $04,        11,      24,    $01,            $04,      $08,        $10
    .byte   $04,        24,      24,    $01,            $04,        $08,        $10


    ; Coins
    .byte   $03,        27,    10,    $01,            $FF,        $04,        $04
    .byte   $03,        27,   23,    $01,            $FF,        $04,        $04
    .byte   $03,        13,    16,    $01,            $FF,        $04,        $04
    .byte   $03,        28,    15,    $01,            $FF,        $04,        $04







