# EvParse

A parser for EvLag output.


## Usage

Note: EvParse only works for EvLag output for mouse and keyboard
devices.  Events from other devices (e.g., touchscreen, game
controller) are logged by EvLag, but EvParse does not parse them.

Note: EvParse handles mouse movements, but for button presses, only
handles left, right and middle.


## Output

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
1.3, 0002, 0000, 0001     # EV_REL / REL_X   1
1.3, 0002, 0001, -006     # EV_REL / REL_Y  -6
1.3, 0000, 0000, 0000     # EV_SYN 

```

means that the mouse moved 1 to the right (on the x-axis) and 6 units
up (on the y-axis).  The timestamps (the number 1.3 in the first
column) are all the same since they happened at the same time.

The output:

```
1.3, 0004, 0004, 589825  # EV_MSC / MSC_SCAN             589825
1.3, 0001, 0110, 0001    # EV_KEY / BTN_LEFT             1
1.3, 0000, 0000, 0000    # EV_SYN 
1.4, 0004, 0004, 589825  # EV_MSC / MSC_SCAN             589825
1.4, 0001, 0110, 0000    # EV_KEY / BTN_LEFT             0
1.4, 0000, 0000, 0000    # EV_SYN 
```

means that the left button was pressed and then released.


### Keyboard Example

The output:

```
1.1, 0004, 0004, 458792    # EV_MSC / MSC_SCAN             458792
1.1, 0001, 001c, 0001      # EV_KEY / KEY_ENTER            1
1.1, 0000, 0000, 0000      # EV_SYN 
```

means the enter key was pressed and then released.  Note a "2" for the
event value (the 4th number).


## To Do

Handle more mouse buttons (e.g., side buttons)

Handle scroll wheel scrolling

I think there are sometimes multiple key events for one SYN.
This should be handled.

-----------------------------

Happy parsing!

-- Mark
