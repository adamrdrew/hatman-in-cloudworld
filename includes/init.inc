; This macro performs the various init procedures that are requires to safely bootstrap 
; a NES program. It performs the following tasks:
; * Ensures IRQs, flags, and the stack are set up correctly
; * Lets the PPU "warm up"
; * Ensures there's no random audio glitches
; * Zeroes-out all RAM
;
; This code was laregly based on the course NES Programming in 6502 Assembly https://courses.pikuma.com/courses/nes-game-programming-tutorial
; And on the example init code found on NES Dev Wiki https://www.nesdev.org/wiki/Init_code

.macro INIT_NES
    sei             ; Disables all IRQs
    cld             ; Clears decimal mode. Decimal mode isn't supported on the NES CPU but this is customary to do for some reason.
    ; Init Stack Pointer
    ldx #$FF        ; Set X to hex literal FF
    txs             ; Init the stack pointer to $01FF
    ; Prevent Audio Glitches
    ldx #$40
    stx $4017       ; Disable APU frame IRQ
    ; We'll use 0 in a bunch of places coming up
    lda #0          ; Load A with 0
    ldx #$00        ; Load X with 0
    ; Disable a bunch of stuff we don't know the state of and want disabled on boot
    stx $2000       ; Disable NMI
    stx $2001       ; Disable rendering
    stx $4010       ; Disable DMC IRQs
    bit PPU_STATUS       ; Clear the VBlank flag as we don't know its state on boot

; The PPU requires a counple of VBLANKS to "warm up"
; This is the first
; We will know the VBLank is done when PPU_STATUS is positive 
WaitFirstVBlank:
    bit PPU_STATUS
    bpl WaitFirstVBlank

; Clear all system RAM
ClearRAM:
    sta $0000,x ; Store value in A (0) in memory address $0 + x (You can't just say x, you gotta do this weird little add) 
    sta $0100,x
    
    ;This is the OAM copy
    ;It is a special case where it needs to have FF as its default value
    ;Or we'll end up with noise in the upper left corner
    lda $FF
    sta $0200,x

    lda #0
    sta $0300,x
    sta $0400,x
    sta $0500,x
    sta $0600,x
    sta $0700,x
    dex         ; x--
    bne ClearRAM

; Do our second VBLank wait
WaitSecondVBlank:
    bit PPU_STATUS
    bpl WaitSecondVBlank
.endmacro

.macro INIT_VARIABLES
    lda #0
    sta Frame
    sta BackgroundPtr
    sta BackgroundPtr+1
    ; Set background pointer
    lda #STATE_IDLE
    sta PlayerState
    lda #TRUE
    sta PlayerFacingRightCache
    sta AButtonReleased
    lda #FALSE
    sta DPADPressed
    sta AButtonPressed
    lda #1
    sta ActorGravityCache
    lda #0
    sta ActorXSpeedCache
    sta ActorYSpeedCache
    sta BulletCount
    sta EnemyCount
    ; sta PlayerIsOnTheGround
.endmacro