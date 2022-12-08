.segment "ZEROPAGE"
DebugByte: .res 1

Buttons: .res 1         ; Reserve one by to represent button state (a,b,sel,sta,u,d,l,r)
Frame:   .res 1         ; Reserve 1 byte to store the framecounter
Seconds: .res 1         ; Reserve 1 butes to store the second counter, increments every 60 frames
BackgroundPtr: .res 2   ; Reserve 2 bytes for a pointer to a background array
BGDrawPointer: .res 2   ; Reserve 2 bytes for the pointer we use to iterate through while drawing
DrawTextPtr: .res 2
DrawTextAsciiCode: .res 1
DrawTextPosPtr: .res 2

; 0 if left, 1 if right
PlayerFacingRight: .res 1
PlayerMovingDown: .res 1
; 0 if not pressed, 1 if pressed
DPADPressed: .res 1

; Buttons that can't be held require 2 variables to track: Pressed and Released
AButtonPressed: .res 1
AButtonReleased: .res 1

; See constants
PlayerState: .res 1

PlayerXSpeed: .res 1
; Player upward movement (jump)
PlayerYSpeed: .res 1

; Contant downward pressure
Gravity: .res 1

PlayerXCollisionPoint: .res 1
PlayerYCollisionPoint: .res 1
PlayerXCollisionTileX: .res 1
PlayerYCollisionTileY: .res 1

PlayerIsOnTheGround: .res 1

TileLookupPointer: .res 2

TileCollisionLookupXOffset: .res 1
TileCollisionLookupYOffset: .res 1