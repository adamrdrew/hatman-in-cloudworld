; Creates a new bullet
; Must be called from the step logic of a CurrentActor
.proc Actor_BulletNew

        lda #ActorType::BULLET
        sta NewActor+Actor::type  
        
        lda_CurrentActor Actor::xPos
        sta NewActor+Actor::xPos

        lda_CurrentActor Actor::yPos
        sta NewActor+Actor::yPos

        lda_CurrentActor Actor::facingRight
        sta NewActor+Actor::facingRight

        lda #$FF
        sta NewActor+Actor::hitpoints

        lda #0
        sta NewActor+Actor::gravity
        sta NewActor+Actor::ySpeed
        lda #3
        sta NewActor+Actor::xSpeed

        lda #8
        sta NewActor+Actor::collisionPointOffset

        lda ActorOAMOffset
        sta NewActor+Actor::oamOffset

        clc
        adc #4
        sta ActorOAMOffset

        jsr Actor_AddToArray

        rts
.endproc


.proc Actor_Bullet_Logic
    jsr Actor_Bullet_Draw
    
    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne MoveLeft
        lda_CurrentActor Actor::xPos
        clc
        adc #3
        sta_CurrentActor Actor::xPos
        jmp DoneMoving 
    MoveLeft:
        lda_CurrentActor Actor::xPos
        sec
        sbc #3
        sta_CurrentActor Actor::xPos
    DoneMoving:

    sec
    cmp #03
    bcs CheckOtherSideOfScreen
        jsr Actor_DeleteCurrentActor
    CheckOtherSideOfScreen:
    sec
    cmp #252
    bcc Exit
        jsr Actor_DeleteCurrentActor
    
    Exit:
    rts
.endproc

.proc Actor_Bullet_Draw
    lda_CurrentActor Actor::oamOffset
    tax 

    lda_CurrentActor Actor::yPos
    sta OAM_COPY, x

    inx
    lda Frame
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

    inx 
    lda #$0
    sta OAM_COPY, x

    inx

    lda_CurrentActor Actor::xPos
    sta OAM_COPY, x
    rts
.endproc

.proc Actor_Bullet_SpriteCollision
    rts
.endproc