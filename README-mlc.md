# Adding Lag to User Input

May 2020

This document provides some information on adding input lag to user
input on Linux.  Potentially useful for, say, lag studies that emulate
cloud-based games.

--------------------------------------------

## EvLag

The base program is EvLag (Filip Aláč <filipalac@gmail.com>):

https://github.com/filipalac/evlag.git

Billed as simple tool for simulating input lag.  Everything works in
userspace through libevdev and uinput.


## Compile

```
  sudo apt install libevdev-lib
```

Also, may need to add to Makefile:

```
CFLAGS := $(CFLAGS) -I/usr/include/libevdev
LIBS := $(LIBS) -levdev
```   

## Tracer

Tracer is an ECS Liva X2 running Linux.

On tracer, the mouse seems to be on event3.  So, to add 200 ms of lag:

sudo ./evlag -d /dev/input/event3 -l 200 -h 1000

The keyboard is on event 7.  It seems to have some odd spaces
at the beginning until you hit a key.  Need to test with a game.

sudo ./evlag -d /dev/input/event7 -l 500 -h 1000

+ This is using the mouse when connected to the Beklin USB humouse
setup.  If I use a different port, it may be different.  Trial and
error found it the first time.

+ Also, mice, mouse0 and mouse1 don't seem to work.


## Extensions

EvLag works as advertised but, unfortunately, only lets you add input
lag to one device at a time (e.g., either the Mouse or the Keyboard
but not the Mouse *and* the Keyboard).

After doing some code review, modifications to the code should be
possible.  Basically, have one RTC (with one corresponding polling
rate) but allow for more than one device specified on the command
line.  Then, for the data structures (including threads) make them an
array of values, where each thread-pair can handle one device.

The code `rctest.c` is some test code for the RTC clock, downloaded
from the Internet in case it is useful for development.

Other links that may be useful:

+ LibEvDev documentation: <https://www.freedesktop.org/software/libevdev/doc/1.4/index.html>

+ Man pages: <https://www.freedesktop.org/software/libevdev/doc/latest/group__init.html>

--------------------------------------------

Happy lagging!

-- Mark
