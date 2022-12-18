; Creates a new bullet
; Must be called from the step logic of a CurrentActor
.proc Actor_BulletDelete
    lda #ActorStates::DYING
    sta_CurrentActor Actor::state 
    dec BulletCount
    jsr Actor_DeleteCurrentActor
    rts
.endproc

.proc Actor_BulletExplode
    lda #02
    ldx #FAMISTUDIO_SFX_CH3
    jsr famistudio_sfx_play
    jsr Actor_ExplosionNew
    jsr Actor_BulletDelete
    rts
.endproc

.proc Actor_Bullet_PlayShootSound
    lda #00
    ldx #FAMISTUDIO_SFX_CH0
    jsr famistudio_sfx_play
    rts
.endproc

.proc Actor_Bullet_DeleteAfter120Frames
    lda_CurrentActor Actor::gravity
    clc
    adc #1
    sta_CurrentActor Actor::gravity
    cmp #120
    bne :+
        jsr Actor_BulletDelete
    :
    rts
.endproc

.proc Actor_Bullet_UpdateXPosition
    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne :+
        lda_CurrentActor Actor::xPos
        clc
        adc #4
        sta_CurrentActor Actor::xPos
    :
    cmp #FALSE 
    bne :+
        lda_CurrentActor Actor::xPos
        sec
        sbc #4
        sta_CurrentActor Actor::xPos
    :
    ; Check for out of bounds
    ; xPos is still in A here
    sec
    cmp #04
    bcs :+
        jsr Actor_BulletDelete
    :
    sec
    cmp #252
    bcc :+
        jsr Actor_BulletDelete
    :
    rts
.endproc

.proc Actor_Bullet_IncrementFrameCounter
    lda_CurrentActor Actor::frame
    clc
    adc #1
    cmp #60
    bne :+
        lda #0
    :
    sta_CurrentActor Actor::frame
    rts
.endproc

.proc Actor_Bullet_HandleTileCollisions
    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne :+
        ldx #8
        ldy #4
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        jsr Actor_BulletExplode    
        rts
    :
        ldx #254
        ldy #4
        jsr Actor_TestTileCollision
        cmp #TRUE
        bne Exit
        jsr Actor_BulletExplode          
    Exit:
        rts
.endproc

.proc Actor_Bullet_Logic
    lda_CurrentActor Actor::state
    cmp #ActorStates::IDLE
    bne :+
        jsr Actor_Bullet_Draw
        jsr Actor_Bullet_DeleteAfter120Frames
        jsr Actor_Bullet_UpdateXPosition
        jsr Actor_Bullet_HandleTileCollisions
        jsr Actor_Bullet_IncrementFrameCounter
    :
    rts
.endproc

.proc Actor_Bullet_Draw
    ; Get the OAM Offset and trasnfer it to X
    lda_CurrentActor Actor::oamOffset
    tax 

    ; Store the yPos in OAM
    lda_CurrentActor Actor::yPos
    sta OAM_COPY, x


    ; Write the sprite index
    inx
    lda_CurrentActor Actor::frame
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
    ; Sprite attrs are always 0
    inx 
    lda #$0
    sta OAM_COPY, x

    ; Store the xPos in OAM
    inx
    lda_CurrentActor Actor::xPos
    sta OAM_COPY, x

    rts
.endproc

.proc Actor_Bullet_SpriteCollision
    lda_CollidingActor Actor::type
    cmp #ActorType::CRAWLER
    bne :+
        jsr Actor_Bullet_CollideWithEnemy
    :
    cmp #ActorType::WALKER
    bne :+
        jsr Actor_Bullet_CollideWithEnemy
    :
    rts
.endproc

.proc Actor_Bullet_CollideWithEnemy
        lda_CurrentActor Actor::facingRight
        sta_CollidingActor Actor::facingRight
        
        

        SetSpeed:
            lda_CurrentActor Actor::xSpeed
            lda #6
            sta_CollidingActor Actor::xSpeed

            lda #ActorStates::TAKING_DAMAGE
            sta_CollidingActor Actor::state
            lda #0
            sta_CollidingActor Actor::frame

        jsr Actor_BulletExplode
        jsr Actor_TakeDamageAndMaybeDie

        rts
.endproc