.proc Actor_WalkerWalkAndForth
    lda Frame
    cmp #15
    bcc Exit

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
        bne Exit
        lda #0
        sta_CurrentActor Actor::facingRight    
        rts
    Left:
        ldx #254
        ldy #8
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        lda #1
        sta_CurrentActor Actor::facingRight           
        rts
    Exit:
    rts
.endproc

.proc Actor_Walker_Logic
        lda #1
        sta_CurrentActor Actor::xSpeed

        ; Because of the constant downward force of gravity we are always potentially
        ; moving on the Y axis
        jsr Actor_HandleVerticalTileCollission
        jsr Actor_UpdateSpritePositionVertical

        jsr Actor_WalkerWalkAndForth


        jsr Actor_Walker_Draw

    rts
.endproc

.proc Actor_Walker_Draw
    lda_CurrentActor Actor::oamOffset
    tax 

    ; Tile one
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$0E
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta OAM_COPY, x

    inx
    ; Tile Two
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$0F
    sta OAM_COPY, x

    inx 
    lda #$0
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
    lda #$1E
    sta OAM_COPY, x

    inx 
    lda #$0
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
    lda #$1F
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    rts
.endproc

.proc Actor_Walker_SpriteCollision
    lda_CollidingActor Actor::type
    cmp #ActorType::BULLET
    bne Exit
        jsr Actor_ExplosionNew
        jsr Actor_DeleteCurrentActor
        jsr Actor_DeleteCollidingActor
    Exit:
        rts
.endproc