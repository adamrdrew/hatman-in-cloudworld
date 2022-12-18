.macro PPU_SETADDR addr
        bit PPU_STATUS  ; Resets the PPU_ADDRESS latch register
        ldx #>addr      ; Get the high byte of the addr
        stx PPU_ADDR    ; Set the MSB of the PPU address
        ldx #<addr      ; Get the low byte of the addr
        stx PPU_ADDR    ; Set the LSB of the PPU address
.endmacro

.macro SetPPUAddresByPointer addr_pointer
        bit PPU_STATUS  ; Resets the PPU_ADDRESS latch register
        ldy #1
        lda addr_pointer, y
        sta PPU_ADDR    ; Set the MSB of the PPU address
        ldy #0
        lda addr_pointer, y
        sta PPU_ADDR    ; Set the LSB of the PPU address
.endmacro

.macro SetPointer pointer, address
        lda #<address
        sta pointer
        lda #>address
        sta pointer+1
.endmacro

.macro CopyPointer source, dest
        lda source
        sta dest
        lda source+1
        sta dest+1
.endmacro

.macro CacheRegisters
        stx RegisterCacheX
        sty RegisterCacheY
        sta RegisterCacheA
.endmacro

.macro FetchCachedRegisters
        ldx RegisterCacheX
        ldy RegisterCacheY
        lda RegisterCacheA
.endmacro

.macro NMI_CacheRegisters
        stx NMIRegisterCacheX
        sty NMIRegisterCacheY
        sta NMIRegisterCacheA
.endmacro

.macro NMI_FetchCachedRegisters
        ldx NMIRegisterCacheX
        ldy NMIRegisterCacheY
        lda NMIRegisterCacheA
.endmacro

; I took this from the NES Game Programming Tutorial couse on Pikuma
; I don not understand this code at all but the course instructor says it
; is a '16-bit Galois linear feedback shift register with polynomial $039'
; Yeah, I don't know
; Returns its value on A, clobbers y, and requires a 2 byte vairable called Seed
.proc GetRandomNumber
        ldy #8
        lda Seed*0
        :
        asl
        rol Seed*1
        bcc :+
                eor #$39
        :
        dey
        bne :--
        sta Seed*0
        cmp #0
        rts
.endproc

; Accepts an struct member label, like Actor::type
; loads A with the value of that member but handles indirection
; Caches Y to make sure we don't stomp on it
.macro lda_CurrentActor memberLabel
        sty ActorPointerOffsetCache     ; Cache Y
        ldy #memberLabel                 ; Load struct member label into y
        lda (CurrentActorPtr), y        ; Load A with the value we're trying to get
        ldy ActorPointerOffsetCache     ; Restore Y
.endmacro
; Same as above but for writing
.macro sta_CurrentActor memberLabel
        sty ActorPointerOffsetCache     ; Cache Y
        ldy #memberLabel                 ; Load struct member label into y
        sta (CurrentActorPtr), y        ; Store A in the member of the indirected actor
        ldy ActorPointerOffsetCache     ; Restore Y
.endmacro

; These two are the same as the above but for the Colliding actor
.macro lda_CollidingActor memberLabel
        sty ActorPointerOffsetCache     ; Cache Y
        ldy #memberLabel                 ; Load struct member label into y
        lda (CollidingActorPtr), y        ; Load A with the value we're trying to get
        ldy ActorPointerOffsetCache     ; Restore Y
.endmacro

.macro sta_CollidingActor memberLabel
        sty ActorPointerOffsetCache     ; Cache Y
        ldy #memberLabel                 ; Load struct member label into y
        sta (CollidingActorPtr), y      ; Store A in the member of the indirected actor
        ldy ActorPointerOffsetCache     ; Restore Y
.endmacro

.macro CreateCurrentActorPointerFromX
        txa
        clc
        adc #<Actors
        sta CurrentActorPtr

        lda #>Actors
        sta CurrentActorPtr+1
.endmacro

.macro CreateCollidingActorPointerFromX
        txa
        clc
        adc #<Actors
        sta CollidingActorPtr
        
        lda #>Actors
        sta CollidingActorPtr+1
.endmacro

.macro CreateCurrentActorPointerFromY
        tya
        clc
        adc #<Actors
        sta CurrentActorPtr

        lda #>Actors
        sta CurrentActorPtr+1
.endmacro

.macro CreateCollidingActorPointerFromY
        tya
        clc
        adc #<Actors
        sta CollidingActorPtr
        
        lda #>Actors
        sta CollidingActorPtr+1
.endmacro

.proc NMI_ChangeLevel
    lda GotoNextLevel
    cmp #FALSE
    bne :+
        rts
    :
    lda #0
    sta Coins
    sta Frame
    sta GotoNextLevel
    jsr LoadLevel
    rts
.endproc

.proc AdvanceLevel
    lda InGameplay
    cmp #TRUE
    beq :+
        rts
    :
    lda EnemyCount
    cmp #0
    beq :+
        rts
    :
    inc Level
    lda #TRUE
    sta GotoNextLevel
    rts
.endproc