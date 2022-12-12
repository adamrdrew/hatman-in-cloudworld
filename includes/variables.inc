.segment "ZEROPAGE"

; We reserve this byte to use during debugging
; It is the first byte in memory so it is easy to find and watch
DebugByte: .res 1

Coins: .res 1
Hearts: .res 1
Keys: .res 1
Lives: .res 1
Level: .res 1
Score: .res 1

; Used for random number generation
Seed: .res 2

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


; The player is different from other actors because it accepts controller input
; and the controller input code may not run in the context of the player's cycle
; so we cache the player's facingRight value here and use it in control code
; and then we sync it back to the player struct in his logic cycle
PlayerFacingRightCache: .res 1   ; 0 if left, 1 if right

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


; Actors store their gravity and speed variables in their structs
; but, we need a place in memory to cache those values to work with them
; in the movement code
ActorYSpeedCache: .res 1
ActorGravityCache: .res 1
ActorXSpeedCache: .res 1

; Other actors may care about player position, such as to change direction if
; he's near by. We sync the player's position to these ever tick so other 
; actors can use it
PlayerXPos: .res 1
PlayerYPos: .res 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Actor Tile Collision Variables
;; These variables are private to the actor collision code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; These two variables define the point that the player is going to check for a tile in pixel space
ActorXCollisionPoint: .res 1
ActorYCollisionPoint: .res 1
; These two are the previous two but recast into tile space. 
ActorXCollisionTileX: .res 1
ActorYCollisionTileY: .res 1
; This is the pointer that is used to hold the position into the tile map that we
; are searching at. Because the tile map is longer than an 8 bit number can hold
; we have to search in batches. We use this variable to hold a pointer to our 
; current position
; TileLookupPointer: .res 2
; These coordinates are used when searching through the tile data array
TileCollisionLookupXOffset: .res 1
TileCollisionLookupYOffset: .res 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Set to TRUE at the end of PPU NMI
IsDrawComplete: .res 1

; The array of actors
Actors:     .res 16 * 11
; The swap space used when creating a new actor
NewActor:   .res 11
; Pointer to the current actor. Used in a lot of per-actor code
CurrentActorPtr: .res 2
; Pointer to the colliding actor. Used in actor collision code.
CollidingActorPtr: .res 2


; This holds the pointer to the currently loaded level's
; actor data. The level pops its level data address
; into here, and then we read it from here to load the 
; actors
LevelActorDataPointer:  .res 2

; These are unfortunate
; There are places (nested loops) where I need to preserve
; the contents of the registers
RegisterCacheX: .res 1
RegisterCacheY: .res 1
RegisterCacheA: .res 1

; This variable is used by the macros that help make reading and writing to
; the actor structs indirected by pointers easier
ActorPointerOffsetCache: .res 1

; When adding actors each one has to know where in the OAM it
; should put its sprite data
; because actors may have differing tile amounts the offset of 
; any given actor is based on the actor before it
; so we need a place to put that when iterating through
ActorOAMOffset: .res 1

; This the point we're going to check for a collision
; It is the middle of CollidingActor
; We see if it occurs within CurrentActor
ActorCollisionPointX: .res 1
ActorCollisionPointY: .res 1
; We know the actor's top left because it is in the actor struct, but we don't know its bottom left for the box
; We figure it out during collission checking and store it here
ActorCurrentBottomRightX: .res 1
ActorCurrentBottomRightY: .res 1
CollisionOccured: .res 1

