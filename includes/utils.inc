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