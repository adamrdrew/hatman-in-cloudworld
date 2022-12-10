.proc Actor_CoinAnimate
    ldx CurrentActor+Actor::oamOffset
    inx

    lda Frame
    cmp #0
    bne :+
        lda #$03
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #15
    bne :+
        lda #$13
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #30
    bne :+
        lda #$23
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #45
    bne :+
        lda #$33
        sta OAM_COPY, x
        jmp Exit
    :
    Exit:
    rts
.endproc

.proc Actor_Coin_Logic
    jsr Actor_CoinAnimate
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