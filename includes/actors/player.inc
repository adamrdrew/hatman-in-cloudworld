
.include "api.inc"

    .proc PlayerShootBullet
        jsr Actor_Player_CreateBullet
        rts
    .endproc

    ; This is the player state machine. Player object state is never updated anywhere else
    ; Instead we set states and react to the environment
    .proc Actor_Player_Logic
        lda #TRUE
        sta InGameplay

        lda #TRUE
        sta_CurrentActor Actor::collides

        ; We start every step assuming we're idle
        lda #STATE_IDLE
        sta PlayerState

        lda PlayerFacingRightCache
        sta_CurrentActor Actor::facingRight

        ; If DPAD is pressed we're walking
        lda DPADPressed
        cmp #TRUE
        bne DontSetWalkState
            lda #STATE_WALKING
            sta PlayerState
        DontSetWalkState:

        lda AButtonPressed
        cmp #TRUE
        bne ExitAButtonHandler
            lda_CurrentActor Actor::isOnGround
            cmp #TRUE
            bne ExitAButtonHandler
                lda #10
                sta_CurrentActor Actor::ySpeed  
                lda #03
                ldx #FAMISTUDIO_SFX_CH0
                jsr famistudio_sfx_play 
        ExitAButtonHandler:

        lda BButtonPressed
        cmp #TRUE
        bne ExitBButtonHandler
            ; We only allow 3 bullets on screen at a time
            jsr PlayerShootBullet
        ExitBButtonHandler:

        ; Because of the constant downward force of gravity we are always potentially
        ; moving on the Y axis
        jsr Actor_HandleVerticalTileCollission
        jsr Actor_UpdateSpritePositionVertical

        Idle:
            ldx PlayerState
            cpx #STATE_IDLE
            bne Walking
            ;Do Idle Stuff
        Walking:
            ldx PlayerState
            cpx #STATE_WALKING
            bne Exit
            jsr Actor_HandleHorizTileCollission
            jsr Actor_UpdateSpritePositionHorizontal
        Falling:
        Jumping:

        Exit:
        jsr Actor_Player_Draw_Right
        rts
    .endproc

.proc Actor_Player_Draw
        lda_CurrentActor Actor::facingRight
        cmp #TRUE 
        bne :+
            jsr Actor_Player_Draw_Right
            jmp Exit
        :
        jsr Actor_Player_Draw_Left
        
    Exit:
    rts
.endproc

.proc Actor_Player_Draw_Right
    lda_CurrentActor Actor::oamOffset
    tax 

    ; Tile one
    lda_CurrentActor  Actor::yPos
    sta PlayerYPos
    sta OAM_COPY, x

    inx
    lda #$30
    sta OAM_COPY, x

    inx 
    lda #$0
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta PlayerXPos
    sta OAM_COPY, x

    inx
    ; Tile Two
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$1
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
    lda #$10
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
    lda #$11
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


.proc Actor_Player_Draw_Left
    lda_CurrentActor Actor::oamOffset
    tax 

    ; Tile one
    lda_CurrentActor  Actor::yPos
    sta PlayerYPos
    sta OAM_COPY, x

    inx
    lda #$1
    sta OAM_COPY, x

    inx 
    lda #%01000000
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    sta PlayerXPos
    sta OAM_COPY, x

    inx
    ; Tile Two
    lda_CurrentActor  Actor::yPos
    sta OAM_COPY, x

    inx
    lda #$30
    sta OAM_COPY, x

    inx 
    lda #%01000000
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
    lda #$11
    sta OAM_COPY, x

    inx 
    lda #%01000000
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
    lda #$10
    sta OAM_COPY, x

    inx 
    lda #%01000000
    sta OAM_COPY, x

    inx
    lda_CurrentActor  Actor::xPos
    clc
    adc #8
    sta OAM_COPY, x

    rts
.endproc

.proc Actor_Player_SpriteCollision
    lda_CollidingActor Actor::type
    cmp #ActorType::COIN
    bne Exit
        inc Coins
        jsr Actor_DeleteCollidingActor
        lda #01
        ldx #FAMISTUDIO_SFX_CH0
        jsr famistudio_sfx_play 
    Exit:
        rts
.endproc

.proc Actor_Player_CreateBullet
    ; Check if we can create a new bullet
    lda BulletCount
    cmp #3
    bcs Exit
    inc BulletCount

    lda #ActorType::BULLET
    sta NewActor+Actor::type  
    
    lda_CurrentActor Actor::facingRight
    cmp #TRUE
    bne :+
        lda_CurrentActor Actor::xPos
        clc
        adc #8
    :
    cmp #FALSE 
    bne :+
        lda_CurrentActor Actor::xPos
        clc
        adc #248       
    :
    sta NewActor+Actor::xPos

    lda_CurrentActor Actor::yPos
    clc
    adc #04
    sta NewActor+Actor::yPos


    lda_CurrentActor Actor::facingRight
    sta NewActor+Actor::facingRight

    lda #$FF
    sta NewActor+Actor::hitpoints


    lda #0
    ; We use gravity as a raw frame counter for the bullet
    ; this is super hacky and terrible. But we need to do this
    ; because we're already using frame for the animation
    sta NewActor+Actor::gravity
    sta NewActor+Actor::ySpeed
    sta NewActor+Actor::frame

    lda #ActorStates::IDLE
    sta NewActor+Actor::state

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

    jsr Actor_Bullet_PlayShootSound

    jsr Actor_AddToArray
    Exit:
        rts
.endproc

;Player_SpriteData:
    ;       Y       Tile    Attrs       X
;    .byte   $80,    $00,    %00000000,  $70
;    .byte   $80,    $01,    %00000000,  $78
;    .byte   $88,    $10,    %00000000,  $70
;    .byte   $88,    $11,    %00000000,  $78