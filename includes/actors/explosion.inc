; Creates a new bullet
; Must be called from the step logic of a CurrentActor
.proc Actor_ExplosionNew

        lda #ActorType::EXPLOSION
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
        ; I am using gravity as a frame counter here
        sta NewActor+Actor::gravity
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

        jsr Actor_AddToArray

        rts
.endproc


.proc Actor_Explosion_Logic
    jsr Actor_Explosion_Draw
    
    lda_CurrentActor Actor::hitpoints
    cmp #0
    bne Exit
    jsr Actor_DeleteCurrentActor
    
    Exit:
    rts
.endproc

.proc Actor_Explosion_Draw
    lda_CurrentActor Actor::oamOffset
    tax 

    lda_CurrentActor Actor::yPos
    sta OAM_COPY, x

    inx
   
    lda_CurrentActor Actor::gravity
    cmp #0
    bne :+
        lda #$06
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #15
    bne :+
        lda #$16
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #30
    bne :+
        lda #$26
        sta OAM_COPY, x
        jmp Exit
    :
    cmp #45
    bne :+
        lda #$36
        sta OAM_COPY, x
        lda #0
        sta_CurrentActor Actor::hitpoints 
        jmp Exit
    :
    Exit:

    clc
    adc #1
    sta_CurrentActor Actor::gravity

    inx 
    lda #$0
    sta OAM_COPY, x

    inx

    lda_CurrentActor Actor::xPos
    sta OAM_COPY, x
    rts
.endproc

