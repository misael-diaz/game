# Low-Level Game Dev in GNU/Linux
I am exploring what it takes to build a game in GNU/Linux from scratch, no frameworks,
just using the `cstdlib` and some facilities of the Linux Kernel for handling user input
and for graphics display.

Not trying to reinvent a framework here so if your hardware does not match mine this code
will not work in your machine. At this point I do not know how far I will be able to
progress, this is a repository for learning. I am just writing code based on the
Linux Kernel
[subsystems API documentation](https://www.kernel.org/doc/html/latest/subsystem-apis.html#human-interfaces),
[GLIBC Reference Manual](https://sourceware.org/glibc/manual/2.41/html_mono/libc.html),
the Linux [man pages](https://man7.org/linux/man-pages/man1/man.1.html), and some
stackoverflow posts.
I am adding references to the source code to keep track of the resources that
I have found to be most useful to me. In the end, what I want is not to build a game
but to learn more about low-level programming and GNU/Linux by trying to build a game.

The game has been inspired by this [Let's Build a Game Tutorial Series in Java](https://www.youtube.com/watch?v=1gir2R7G9ws&list=PLWms45O3n--6TvZmtFHaCWRZwEqnz2MHa). Great place
to start if you are new to game development.

## Compile

```
make clean && make
```

## Execute

You need to elevate your permissions to root level otherwise the application will get
a permissions error when trying to open the keyboard device for reading. Not ideal,
don't like this but that's what am doing for now to test it.

```
./bin/game.bin
```

## EPILEPSY WARNING

The game screen will **flicker** so if you know that you or a relative has this condition
do not play this game. We have yet to implement a buffer strategy to mitigate this issue.
All in all even if you try to run this in your Linux machine the executable shouldn't
produce images unless you are in plain console mode. You will have to switch to console
mode to play this game.

## Game Controls

Use the keyboard arrow keys to move the player to try to avoid collisions with the
enemies.

To quit the game simply press the `Esc` key.
