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

A simple loop:

```asm
    LDY #100    ; Set Y to 100
Loop:           ; Create a label
    DEY         ; Decrement Y
    BNE Loop    ; Brance if not equal to 0. If y != 0 GOTO Loop
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