# tarpe-2d
TARPE is an acronym for yeT Another Rigidbody Physics Engine. It's very basic: only gravity (all bodies attracts to each other) and basic impulse collision resoluton are implemented; and there're only 2 shapes: circles and rectangles (OBBs).
It uses:
- Barnes-Hut algorithm to simulate gravity
- SAT to detect collisions
- Clipping to find contact points between rectangles (it's kinda buggy right now)
- Basic impulse-based collision resolution system (sequential impulses will be implemented after clipping is fixed)

## Building
By itself, tarpe-2d uses only C standard library (you need to link libm, because math functions implementation is separated from libc).
So, to build it you basically need only a compiler and libc implementation (e.g. gcc and glibc).
But in that case you will have to do it by hands, so there's Makefile.
To use it you need a make implementation (e.g. GNU Make).
To build static library with make you need to type `make lib_static` in you termimal in the root of the engine. Built lib will be `lib/libtarpe2d.a`
### Graphical example
To build graphical example you need to have SDL2 and SDL_gpu installed.
Then just type `make` in you termimal in the root of the engine. Built executable will be `bin/main`. You can run it by yourself or just run `make run` (it will also build the executable if there's no, so you can skip typing `make`)
