.proc Actor_StartButton_Logic
        lda BButtonPressed
        cmp #TRUE
        bne ExitBButtonHandler
            ldx #FAMISTUDIO_SFX_CH0
            jsr famistudio_sfx_play
            lda #4
            sta Coins 
        ExitBButtonHandler:
.endproc

.proc Actor_StartButton_Draw
    rts
.endproc

.proc Actor_StartButton_SpriteCollision
    rts
.endproc