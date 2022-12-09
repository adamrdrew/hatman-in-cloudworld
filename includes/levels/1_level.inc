LevelOne_PaletteData:
    .byte $0F,$0C,$13,$33, $21,$0D,$07,$1B, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Background
    .byte $0F,$0D,$16,$20, $21,$0D,$1A,$2A, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Sprites

;This is tile data that must be copied to the nametable
LevelOne_BackgroundData:
    .incbin "maps/lvl1.nam"

LevelOne_ActorData:
    ; First byte of actor data is always actor count
    .byte $01
    ;      ActorType    xPos    yPos    FacingRight     hitpoints   oam byte size
    .byte  $03,         $20,    $20,    $01,            $FF,        $04

