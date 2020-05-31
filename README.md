# GenChord

A chord generator using midi input and automatable variables.
The plugin takes one note as midi input and generates a triad chord on top of it.
By changing the Chord Type you can change the chord "type" or "color".
[0 - 0.125) -> midi passthrough (no chord)
[0.125 - 0.375) -> major 
[0.375 - 0.625) -> minor 
[0.625 - 0.875) -> augmented
[0.875 - 1)     -> disminished

Note:
For now it has a bug, if you change the chord type while playing some notes do not receive the "note off" signal 
