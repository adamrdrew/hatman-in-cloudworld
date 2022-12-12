# Hatman in Cloudworld

![Game Screenshot](.github/hatman.png "a title")

A simple single-screen platformer for the NES written in 6502 Assembly! This was a project I did for Hackathon Dec 22 @ Red Hat. It was a ton of fun and I learned a lot.

This is a work in progress. I do hope to finish the game eventually, but right now its only a couple of levels. It is also just barely within the vblank timing budget so it needs a lot of slimming down.

## Requirements
You'll need the following:
* The CA65 assmbler (part of the CC65 suite) that you can [Download Here](https://cc65.github.io/getting-started.html)
* An NES emulator. I recommend either Messen or FCEUX.

## Build
If you have CA65 installed and on your path simply run:

```bash
$ make build
```

And a `.nes` file will be generated that you can run in your emulator. If you have Messen or FCEUX on your path you can run either of the following:

```bash
$ make run-messen
$ make run-fceux
```

And it will build and run in your emulator.

## Hacking and Developing
This is Free Software so please feel free to fork and patches are very much welcome :) For development I *highly* recommend using the Messen emulator. Its debugger is fantastic and it really helps with tracking stuff down. Our make targets build with debug symbols so it makes things really nice to debug in Messen.

## Resources Consulted
I went from never having done any NES development before to a decent chunk of a game in a week, thanks to these awesome resources:

* [NES Development Enviroment by NESHacker](https://www.youtube.com/watch?v=RtY5FV5TrIU)
* [CA65 Macro Assembler Language Support for VSCode](https://github.com/tlgkccampbell/code-ca65)
* [NESHacker's Demo Project](https://github.com/NesHacker/DevEnvironmentDemo)
* [NESHacker's 6502 Assembly Crash Course](https://www.youtube.com/playlist?list=PLgvDB6LWam2WvoFvh8tlUqbqw92qWM0aP)
* [NES Game Programming Course](https://courses.pikuma.com/courses/take/nes-game-programming-tutorial)
* [NES Dev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
* [6502 OpCode List](http://www.6502.org/tutorials/6502opcodes.html)
* [NES LightBox](https://famicom.party/neslightbox/)
* [This made me completely insane for an hour](https://yeahexp.com/why-in-mos-6502-does-the-sbc-subtract-2-instead-of-1-the-first-time/)
* [Middle Engine NES Programming Guide](https://www.middle-engine.com/blog/posts/2020/06/23/programming-the-nes-the-6502-in-detail)
* [6502 Division Algos](https://mdfs.net/Info/Comp/6502/ProgTips/6502Divide)

Special shout-out to [Gustavo Pezzi](https://github.com/gustavopezzi) - his course [NES Game Programming](https://courses.pikuma.com/courses/take/nes-game-programming-tutorial) was my roadmap and really got me off to a great start. Much of the work in this project (the Actor system for example) scales-up and builds-on his work. He put together a fantastic course and I recommend it highly to anyone who wants to learn NES development.