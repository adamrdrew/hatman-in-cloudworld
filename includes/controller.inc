    ;Called every NMI to read the buttons
    .proc Controller_ReadButtons
            lda #1
            sta Buttons

            sta CONTROLLER_ONE  ; Controller polls for player input
            lda #0
            sta CONTROLLER_ONE  ; Controller will send that input to the NES

            LoopButtons:
                lda CONTROLLER_ONE  ; Read from the controller. We get a byte, but only the last bit 
                                    ; contains the data we want
                lsr                 ; We shift right to place the first bit we read into the
                                    ; carry flag
                rol Buttons         ; Roll the bits in Buttons left, popping the value from the carry flag 
                                    ; on the end. The carry flag will now we empty unless we reached the
                                    ; end of the input
                                    ; Remember we started with Buttons set to %00000001. That 1 bit we 
                                    ; had will be popped onto the carry flag if we filled Buttons up
                                    ; So, at the end of the rol call carry flag will be empty (not 0)
                                    ; unless we reached the end in which case our initial 1 will be there
                bcc LoopButtons     ; y so we're done
            rts
    .endproc

    ; Called every NMI to handle the buttons
    .proc Controller_ButtonHandler
        ; Buttons that can be held down must be initialized to false
        ; at the start of every frame and then set to pressed if pressed detected
        ; 
        ; Buttons that must be pressed and releases should be set to false
        ; if they are detected to not being pressed, and true if detected
        ; See DPAD vs A Buttton
        lda #FALSE
        sta DPADPressed

        CheckA:
            lda Buttons
            and #%10000000
            beq AButton_HardwareNotPressed
                lda AButtonReleased
                cmp #FALSE
                ; The physical hardware button is pressed, but the released flag hasn't been set
                ; this means the player is holding the button down
                ; We clean the AButtonPressed flag so that any logic that triggers on that doesn't happen
                ; and do nothing else
                ; The player will need to release the button and then press again to pass this guard
                beq AButton_ClearPressedFlag
                    lda #TRUE
                    sta AButtonPressed
                    lda #FALSE
                    sta AButtonReleased
                    jmp CheckB
            ; If the physical hardware button isn't pressed then we can clear both of
            ; our status flags. 
            AButton_HardwareNotPressed:
                lda #TRUE
                sta AButtonReleased
            AButton_ClearPressedFlag:
                lda #FALSE
                sta AButtonPressed
        CheckB:
            lda Buttons
            and #%01000000
            beq BButton_HardwareNotPressed
                lda BButtonReleased
                cmp #FALSE
                ; The physical hardware button is pressed, but the released flag hasn't been set
                ; this means the player is holding the button down
                ; We clean the AButtonPressed flag so that any logic that triggers on that doesn't happen
                ; and do nothing else
                ; The player will need to release the button and then press again to pass this guard
                beq BButton_ClearPressedFlag
                    lda #TRUE
                    sta BButtonPressed
                    lda #FALSE
                    sta BButtonReleased
                    jmp CheckSelect
            ; If the physical hardware button isn't pressed then we can clear both of
            ; our status flags. 
            BButton_HardwareNotPressed:
                lda #TRUE
                sta BButtonReleased
            BButton_ClearPressedFlag:
                lda #FALSE
                sta BButtonPressed
        CheckSelect:
            lda Buttons
            and #%00100000
            beq CheckStart
            ; Do Select Stuff
        CheckStart:
            lda Buttons
            and #%00010000
            beq CheckUp
            ; Do Start Stuff
        CheckUp:
            lda Buttons
            and #%00001000
            beq CheckDown
            ;Do up stuff
        CheckDown:
            lda Buttons
            and #%00000100
            beq CheckLeft
            ; Do Down Stuff
        CheckLeft:
            lda Buttons
            and #%00000010
            beq CheckRight
            lda #$AA
            sta DebugByte
            lda #FALSE                  
            sta PlayerFacingRightCache
            lda #TRUE
            sta DPADPressed
        CheckRight:
            lda Buttons
            and #%00000001
            beq Exit
            lda #$BB
            sta DebugByte
            lda #TRUE            
            sta PlayerFacingRightCache
            lda #TRUE
            sta DPADPressed
        Exit:
        rts
    .endproc 