.proc Actor_StartButton_Logic
    lda #FALSE
    sta InGameplay
    lda BButtonPressed
    cmp #TRUE
    bne ExitBButtonHandler
        ldx #FAMISTUDIO_SFX_CH0
        jsr famistudio_sfx_play
        inc Level
        lda #TRUE
        sta GotoNextLevel
    ExitBButtonHandler:
    rts
.endproc

.proc Actor_StartButton_Draw
    rts
.endproc

.proc Actor_StartButton_SpriteCollision
    rts
.endproc