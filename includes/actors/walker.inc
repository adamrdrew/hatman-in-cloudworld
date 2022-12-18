.proc Actor_WalkerWalkAndForth
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
    lda_CurrentActor Actor::state
    cmp #ActorStates::CREATING
    bne :+
        lda #1
        sta_CurrentActor Actor::xSpeed
        lda #ActorStates::IDLE
        sta_CurrentActor Actor::state
        lda #FALSE
        sta_CurrentActor Actor::isOnGround
        jmp Exit
    :
    cmp #ActorStates::IDLE 
    bne :+
        jsr Actor_HandleVerticalTileCollission
        jsr Actor_UpdateSpritePositionVertical
        jsr Actor_WalkerWalkAndForth
        jmp Exit
    :
    cmp #ActorStates::ATTACKING
    bne :+
        jmp Exit
    :
    cmp #ActorStates::TAKING_DAMAGE
    bne :+
        jsr Actor_TakeDamageKnockBack
        jmp Exit
    :
    cmp #ActorStates::DYING 
    bne :+
        jmp Exit
    :
    cmp #ActorStates::DELETING
    bne :+
        jmp Exit
    :
    
    Exit:
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
