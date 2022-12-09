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
    lda #FALSE
    sta NewActor+Actor::hasGravity

    jsr Actor_AddToArray
    rti 
.endproc

.proc Actor_Bullet_Logic

.endproc