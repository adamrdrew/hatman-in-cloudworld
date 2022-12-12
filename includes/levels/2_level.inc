LevelTwo_PaletteData:
    .byte $0F,$13,$24,$30, $00,$00,$00,$00, $00,$00,$00,$00, $00,$00,$00,$00 ; Background
    .byte $0F,$0D,$16,$20, $31,$18,$38,$28, $31,$0F,$0A,$30, $21,$0D,$27,$07 ; Sprites

;This is tile data that must be copied to the nametable
LevelTwo_BackgroundData:
    .incbin "maps/lvl2.nam"

LevelTwo_ActorData:
    ; First byte of actor data is always actor count
    .byte 10
    ;      ActorType    xPos    yPos    FacingRight     hitpoints   col point   oam byte count (4 per tile)
    ;Player
    .byte   $08,        16,    23,    $01,            $03,        $08,        $10


    ; Enemies

    
    .byte   $07,        27,    23,    $00,            $04,        $08,        $10

    .byte   $04,        6,      7,    $00,            $04,        $08,        $10
    .byte   $04,        25,      7,    $01,            $04,        $08,        $10
    .byte   $04,        14,      12,    $01,            $04,      $08,        $10
    .byte   $04,        6,      18,    $01,            $04,        $08,        $10


    ; Coins
    .byte   $03,        5,    7,    $01,            $FF,        $04,        $04
    .byte   $03,        27,   7,    $01,            $FF,        $04,        $04
    .byte   $03,        10,    23,    $01,            $FF,        $04,        $04
    .byte   $03,        26,    24,    $01,            $FF,        $04,        $04







