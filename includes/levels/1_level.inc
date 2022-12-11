LevelOne_PaletteData:
    .byte $0F,$0C,$13,$33, $21,$0D,$07,$1B, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Background
    .byte $0F,$0D,$16,$20, $21,$0D,$1A,$2A, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Sprites

;This is tile data that must be copied to the nametable
LevelOne_BackgroundData:
    .incbin "maps/lvl1.nam"

LevelOne_ActorData:
    ; First byte of actor data is always actor count
    .byte $04
    ;      ActorType    xPos    yPos    FacingRight     hitpoints   col point   oam byte count (4 per tile)
    .byte   $08,        $80,    $70,    $01,            $03,        $10,        $10
    .byte   $03,        $30,    $30,    $01,            $FF,        $08,        $04
    .byte   $03,        $40,    $40,    $01,            $FF,        $08,        $04
    .byte   $07,        $D8,    $40,    $00,            $04,        $10,        $10

