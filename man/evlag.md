% EVLAG(1) evlag 2.5
% Mark Claypool
% July 2021

# NAME
evlag - add lag and monitor input for device(s)


# SYNOPSIS

**evlag** [OPTION]

Must be run as a superuser.


# DESCRIPTION

evlag is a command line tool that adds lag to one or more Linux
devices.  evlag can also log the events to a file, with a separate
file per device.


# OPTIONS

**-b**, **--buffer=NUM**
: Set the size of the device buffer (in events) (default 40)

**-d, --device=FILE**
: Specify the input device /dev/input/eventX to lag/log

**-f**, **--file=FILE**
: Specify the logfile for events (default none)

**-h**, **--Hz=NUM**
: Set the polling rate of the input device(s) (1 - 8192, default 2048)

**-l**, **--lag=NUM**
: Set the length of delay (in milliseconds) (default 0)

**-p**, **--priority=NUM**
: Set the scheduler priority (1 - 99, default 20).  Lower is higher priority.

**-r, **--resize=NUM**
: Set the resize factor for increasing the buffer when full (default 2x)

**-v**, --verbose**
: Set verbosity on to display evlag settings and options (default off)

**-?**, --help**
: List the command line options

**--usage**
: Provide a short usage message

**-V**, **--version**
: Print the program version


# EXAMPLES

**sudo ./evlag -l 10 -d /dev/input/event3**

: adds 10 milliseconds of delay to the event3 device (e.g., a gamepad)
with a polling rate of 8192 Hz.

**sudo ./evlag -h 8192 -l 100 -d /dev/input/event6 -d /dev/input/event10**

: adds 100 milliseconds of delay to the event6 and event10 devices
(e.g., a mouse and a keyboard) with a polling rate of 8192 Hz.

**sudo ../evlag/evlag -d /dev/input/event10 -f out**

: logs output from the event10 device (e.g., a mouse) to a file
prefixed with *out*.  Since *-l* is not specified, this does not add
any lag.


# OUTPUT

If specified, the logfile output is in comma separated value (CSV)
format, with each row:

millisec, event-type, event-code, event-value

The first row is the header.  The first column is the time the event
happened (in milliseconds) relative to the start, followed by the
event type, event code and event value.


# EXIT VALUES

SIG if exited due to a signal (e.g., SIGINT)

EXIT_FAILURE if exit due to some error (e.g., lack of sudo permission)


# BUGS

Evlag has been known to infrequently stop logging device events in the
middle of execution.


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

evparse.py(1), evdetect.sh(1)

For more information, see:

Shengmei Liu and Mark Claypool. "EvLag - A Tool for Monitoring and
Lagging Linux Input Devices", In *Proceedings of the ACM Multimedia
Systems Conference (MMSys)*, Istanbul, Turkey, September 28 - October
1, 2021. Online at: http://www.cs.wpi.edu/~claypool/papers/evlag/
