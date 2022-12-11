LevelOne_PaletteData:
    .byte $0F,$0C,$13,$33, $21,$0D,$07,$1B, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Background
    .byte $0F,$0D,$16,$20, $31,$18,$38,$28, $31,$0F,$0A,$30, $21,$0D,$27,$07 ; Sprites

;This is tile data that must be copied to the nametable
LevelOne_BackgroundData:
    .incbin "maps/lvl1.nam"

LevelOne_ActorData:
    ; First byte of actor data is always actor count
    .byte 9
    ;      ActorType    xPos    yPos    FacingRight     hitpoints   col point   oam byte count (4 per tile)
    ;Player
    .byte   $08,        3,    20,    $01,            $03,        $08,        $10


    ; Enemies
    .byte   $07,        28,    11,    $01,            $04,        $08,        $10
    .byte   $04,        9,      8,    $01,            $04,        $08,        $10
    .byte   $07,        15,      14,    $01,            $04,      $08,        $10
    .byte   $04,        18,      8,    $01,            $04,        $08,        $10


    ; Coins
    .byte   $03,        3,    10,    $01,            $FF,        $04,        $04
    .byte   $03,        4,    10,    $01,            $FF,        $04,        $04
    .byte   $03,        3,    11,    $01,            $FF,        $04,        $04
    .byte   $03,        4,    11,    $01,            $FF,        $04,        $04

    .byte   $03,        27,    10,    $01,            $FF,        $04,        $04
    .byte   $03,        28,    10,    $01,            $FF,        $04,        $04
    .byte   $03,        27,    11,    $01,            $FF,        $04,        $04
    .byte   $03,        28,    11,    $01,            $FF,        $04,        $04







