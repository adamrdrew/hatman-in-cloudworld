.proc Actor_CrawlerWalkAndForth
    lda Frame
    jsr GetRandomNumber
    lsr
    lsr
    cmp #32
    bcs Exit

    lda_CurrentActor Actor::isOnGround
    cmp #TRUE
    bne Exit

    jsr Actor_UpdateSpritePositionHorizontal

    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne Left
    Right:
        ldx #17
        ldy #8
        jsr Actor_TestTileCollision
        cmp #TRUE
        beq UpdateFacingLeft
        ldx #18
        ldy #18
        jsr Actor_TestTileCollision
        cmp #TRUE
        beq Exit
        UpdateFacingLeft:
            lda #0
            sta_CurrentActor Actor::facingRight    
        rts
    Left:
         ldx #254
        ldy #8
        jsr Actor_TestTileCollision
        cmp #TRUE
        beq UpdateFacingRight
        ldx #253
        ldy #18
        jsr Actor_TestTileCollision
        cmp #TRUE
        beq Exit
        UpdateFacingRight:
            lda #1
            sta_CurrentActor Actor::facingRight           
            rts
    Exit:
    rts
.endproc

.proc Actor_Crawler_Logic
        lda #1
        sta_CurrentActor Actor::xSpeed

        ; Because of the constant downward force of gravity we are always potentially
        ; moving on the Y axis
        jsr Actor_HandleVerticalTileCollission
        jsr Actor_UpdateSpritePositionVertical

        jsr Actor_CrawlerWalkAndForth

        jsr Actor_Crawler_Draw

    rts
.endproc

.proc Actor_Crawler_Draw
        lda_CurrentActor Actor::facingRight
        cmp #TRUE 
        bne :+
            jsr Actor_Crawler_Draw_Right
            jmp Exit
        :
        jsr Actor_Crawler_Draw_Left
        
    Exit:
    rts
.endproc

.proc Actor_Crawler_Draw_Right
    lda_CurrentActor Actor::oamOffset
    tax 

    ; Tile one
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$6E
    sta OAM_COPY, x

    inx 
    lda #%00000010
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile Two
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$6F
    sta OAM_COPY, x

    inx 
    lda #%00000010
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    ; Tile three
    lda_CurrentActor  Actor::yPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    lda #$7E
    sta OAM_COPY, x

    inx 
    lda #%00000100
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile four
    lda_CurrentActor  Actor::yPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    lda #$7F
    sta OAM_COPY, x

    inx 
    lda #%00000100
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    rts
.endproc

.proc Actor_Crawler_Draw_Left
    lda_CurrentActor Actor::oamOffset
    tax 

    ; Tile one
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$6F
    sta OAM_COPY, x

    inx 
    lda #%01000010
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile Two
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$6E
    sta OAM_COPY, x

    inx 
    lda #%01000010
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    ; Tile three
    lda_CurrentActor  Actor::yPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    lda #$7F
    sta OAM_COPY, x

    inx 
    lda #%01000100
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile four
    lda_CurrentActor  Actor::yPos
    clc
    adc #8
    sta OAM_COPY, x

    inx
    lda #$7E
    sta OAM_COPY, x

    inx 
    lda #%01000100
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    rts
.endproc

.proc Actor_Crawler_SpriteCollision
    jsr Actor_TakeDamageAndMaybeDie
    rts
.endproc