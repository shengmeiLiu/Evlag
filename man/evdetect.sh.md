% EVDETECT.SH(1) evdetect.sh 2.0
% Mark Claypool
% July 2021

# NAME
evdetect.sh - detect input device for mouse or keyboard


# SYNOPSIS

**evparse.py** [OPTION] {STR}


# DESCRIPTION

evdetect.sh detects the event device number for a specified device
string.  This allows evdetect.sh to be used in conjunction with
evlag(1) for scripting functionality.

# OPTIONS

**str**
: device string to match (case sensitive)

**-h**
: show this help message and exit

**-q**
quiet mode on (default off)


# EXAMPLE

**evdetect.sh -q "Keyboard"

: detects the device input matching the string "keyboard"
and displays just this match (e.g., /dev/input/event6).


# COPYRIGHT

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.


# AUTHOR

Mark Claypool, WPI


# SEE ALSO

evlag(1), evparse.py(1)

For more information, see:

Shengmei Liu and Mark Claypool. "EvLag - A Tool for Monitoring and
Lagging Linux Input Devices", In *Proceedings of the ACM Multimedia
Systems Conference (MMSys)*, Istanbul, Turkey, September 28 - October
1, 2021. Online at: http://www.cs.wpi.edu/~claypool/papers/evlag/
