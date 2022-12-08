.segment "ZEROPAGE"

; We reserve this byte to use during debugging
; It is the first byte in memory so it is easy to find and watch
DebugByte: .res 1

TileLookupPointer: .res 2
TileLookupPointerDebug: .res 2


Buttons: .res 1         ; Reserve one by to represent button state (a,b,sel,sta,u,d,l,r)
Frame:   .res 1         ; Reserve 1 byte to store the framecounter
Seconds: .res 1         ; Reserve 1 butes to store the second counter, increments every 60 frames

BackgroundPtr: .res 2   ; Reserve 2 bytes for a pointer to a background array
BGDrawPointer: .res 2   ; Reserve 2 bytes for the pointer we use to iterate through while drawing

DrawTextPtr: .res 2         ; Reserve 2 bytes for the text we are going to draw
DrawTextAsciiCode: .res 1   ; Reserve one byte for the ascii code we are going to look up to convert to a tile ID
DrawTextPosPtr: .res 2      ; Reserve 2 bytes for the pointer to where in tile space we are going to draw the tiles for the text


PlayerFacingRight: .res 1   ; 0 if left, 1 if right

; Buttons that can be held only need one variable
DPADPressed: .res 1
; Buttons that can't be held require 2 variables to track: Pressed and Released
; A Button is jump button
AButtonPressed: .res 1
AButtonReleased: .res 1
; B Button is shoot button
BButtonPressed: .res 1
BButtonReleased: .res 1


; The player is governed by a state machine
; This byte holds the player's current state
; states are defined in contants
PlayerState: .res 1

; Player's horizontal movement speed. Used by the player's walking and moving code
PlayerXSpeed: .res 1

; Player has 3 vertical movement variables that factor into his vertical movement system
; Player upward movement
; happens when jumping
; is quickly reduced over time
PlayerYSpeed: .res 1
; Contant downward pressure applied every frame where we aren't on the ground
Gravity: .res 1
; This flag is set when the player collides with a floor tile
; when this is TRUE we don't so any veritcal movement code
PlayerIsOnTheGround: .res 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Player Tile Collision Variables
;; These variables are private to the player collision code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; These two variables define the point that the player is going to check for a tile in pixel space
PlayerXCollisionPoint: .res 1
PlayerYCollisionPoint: .res 1
; These two are the previous two but recast into tile space. 
PlayerXCollisionTileX: .res 1
PlayerYCollisionTileY: .res 1
; This is the pointer that is used to hold the position into the tile map that we
; are searching at. Because the tile map is longer than an 8 bit number can hold
; we have to search in batches. We use this variable to hold a pointer to our 
; current position
; TileLookupPointer: .res 2
; These coordinates are used when searching through the tile data array
TileCollisionLookupXOffset: .res 1
TileCollisionLookupYOffset: .res 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


Actors:    .res MAX_ACTORS * .sizeof(Actor);
