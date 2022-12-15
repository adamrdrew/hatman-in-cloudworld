; Creates a new bullet
; Must be called from the step logic of a CurrentActor
.proc Actor_BulletNew
    lda #ActorType::BULLET
    sta NewActor+Actor::type  
    
    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne PutItToTheLeft
        lda_CurrentActor Actor::xPos
        clc
        adc #18
        jmp SaveIt
    PutItToTheLeft:
        lda_CurrentActor Actor::xPos
        clc
        adc #249         
    SaveIt:
    sta NewActor+Actor::xPos
    sta BulletOriginX

    lda_CurrentActor Actor::yPos
    clc
    adc #04
    sta NewActor+Actor::yPos


    lda_CurrentActor Actor::facingRight
    sta NewActor+Actor::facingRight

    lda #$FF
    sta NewActor+Actor::hitpoints

    ; I am cheating and using gravity to store a framecounter
    lda #0
    sta NewActor+Actor::gravity
    lda #0
    sta NewActor+Actor::ySpeed
    lda #3
    sta NewActor+Actor::xSpeed

    lda #4
    sta NewActor+Actor::collisionPointOffset

    lda ActorOAMOffset
    sta NewActor+Actor::oamOffset

    clc
    adc #4
    sta ActorOAMOffset

    lda #TRUE
    sta NewActor+Actor::collides

    jsr Actor_AddToArray
    Exit:
        rts
.endproc


.proc Actor_Bullet_Logic
    jsr Actor_Bullet_Draw
    
    lda_CurrentActor Actor::frame
    sec
    adc #1
    sta_CurrentActor Actor::frame
    cmp #120
    bne :+
        dec BulletCount
        jsr Actor_DeleteCurrentActor
    :

    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne MoveLeft
        lda_CurrentActor Actor::xPos
        clc
        adc #4
        sta_CurrentActor Actor::xPos
        jmp DoneMoving 
    MoveLeft:
        lda_CurrentActor Actor::xPos
        sec
        sbc #4
        sta_CurrentActor Actor::xPos
    DoneMoving:

    sec
    cmp #03
    bcs CheckOtherSideOfScreen
        dec BulletCount
        jsr Actor_DeleteCurrentActor
    CheckOtherSideOfScreen:
    sec
    cmp #252
    bcc CheckForBlocks
        jsr Actor_DeleteCurrentActor
    
    CheckForBlocks:
        lda_CurrentActor Actor::facingRight
        cmp #TRUE
        bne Left
        Right:
            ldx #8
            ldy #4
            jsr Actor_TestTileCollision
            cmp #TRUE
            bne Exit
            lda #0
            dec BulletCount
            jsr Actor_ExplosionNew
            jsr Actor_DeleteCurrentActor    
            rts
        Left:
            ldx #254
            ldy #4
            jsr Actor_TestTileCollision
            cmp #TRUE
            bne Exit
            lda #1
            dec BulletCount
            jsr Actor_ExplosionNew
            jsr Actor_DeleteCurrentActor           
            rts
    Exit:
    rts
.endproc

.proc Actor_Bullet_Draw
    lda_CurrentActor Actor::oamOffset
    tax 

    lda_CurrentActor Actor::yPos
    sta OAM_COPY, x

    inx
    lda_CurrentActor Actor::gravity

    cmp #0
    bne :+
        lda #$02
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #15
    bne :+
        lda #$12
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #30
    bne :+
        lda #$22
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #45
    bne :+
        lda #$32
        sta OAM_COPY, x
        jmp Exit
    :
    Exit:

    lda_CurrentActor Actor::gravity
    clc
    adc #1
    cmp #60
    bne SetFrameCounter
        lda #0
    SetFrameCounter:
        sta_CurrentActor Actor::gravity


    inx 
    lda #$0
    sta OAM_COPY, x

    inx

    lda_CurrentActor Actor::xPos
    sta OAM_COPY, x
    rts
.endproc

.proc Actor_Bullet_SpriteCollision
    lda_CollidingActor Actor::type
    dec BulletCount
    cmp #ActorType::CRAWLER
    bne :+
        jsr Actor_Crawler_SpriteCollision
    :
    cmp #ActorType::WALKER
    bne :+
        jsr Actor_Walker_SpriteCollision
    :
    lda #02
    ldx #FAMISTUDIO_SFX_CH3
    jsr famistudio_sfx_play
    jsr Actor_DeleteCurrentActor
    rts
.endproc