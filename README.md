# EvLag

A simple tool for simulating input lag in Linux.

Original source: <https://github.com/filipalac/evlag.git>  
by Filip Aláč <filipalac@gmail.com>, copyright 2018.


## Overview

This provides a stand-alone program that adds lag to to events from
input devices.  The additions to this version (over the original) are:

1) The ability to add lag to more than one input device at a time.

2) The option to log input events to a log file, one per device.

There is no need to install any additional kernel module, everything
works in userspace through libevdev and uinput.


## Compile

The libevdev package is needed:

```
  sudo apt install libevdev-lib
```

To build:

```
  make
```   


## Usage

EvLag is run as a command line tool, indicating the amount of lag and
device(s).

```
Usage: evlag [OPTION...]
            --lag <NUM> --device <FILE> [--device <FILE> ... (max 10)]

  -b, --buffer=NUM           Set size of buffer (MiB)
  -d, --device=FILE          /dev/input/eventX
  -f, --file=FILE            Logfile for events (default none)
  -h, --Hz=NUM               Set polling rate of uinput device (1 - 8192,
                             default 2048)
  -l, --lag=NUM              Set length of delay (ms)
  -p, --priority=NUM         Set scheduler priority (1 - 99, default 20)
  -r, --resize=NUM           Set resize factor of full buffer (default 2)
  -v, --verbose              Set verbose on (default off)
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

```

For example:

```
  evlag -h 8192 -l 100 -d /dev/input/event10 -d /dev/input/event6
```

Will generate additional 100 ms of delay on the event10 device (e.g.,
a mouse and a keyboard) with a polling rate of 8192 Hz.


## Output

EvLag can produce logfiles upon request.  This is done by using the
`-f NAME` flag, with NAME being used as the file prefix and `.log` as
the suffix.  This creates a logfile for each device, detecting the
device type as one of:

+ mouse
+ keyboard
+ other

The device type is used as a prefix in the name.

The logfiles are in comma separated value (CSV) format, with each row:

```
    millisec, event-type, event-code, event-value
```

The first row is the header.  The first column is the time the event
occurred (in milliseconds) relative to the start.


## Links

Some related links that may be useful:

+ LibEvDev documentation:
<https://www.freedesktop.org/software/libevdev/doc/1.4/index.html>

+ Man pages:
<https://www.freedesktop.org/software/libevdev/doc/latest/group__init.html>

+ Understand the codes and values in the logfile:
<http://who-t.blogspot.com/2016/09/understanding-evdev.html>


## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
