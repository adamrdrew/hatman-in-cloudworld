; This file is for the FamiStudio Sound Engine and was generated by FamiStudio


.if FAMISTUDIO_CFG_C_BINDINGS
.export _sounds=sounds
.endif

sounds:
	.word @ntsc
	.word @ntsc
@ntsc:
	.word @sfx_ntsc_shoot
	.word @sfx_ntsc_coin
	.word @sfx_ntsc_explosion
	.word @sfx_ntsc_jump
	.word @sfx_ntsc_damage

@sfx_ntsc_shoot:
	.byte $82,$00,$81,$5b,$80,$3f,$85,$00,$84,$73,$83,$3f,$89,$f0,$01,$81
	.byte $53,$84,$61,$01,$81,$4b,$84,$4f,$01,$81,$46,$84,$86,$01,$83,$30
	.byte $01,$84,$4f,$83,$3f,$01,$80,$30,$00
@sfx_ntsc_coin:
	.byte $82,$00,$81,$21,$80,$3f,$89,$f0,$0a,$81,$1f,$0a,$00
@sfx_ntsc_explosion:
	.byte $8a,$0b,$89,$3f,$04,$8a,$0c,$04,$8a,$0d,$04,$8a,$0e,$04,$8a,$0f
	.byte $8a,$0b,$89,$3f,$04,$8a,$0c,$04,$8a,$0d,$04,$8a,$0e,$04,$8a,$0f
	.byte $04,$8a,$00,$0a,$00
@sfx_ntsc_jump:
	.byte $85,$02,$84,$98,$83,$3f,$89,$f0,$01,$84,$8a,$01,$84,$7c,$01,$84
	.byte $6e,$01,$84,$60,$01,$84,$52,$01,$84,$44,$01,$84,$36,$01,$84,$28
	.byte $01,$84,$1a,$01,$00
@sfx_ntsc_damage:
	.byte $85,$06,$84,$d9,$83,$3f,$89,$f0,$01,$85,$07,$84,$06,$01,$84,$32
	.byte $01,$84,$5f,$01,$84,$8b,$01,$84,$b8,$01,$84,$e4,$01,$84,$ff,$03
	.byte $00

.export sounds
