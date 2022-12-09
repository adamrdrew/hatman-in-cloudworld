.proc Actor_Coin_Logic
    rts
.endproc

.proc Actor_Coin_Draw
    ldx CurrentActor+Actor::oamOffset

    lda CurrentActor+Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$03
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda CurrentActor+Actor::xPos
    sta OAM_COPY, x

    rts
.endproc