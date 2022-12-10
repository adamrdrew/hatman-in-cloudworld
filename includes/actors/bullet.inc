.proc Actor_BulletNew
    lda #ActorType::BULLET
    sta NewActor+Actor::type
    lda #3
    lda OAM_COPY, x
    sta NewActor+Actor::xPos
    lda OAM_COPY
    sta NewActor+Actor::yPos
    lda PlayerFacingRight
    sta NewActor+Actor::facingRight
    lda 0
    sta NewActor+Actor::gravity
    sta NewActor+Actor::xSpeed
    sta NewActor+Actor::ySpeed

    jsr Actor_AddToArray
    rti 
.endproc

.proc Actor_Bullet_Logic
    rts
.endproc

.proc Actor_Bullet_Draw
    rts
.endproc

.proc Actor_Bullet_SpriteCollision
    rts
.endproc