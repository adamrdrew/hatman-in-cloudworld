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
        ldy #0
        lda source, y
        sta dest
        ldy #1
        lda source, y
        sta dest+1
.endmacro