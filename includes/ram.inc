.SEGMENT "RAM"

; The swap space used when creating a new actor
NewActor:   .res ACTOR_SIZE

; The array of actors
Actors:     .res ACTOR_SIZE * MAX_ACTORS