; This program shows the basic boilerplate require to set up an NES program
; This is pretty close to the bare minimum a project must do to operate correctly

.include "../includes/constants.inc"
.include "../includes/header.inc"
.include "../includes/init.inc"

; $0800
.segment "CODE"
    ; Code of PRG ROM
    Reset:
        INIT_NES

    MainLoop:
        jmp MainLoop

    NMI:
        rti ; Return from Interrupt
    IRQ:
        rti ; Return from Interrupt

; $FFFA
.segment "VECTORS"
    ;On boot the NES jumps to $FFFA to find the addresses for the handlers
    ;This is called the VECTORS 
    .word NMI   ; address of the NMI handler
    .word Reset ; address of the RESET handler
    .word IRQ   ; address of the IRQ handler
