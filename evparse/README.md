# EvParse

A parser for EvLag output.


## Usage

Note: requires the evdev package (`pip install evdev`).

```
Usage: evparse.py [-h] [-q] {-i IN} {-o OUT}

Options:
  --version          show program's version number and exit
  -h, --help         show this help message and exit
  -i IN, --in=IN     read input from IN file.
  -o OUT, --out=OUT  Write output to OUT file.
  -d, --debug        display debug messages
  -q, --quiet        quiet, not printing status messages to stdout

```

For example:

```
 evparse.py -i ../logs/nt-mouse.log -o nt-mouse.parsed
```

```
Verbose: True
Input file: ../logs/nt-mouse.log
Output file: nt-mouse.parsed
Debug: False
```

Creating the file `nt-mouse.parsed` that looks like:

```
relative axis event at 0.001368, REL_X , X val: -1
synchronization event at 0.001368, SYN_REPORT
relative axis event at 0.001369, REL_Y , Y val: 1
synchronization event at 0.001369, SYN_REPORT
relative axis event at 0.001371, REL_X , X val: -1
synchronization event at 0.001371, SYN_REPORT
relative axis event at 0.001374, REL_X , X val: -1
relative axis event at 0.001374, REL_Y , Y val: 1
synchronization event at 0.001374, SYN_REPORT
...
```

## Details on EvDev Output

Since evdev is a serialized protocol, simultaneous events are
indicated by a "synchronous" marker.  When several actions happened at
the same time, there will be a series of events and then a EV_SYN to
indicate that the preceding events all belong together.

EvLag produces a CSV file with the following headers:

```
millisec, event-type, event-code, event-value
```

### Mouse Example

The output (`#` and following characters are comments and not in the
EvLag output file):

```
13, 0002, 0000, 0001     # EV_REL / REL_X   1
13, 0002, 0001, -006     # EV_REL / REL_Y  -6
13, 0000, 0000, 0000     # EV_SYN 

```

means that the mouse moved 1 to the right (on the x-axis) and 6 units
up (on the y-axis).  The timestamps (the number 1.3 in the first
column) are all the same since they happened at the same time.

The output:

```
23, 0004, 0004, 589825  # EV_MSC / MSC_SCAN             589825
23, 0001, 0110, 0001    # EV_KEY / BTN_LEFT             1
23, 0000, 0000, 0000    # EV_SYN 
34, 0004, 0004, 589825  # EV_MSC / MSC_SCAN             589825
34, 0001, 0110, 0000    # EV_KEY / BTN_LEFT             0
34, 0000, 0000, 0000    # EV_SYN 
```

means that the left button was pressed and then released.


### Keyboard Example

The output:

```
51, 0004, 0004, 458792    # EV_MSC / MSC_SCAN             458792
51, 0001, 0028, 0001      # EV_KEY / KEY_ENTER            1
51, 0000, 0000, 0000      # EV_SYN 
```

means the enter key was pressed.


## Example Logs

The above example logs are provided in this directory:

+ `example-keyboard.log`
+ `example-mouse.log`

-----------------------------

Happy parsing!

-- Mark
