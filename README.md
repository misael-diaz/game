# game
experimenting building a game in Linux from scratch

## Compile

```
gcc -Wall -O0 -g game.c -o game.bin
```

## Execute

You need to elevate your permissions to root level otherwise the application will get
a permissions error when trying to open the keyboard device for reading. Not ideal,
don't like this but that's what am doing for now to test it.

```
./game.bin
```

To quit the game simply press `Esc` key.

## Notes

If you try to run this code it's likely that it won't work unless you happen to have
the same keyboard so you will have to modify the device name accordingly.
