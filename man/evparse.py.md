% EVPARSE.PY(1) evparse.py 2.0
% Mark Claypool
% July 2021

# NAME
evparse.py - parse logfile output from evlag


# SYNOPSIS

**evparse.py** [OPTIONS] 

**evparse.py** [OPTIONS] {-i *OUT*} {-o *IN*}


# DESCRIPTION

evparse.py parses output from logfiles produced from the evlag command.

The logfile output is in comma separated value (CSV) format, with each row:

millisec, event-type, event-code, event-value

The first row is the header.  The first column is the time the event
happened (in milliseconds) relative to the start, followed by the
event type, event code and event value.


# OPTIONS

**--version**
: show program's version number and exit

**-h**, **--help**
: show this help message and exit

**-i IN**, **--in=IN**
read input from IN file

**-o OUT**, **--out=OUT**
Write output to OUT file

**-d**, **--debug**
: display debug messages

**-q**, **--quiet**
: quiet, not printing status messages to stdout


# EXAMPLES

**evparse.py ---help**

: displays a help/usage message.

**evparse.py -i basic_mouse.log -o basic_mouse.out**

: parses the file basic_mouse.log
and writes the parsed output to basic_mouse.out.


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

evlag (1), evdetect.sh (1)

For more information, see:

Shengmei Liu and Mark Claypool. "EvLag - A Tool for Monitoring and
Lagging Linux Input Devices", In *Proceedings of the ACM Multimedia
Systems Conference (MMSys)*, Istanbul, Turkey, September 28 - October
1, 2021. Online at: http://www.cs.wpi.edu/~claypool/papers/evlag/
