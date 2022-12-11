.proc Actor_CoinAnimate
    lda_CurrentActor Actor::oamOffset
    tax
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
    lda_CurrentActor Actor::oamOffset
    tax 

    lda_CurrentActor Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$03
    sta OAM_COPY, x

    inx 
    lda #%00000001
    sta OAM_COPY, x

    inx
    lda_CurrentActor Actor::xPos
    sta OAM_COPY, x

    rts
.endproc

