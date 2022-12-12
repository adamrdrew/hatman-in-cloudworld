Between_PaletteData:
    .byte $0F,$0D,$16,$20, $21,$0D,$07,$1B, $21,$0D,$3C,$20, $21,$0D,$27,$07 ; Background
    .byte $0F,$0D,$16,$20, $31,$18,$38,$28, $31,$0F,$0A,$30, $21,$0D,$27,$07 ; Sprites

;This is tile data that must be copied to the nametable
Between_BackgroundData:
    .incbin "maps/between.nam"

Between_ActorData:
    ; First byte of actor data is always actor count
    .byte 1
    ;      ActorType    xPos    yPos    FacingRight     hitpoints   col point   oam byte count (4 per tile)
            .byte 10,          00,     00,      00,           $ff,         08,        04






