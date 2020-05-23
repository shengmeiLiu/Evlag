# Evlag

A simple tool for simulating input lag in Linux.

Original source: `https://github.com/filipalac/evlag.git`  
by Filip Aláč <filipalac@gmail.com>, copyright 2018.


## Overview

This provides a stand-alone program that adds lag to to events from
input devices.

The primary additions to this version are:

1) The ability to add lag to more than one input device
at a time.

2) The ability to log input events to a log file.


## Compile

Libevdev needed:

```
  sudo apt install libevdev-lib
```

Then:

```
  make
```   


## Usage

Mark: Update


For example:

```
  evlag -d /dev/input/event16 -l 200 -h 8192
```

Will generate additional 200 ms of delay on device(in my case trackpoint)
with consumer polling rate(rate at which are events written to virtual device)
of 8192 Hz.

No need to install any additional kernel module, everything works in
userspace through libevdev and uinput.


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
