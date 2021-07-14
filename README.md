# EvLag

A simple tool for adding lag and monitoring device input in Linux.

Copyright 2021 Mark Claypool, WPI.

Based on initial code (<https://github.com/filipalac/evlag.git>) by
Filip Aláč <filipalac@gmail.com>, copyright 2018.


## Overview

*EvLag* provides a stand-alone program that adds lag and monitors
input to devices in Linux.  Features:

0) Add a fixed (constant) amount of lag to any Linux input device.

1) Add lag to more than one input device at a time.

2) Option to log all input events to a file, one file per device.

There is no need to install any additional kernel modules - everything
works in userspace through `libevdev` and `uinput`.

The EvLag repository also includes *EvParse*, *EvDetect* and sample
log files.  See below for details.


## Directories

+ `evlag` - the EvLag source code for lagging and logging input.

+ `evparse` - the EvParse utility for parsing EvLag log files.

+ `evdetect` - the EvDetect utility for finding a particular device number.

+ `logs` - sample log files produced by EvLag.

+ `debian` - configuration files for making and hosting a Debian
  package for EvLag.


## Compile

To compile EvLag, the `libevdev` development package is needed.  On
Ubuntu, this can be installed via:

```
  sudo apt install libevdev-dev
```

To build EvLag:

```
  make
```


## Install

After successful compilation, the EvLag binary can be put anywhere
in your path (e.g., `/usr/local/bin`).

Alternatively, a Debian package is provided via the Web. See "Install
Package" in `debian/README.md`.


## Usage

EvLag is run as a command line tool, indicating the amount of lag and
the device(s) to lag.

```
Usage: evlag [OPTION...]
            --lag <NUM> --device <FILE> [--device <FILE> ... (max 10)]

  -b, --buffer=NUM           Set size of buffer (in events)
  -d, --device=FILE          Input device /dev/input/eventX to lag
  -f, --file=FILE            Logfile for events (default none)
  -h, --Hz=NUM               Set polling rate of input device (1 - 8192,
                             default 2048)
  -l, --lag=NUM              Set length of delay (ms)
  -p, --priority=NUM         Set scheduler priority (1 - 99, default 20)
  -r, --resize=NUM           Set resize factor of full buffer (default 2)
  -v, --verbose              Set verbose on (default off)
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

```

EvLag must be run as a superuser.

For example:

```
  sudo ./evlag -h 8192 -l 100 -d /dev/input/event6 -d /dev/input/event10
```

Will generate additional 100 ms of delay on the event6 and event10
devices (e.g., a mouse and a keyboard) with a polling rate of 8192 Hz.


## Output

EvLag can produce logfiles upon request by using the `-f NAME` flag,
with NAME being used as the file prefix and `.log` as the suffix.
This creates a logfile for each device, using `libevdev_get_name()` as
the device type used as a prefix in the file name.

The logfiles are in comma separated value (CSV) format, with each row:

```
    millisec, event-type, event-code, event-value
```

The first row is the header.  The first column is the time the event
happened (in milliseconds) relative to the start, followed by the
event type, event code and event value.


## Notes

### Figuring Out Which Device is What

Which device is a mouse or keyboard (or other) can be done by trial
and error, checking each input device (eventX) one by one.
Alternatively, the Linux program `evtest` might be helpful (on Ubuntu,
install via `apt install evtest`).  Sample output of `sudo evtest` on
a Linux box with a mouse (but no keyboard):

```
No device specified, trying to scan all of /dev/input/event*
Available devices:
/dev/input/event0:      Power Button
/dev/input/event1:      Sleep Button
/dev/input/event2:      Power Button
/dev/input/event3:      Video Bus
/dev/input/event4:      HDA Intel PCH Headphone
/dev/input/event5:      HDA Intel PCH HDMI/DP,pcm=3
/dev/input/event6:      PS/2+USB Mouse
```

In the above example, the mouse is `/dev/input/event6`.


### Implementation

EvLag uses the Linux event devices (`evdev`) - generic interfaces that
generalize raw input events from device drivers and makes them
available through character devices.  These character devices are, by
default, in the `/dev/input` directory.

EvLag accesses the `evdev` devices via `libevdev`, a user-space
library that abstracts the IO calls through a type-safe interface.

EvLag uses `pthreads`, creating two threads for each lagged device -
one for reading from the input device and one for writing to the input
device.  The read thread pulls input events out of the device, adds
the appropriate amount of lag, and puts them in a shared FIFO queue.
The write thread pulls events out of the FIFO queue at the appropriate
time and writes them to the device.

Psuedo-code for the reader thread:

```
READER: read_event()

  while (1):
  
    libevdev_next_event() // Pull event from device (blocking)
    timeradd()            // Add delay to event 
    lock FIFO
    enqueue event         // Add to queue for writer
	unlock FIFO

  end while
```

Pseudo-code for the writer thread:

```
WRITER: write_event()

  while (1):
  
    gettimeofday()
    if event expired:
  	  libevdev_uinput_write_event()  // Push event to device
      lock FIFO
   	  dequeue event                  // Get next event from reader
      unlock FIFO
    end if
	
	pthread_cond_wait()    // Block until next interrupt from main()

  end while
```

The main thread uses the real time clock drivers for Linux, accessed
through `/dev/rtc`, to get interrupts at the desired polling rate.
The main thread monitors these by doing a `read()`, which blocks until
the next interrupt is received.  This allows for a high frequency of
polling without being CPU bound, say, by polling `gettimeofday()`.

When the main thread does get an interrupt, it signals all the writer
threads.

```
MAIN:

  open /dev/rtc
  ioctl()           // set interrupt/polling rate

  for each device:

    setup FIFO queue shared by reader-writer
	create reader thread
	create writer thread

  end for
  
  while (1):

    read /dev/rtc
    pthread_cond_broadcast()  // inform writer of "tick"

  end while
```

### Virtual Machines

When using in a virtual machine, some care may need to be taken when
lagging input depending upon how the input devices are shared with the
host OS.  For example, with Oracle Virtual Box, with the "mouse
integration" setting (the default), EvLag does not receive the mouse
movement commands and so cannot delay (or log) the mouse movement.
Evlag can still delay (and log) mouse clicks and the keyboard.  The
"mouse integration" setting can be toggled on and off and when off,
EvLag works as expected (i.e., the mouse movements can be delayed).
Note, in this "off" mode, the right control key can be used to
re-capture the mouse by the host OS.


## Links

For more information, see:

> Shengmei Liu and Mark Claypool. "EvLag - A Tool for Monitoring and
> Lagging Linux Input Devices", In *Proceedings of the ACM Multimedia
> Systems Conference (MMSys)*, Istanbul, Turkey, September 28 - October
> 1, 2021. Online at: <http://www.cs.wpi.edu/~claypool/papers/evlag/>

Some related links that may be useful:

+ EvDev Wiki:  
<https://en.wikipedia.org/wiki/Evdev>

+ LibEvDev documentation:  
<https://www.freedesktop.org/software/libevdev/doc/1.4/index.html>

+ LibEvDev man pages:  
<https://www.freedesktop.org/software/libevdev/doc/latest/group__init.html>

+ Understand the codes and values in the logfile:  
<http://who-t.blogspot.com/2016/09/understanding-evdev.html>  
<https://www.kernel.org/doc/Documentation/input/event-codes.txt>  
<https://elinux.org/images/9/93/Evtest.c>


## To Do

A partial list of possible next steps/features includes:

+ Provide ability to delay multiple devices with different amounts of
delay for each.

+ Add option to add delay variance (jitter).


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
