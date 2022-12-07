# 6502 Notes

## Data and Address Sizes
* 8 bit data bus
* 16bit address bus
* so we will be putting 8 bit values in memory using 16bit addresses
* Most Significant Bit (MSB) is first bit in a byte
* Lowest Significant Bit (LSB) is last bit in a byte
* Upper Byte - in a 16 bit address value it is the first byte - so 08FF has 08 as upper
* Lower Byte - Inverse of upper byte

## Registers
### Programmable registers:
* A - Accumulator - Allows for arithmatic operations (add, subtract, compare). Typical pattern is to place a value on A and then add/sub/cmp with a value in memory. The result will be stored on A.
* X
* Y

Note: X and Y are indexed registers and are really good at like counting down loops and things like that

### Other Registers
* PC - Program Counter - stores address of next instruction to be executed. 16 bit.
* SP - 8bit - holds the lower byte of the address of the top position on the stack. 
* P - Processor Flags - Allows information about the result of the previous instruction. Was it 0? Was it an overflow? Was it negative?

### Processor Status Flags 
Gives context to the result of the previous operation. Flags are stored bitwise as they are just simple binary on/off values. VERY usful for conditionals. The flag bits are as follows:

* N - Negative Flag. Result of last op was negative.
* V - Overflow Flag. The last op resulted in an overflow. Means that you tried to add 01111111 to 00000001 and the result is 10000000 which in 2s complement is -128, not 127!
* - unused
* B - Break Flag
* D - Decimal Mode (BCD) Flag - In a normal 6502 this would allow us to use decimal numbers. The NES does not have the BCD. Unused in NES code.
* I - IRQ Disabled Flag - Enable or disable interupts
* Z - Zero Flag. Result of last OP was zero
* C - Carry Flag. Set to 1 if last op resulted in a carry. Means the result in the last op resulted in a value too large to fit in a 8bit data value.

### Negative Numbers
NES 6502 uses Two's Complement to represent negative numbers. In Two's Complement the MSB of the byte is reserved to represent whether the byte starts counting from 0 or from -128. When used this way the MSB is called the Sign Bit. If the Sign bit is 0 you just do normal binary math. You lose a bit so can only represent values up to 127. If the Sign Bit is 1 then the initial value is -128 and then you add whatever the value of the other 7 bits is. This means you can never have -0 because the max value 11111111 would be -1 (-128 + 127).

A couple of notes about this approach. You still have 256 possible values, its just that the range is -128-128 rather than 0-256. The other thing to remember is that you don't worry about this unless you are specifically looking for a negative value, and you only do that if the N processor flag is 1 :)

Also, super important, if you add 1 to 127 with 2's Complement (01111111 + 00000001) the result is 10000000, which flips the negative flag. So instead of getting 127 you get -128. It rotates around. This is called an Overflow and will set the V processor flag.

### Common 6502 Assembly instructions
|code|name|description|examples|
|-|-|-|-|
| `LDA` | Load A | Load a value into the A register | `LDA #4`|
| `LDX` | Load X | Load a value into the X register | `LDX #4`|
| `LDY` | Load Y | Load a value into the Y register | `LDY #4`|
| `STA` | Store A | Store the value in the A register in memory | `STA $02FF` |
| `STX` | Store X | Store the value in the X register in memory | `STX $02FF` |
| `STA` | Store Y | Store the value in the Y register in memory | `STY $02FF` |
| `ADC` | Add with Carry | Add to the value in register A. Set carry flag if carry happens. | `ADC #4` |
| `SBC` | Subtract with Carry | Subtract from the value in register A. Set carry flag if carry happens. | `SDC #4` |
| `CLC` | Clear Carry Flag | Clears the carry flag. Often done before `ADC` | `CLC` |
| `SEC` | Set Carry Flag | Sets the carry flag. Often done before `SBC` | `CLC` |
| `INC` | Increment Memory | Increments the value of a memory location by one | `INC $02FF`|
| `INX` | Increment X | Increments the value of X by one | `INX` |
| `INY` | Increment Y | Increments the value of Y by one | `INY` |
| `DEC` | Decrement Memory | Decrements the value of a memory location by one | `DEC $02FF`|
| `DEX` | Decrement X | Decrements the value of X by one | `DEX` |
| `DEY` | Decrement Y | Decrements the value of Y by one | `DEY` |
| `JMP` | Jump | Move the program counter to a specific location. GOTO. | `JMP $02FF` |
| `BCC` | Branch on Carry Clear | Jump if the carry flag is 0 | `BCC $02FF` |
| `BCS` | Branch on Carry Set | Jump if the carry flag is 1 | `BCS $02FF` |
| `BEQ` | Branch on equal to zero | Jump if the zero flag is 1 | `BEQ $02FF` |
| `BNE` | Branch on not equal to zero | Jump if the zero flag is 0 | `BNE $02FF` |
| `BMI` | Branch on minus | Jump if the negative flag is 1 | `BMI $02FF` |
| `BPL` | Branch on plus | Jump if the negative flag is 0 | `BPL $02FF` |
| `BVC` | Branch on Overflow Clear | Jump if the overflow flag is 0 | `BVC $02FF` |
| `BVS` | Branch on Overflow Set | Jump if the overflow flag is 1 | `BCS $02FF` |

### Common 6502 Assembly Patterns

#### For Loop
A simple loop:

```asm
    LDY #100    ; Set Y to 100
Loop:           ; Create a label
    DEY         ; Decrement Y
    BNE Loop    ; Brance if not equal to 0. If y != 0 GOTO Loop
```

#### If block
For an if we can do a compare and branch if not equal, and then use that to skip logic with an anonymous lable.

```
.segment "ZEROPAGE"
Frame: .res 1
Seconds: .res 1

.segment "CODE"
    lda #0
    sta Frame
    sta Seconds

    Loop:
        jmp Loop

NMI:
    inc Frame
    lda Frame       ; Variable we are going to compare
    cmp #60         ; CONDITIONAL = "frame == 60?"
    bne :SKIP_LOGIC ; if !CONDITONAL GOTO SKIP_LOGIC
    inc Seconds
    lda #0
    :SKIP_LOGIC
```
As you can see, and as with a lot of things in assembly, it is cast in the negative. It is the opposite of what we expect with a standard if block, but it does the same thing. An if in asm is cast sort of like this

```
if Frame != 60 {
} else {
    Seconds++
    Frame = 0
}
```

#### Looping through an array:

```
; Define an array of bytes in ROM
MyArray:
    .byte $32, $FF, $41, $56, $32, $3E, $AF, $FF

; Set our iteration count to 0
ldy #0
; Give our starting point a label
ReadArray: 
    ; Get the value from the array at position iteration count
    ldx MyArray, y
    ; do something with x
    iny
    ; Compare our iteration count to the length of the array
    cpy #8
    ; Loop if the iteration count is not equal to the array length
    bne ReadArray
```

#### Dynamic Macros
Dynamic macros are macros that can accept a parameter. These are not funcitons - they have no scope and do not return anything. These are not subroutes, the program counter never changes. These are inserted by the assembler into your code at the place you call them. However, they are super handy because they can accept parameters:

```
.macro PPU_SETADDR addr
        bit PPU_STATUS  ; Resets the PPU_ADDRESS latch register
        ldx #>addr      ; Get the high byte of the addr
        stx PPU_ADDR    ; Set the MSB of the PPU address
        ldx #<addr      ; Get the low byte of the addr
        stx PPU_ADDR    ; Set the LSB of the PPU address
.endmacro
```

That macro accepts the `addr` param which it then splits into high and low bytes for writing to the PPU address latch register. Calling it is as simple as

```
PPU_SETADDR $2000
```

#### Variables
We can declare named variables in the zero page for keeping track of data. This is incredibly awesome and handy and I did not expect assembly to allow this.

```
; Declare the variables by reserving their size in the zero page
.segment "ZEROPAGE"
Score: .res 1 ; Reserve one byte for score
Frame: .res 1 ; Reserve one byte for frame counter
```

At the beginning of your program you should set their initial values:

```
lda #0
sta Score
```

Any op code that can act on an address can act on the variable:

```
inc Score
dec Score
lda Score
```

#### Pointers
Pointers allow you a level of abstraction by storing a reference in memory to the address of something else in memory. You can then manipulate that thing by using the pointer to look up the address. This is handy if you want to be able to use the same code to work with different structures at different places in memory. Say for example, code to copy background data into a nametable. You are going to have many background data tables, so being able to reference what table you want to read by address rather than by name is handy, because by changing the address value of the pointer you can change what table you read.

Note: Pointer usage comes with some strict caveats that will cause your program not to compile. They are:
1. Pointers are 16 bit values because they are memory addresses, meaning that they need to be populated in 2 ops: lo-byte and then hi-byte
2. When Addressing an offset into a memory area by pointer you can only use the Y register for the offset. This is a hardware limitation.
3. When reading from a pointer you can only read into the accumulator. This is a hardware limit. Basically, never use the X register when working with pointers.


```
.macro SetPointer pointer, address
        lda #<address       ; Get the lo-byte of the background data address
        sta pointer         ; Store the lo-byte at the BackgrounbPtr
        lda #>address       ; Get the hi-byte at the BackgroundPtr
        sta pointer+1       ; Store the hi-byte at BackgroundPtr +1 (cuz its the next bit) 
.endmacro

.segment "ZEROPAGE"
    BackgroundPtr: .res 2   ; Reserve 2 bytes for the pointer

.segment "CODE"

    .proc IterateOverBackgroundData
        ; You MUST use Y as the index for the iterator when using a pointer
        ldy #0
        ReadBackgroundDataBytes:
            ; You MUST use A as your register when reading from a pointer
            ; The pointer address syntax is just the parens around the pointer name
            ; And notice again, we're using y as our index
            lda (BackgroundPtr), y 
            ; Do something with our data in A here, like send to PPU or whatever
            iny
            cpy #255
            bne ReadBackgroundDataBytes
        rts
    .endproc

    SetPointer BackgroundPtr, BackgroundData1
    IterateOverBackgroundData
    SetPointer BackgroundPtr, BackgroundData2
    IterateOverBackgroundData

    BackgroundData1:
        .byte 23,34,23,34,45,56,67,67,67,67,67 ;...
    BackgroundData2:
        .byte 34,67,23,45,67,34,56,78,89,56,34 ;...
```

### Addressing Modes
Consider these two accumulator load codes:

```
lda #80

lda $80
```

These look similar but they are different. 
* `lda #80` is Immediate Mode - it loads the value (in this case decimal 80) directly into the accumulator
* `lda $80` is Absolute Mode - it loads the accumulator with the value located at address `$0080`. This is also called Zero Page mode because it loads from the zero page (meaning the MSB is 00)

Note that it is easy to mix up loading a hex value in immediate mode with absolute mode:

```
lda #$80    ; Load the hex value 80 into the accumulator
lda $80     ; Load the value in $0080 into the accumulator
```

There's also Absolute Indexed:
```
STA $80,X
```

That stores the accumulator in the memory address $0080 + the value of X. So if X was $10 we'd store in $0090 - this is basically 6502 assembly's way of letting you programatically generate addresses from register values.


### Memory Map
The NES being a 6502 based computer is all about memory mapping. You don't have some rich SDK in software, rather you manipulate memory addresses that have specific functions. For example, the PPU, sound chip, and controllers are all accessed through memory. So how does address space break down?

```
-------------------------------------------------------------------
| $0000 - $07FF RAM
|     $0000 - $00FF is the zero page which is faster to access
|     $0100 - $01FF is the stack
| $0800 - $1FFF - Just repeating mirrors of $0000-$07FF
-------------------------------------------------------------------
| $2000 - $2007 - PPU
|    $2000 - PPU_CTRL
|    $2001 - PPU_MASK
|    $2002 - PPU_STATUS
|    $2003 - OAM_ADDR
|    $2004 - OAM_DATA
|    $2005 - PPU_SCROLL
|    $2007 - PPU_DATA
|    -- Mirrors of $2000-2007 repeating through $3FFF
-------------------------------------------------------------------
|$4000 - $401F - APU ports and IO registers
-------------------------------------------------------------------
|$4020 - $7FFF - Cartridge WRAM, battery backed save, and mapper registers
-------------------------------------------------------------------
|$8000 - $FFFF - Cartridge PRG-ROM
|    $FFFA-$FFFF - Vectors
-------------------------------------------------------------------
```

### PPU
* Has direct access to the CHR ROM in the cartridge. CHR ROM is a bitmap of all tiles in the game.
* Has direct access to 2kb of VRAM 

#### CHR ROM
* Where the graphics for the game is stored
* Partitioned into two Pattern Tables, 0 and 1
* Pattern table 0 is for sprite tiles and Pattern table 1 is for background tiles
* Tile bitmaps do not contain color information. They are just 4 shades of gray. Color is handled seperately with palletes.

#### Name Tables and VRam
* VRam is split up into 2 nametables, each 1kb in size
* The nametables are where the the tiles are layed-out into what we are going to display on the screen


#### VBlank
There are 240 scanlines where the PPU is drawing, and then 21 scanlines worth of VBlank where the electron gun is resetting. During this time we can manipulate the PPU and do things like change palettes or update VRAM. We know we are in VBLANK because an NMI will be raised. VBLANK gives us 339 CPU clock cycles worth of work time! 

#### PPU Registers
[This page](https://www.nesdev.org/wiki/PPU_registers) on the NES Dev wiki provides a fantastic overview that I'm largely going to copy.

For drawing the two most important registers are $2006 and $2007. $2006 is the address of the PPU memory map that you want to mutate. $2007 is where you put the data you want set in the address you put in $2006.

So, say you want to write #2A to PPU memory location $3F00 (set background to linme green) you'd do this:
```
PPU_MASK = $2001
PPU_ADDR = $2006
PPU_DATA = $2007

ldx #$3F
stx PPU_ADDR

ldx #$00
stx PPU_ADDR

ldx #$2A
sta PPU_DATA 

lda #&00011110
sta PPU_MASK
```

#### PPU Memory Map
```
-------------------------------------------------------------------
|   Pattern Tables (CHR ROM)
|       $0000-$0FFF - Pattern Table 0
|       $1000-$1FFF - Pattern Table 1
-------------------------------------------------------------------
|   Name Tables (VRAM)
|       $2000-$23FF - Name Table 0 & attributes
|       $2400-$27FF - Name Table 1 & attributes
|       $2800-$2BFF - Junk (Name Table 0 mirror)
|       $2C00-$2FFF - Junk (Name Table 1 mirror)
-------------------------------------------------------------------
|   Unused (Mirrors) $3000-3EFF
-------------------------------------------------------------------
|   Palettes #3F00-$3FFF
-------------------------------------------------------------------
```

#### Drawing a Background
Putting everything together we can now draw a background. The basics are:
* Load a CHR ROM
* Load background data (an array of tile IDs)
* Load palettes 
* Load the background data onto a nametable
* Tell the PPU to render

You load the CHR ROM in a segment and use a binary include:
```
;Load CHAR_ROM pattern tables
.segment "CHARS"
.incbin "resources/learning.chr"
```

The background data - what will be populated into your nametable - is just an array of tile IDs from the char rom

```
    BackgroundData:
        ;.incbin "resources/hello_hackathon.nam"
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$12,$0f,$16,$16,$19,$00,$12,$0b
        .byte $0d,$15,$0b,$1e,$12,$19,$18,$26,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$27,$0b,$0e,$0e
        .byte $1c,$0f,$21,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$e0,$e1
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$f0,$f1
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
```

Loading the nametable is as easy as pushing that BackgroundData onto the PPU at 0x2000

```
    .proc LoadNametable
        bit PPU_STATUS
        ldx #$20
        stx PPU_ADDR    ; Set the MSB of the PPU address we'll update to $3F
        ldx #$00
        stx PPU_ADDR    ; Set the LSB of the PPU address we'll update to $00 

        ldy #0
        ReadNametableBytes:
            ldx BackgroundData, y
            stx PPU_DATA
            iny 
            cpy #255
            bne ReadNametableBytes
        
        rts 
    .endproc
```

And then force the PPU to render:

```
        ;Enable PPU Rendering
        lda #%10010000  ; Enable NMI interrupts from PPU. Set BG to use 2nd pattern table
        sta PPU_CTRL
        lda #%00011110
        sta PPU_MASK    ; Setting the mask ensures we show the background
```

That's the basics. 

Note: The first tile on the top, left, right, and bottom edges isn't rendered correctly, it is in the overscan area.

#### Resolution, Nametable Size, and Attributes
Each screen displayed is 256x240 or 32 tiles x 30 tiles for 960 bytes total. In each nametable we have 1k of VRAM to use. That leaves 64 bytes left over. That is where we store attributes. Attrbites tell the PPU what patelletes to use. There isn't enough space for per-tile control, so it needs to be aggregated.

Attributes represent 4x4 "attribute tiles." Basically, if the nametable is broken up into a grid of 32x30 tiles. The attributes are broken up into a grid of 8x8 tiles, with each entry in the attribute table representing 4 total tiles - so we can control the palettes on a 2x2 tile basis. This is called the Attribute Grid.

Because we're dealing with so little space the Attribute Grid is stored bitwise. Here's an example:

```
AttributeData:
    .byte %00000000, %00000000, %10101010, %00000000, %11110000, %00000000, %00000000, %00000000
    .byte %11111111, %11111111, %11111111, %11111111, %11111111, %11111111, %11111111, %11111111
````
Each nibble (2 bits) represents the color palette for a group of 4 tiles. So the first byte represents the palette for 16 total tiles. Valid palette values are:

* 00 - Palette 0 
* 01 - Palette 1
* 10 - Palette 2
* 11 - Palette 3

### Sprites
* Sprites are composed of one ore more sprite tiles
* A max of 64 sprite tiles can be on the screen
* Sprites can move freely
* Sprite layer resolution is 256x240
* Sprites are managed by OAM (Object Attribute Memory) on the PPU
* OAM is internal to PPU as is 256 bytes of memory
* Sprites 0-63 each with 4 bytes of memory
* The 4 bytes that represent a sprite are:
    * YPos
    * Tile ID
    * Attributes (palette, priority, flip) - stored bitwise
    * XPos

#### Writing to the OAM
* The standard method is to make a copy of OAM data into RAM
* NES Programmers use $0200-$02FF for OAM copy
* You mutate OAM data in RAM
* Then you stomp on OAM
* Every frame we start by copying OAM into RAM
* We do our sprite logic
* We stomp over at the end of the frame
* To write to the OAM simply store the starting address of the RAM copy of OAM data to memory position $4014

```
NMI:
    lda #$02    ; Starting address of our OAM copy
    sta $4014   ; Tell OAM to start copying to that starting address
```

#### Copying from OAM
* OAM copy is done by DMA and is done by sending the address you want the OAM to copy to


### Input
* The button states are represented as one byte representing a,b,sel,sta,u,d,l,r
* The controller has 2 states: input and output. In input mode it is gathering user button presses. In output mode it is sending the current values to the NES
* The programmer has to "strobe" a latch register to switch IO mode
* And then you need to read byte by byte into a variable

