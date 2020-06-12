#!/usr/bin/python3

#
# EvParse - parse output from EvLag
#
# Copyright 2020 Mark Claypool, WPI
#
# Last signficantly modified: June 2020
#

#VERBOSE = True
VERBOSE = False

#ToDo:

#Input file command line (blah.log)
#Output file (blah.csv)
#Options -o file name
#Options -v verbose
#Options -h help

FILE = "single-key.log"
#FILE = "basic-keyboard.log"

# EvLag output: millisec, event-type, event-code, event-value
TIME_COL = 0
TYPE_COL = 1
CODE_COL = 2
VALUE_COL = 3
  
####################################
# Utility functions

# Print line number and warning.
def warn(str):
  global line_number

  print("Line:", line_number, " Warning!", str)

# Parse line: millisec, event-type, event-code, event-value
def parse_line(line):

  global lines
  if (line >= len(lines)):
    warn("parse_line(): past end of file")
    return
  
  cell = lines[line].split(',')
  if (len(cell) != 4):
    warn("parse_line(): too few cells")
    return 'Error'
  
  event_time = cell[TIME_COL]
  event_type = type2str(cell[TYPE_COL])
  event_code = key2str(cell[CODE_COL])
  event_value = val2str(cell[VALUE_COL])

  if (VERBOSE):
    print(line, event_time, event_type, event_code, event_value)
    
  return event_time, event_type, event_code, event_value

####################################
# EvDev functions

# Convert event type to string.
def type2str(num):

  switch = {
    0: 'EV_SYN', 
    1: 'EV_KEY',
    2: 'EV_REL',
    3: 'EV_ABS',
    4: 'EV_MSC',
    5: 'EV_SW',
    6: 'EV_LED',
    7: 'EV_SND',
    8: 'EV_REP',
    9: 'EV_FF',
    10: 'EV_PWR',
    11: 'EV_FF_STATUS'
  }

  return switch.get(int(num))

# Convert mouse events to string.
def mouse2str(num):

  switch = {
    110: 'BTN_MOUSE',
    110: 'BTN_LEFT',
    111: 'BTN_RIGHT',
    112: 'BTN_MIDDLE',
    113: 'BTN_SIDE',
    114: 'BTN_EXTRA',
    115: 'BTN_FORWARD',
    116: 'BTN_BACK',
    117: 'BTN_TASK',
  }

  return switch.get(int(num))

# Convert key code to string.
def key2str(num):

  switch = { 0: 'NONE',
             1: 'ESC',

             # Numbers
             2: '1',
             3: '2',
             4: '3',
             5: '4',
             6: '5',
             7: '6',
             8: '7',
             9: '8',
             10: '9',
             11: '0',
             
             12: '-',
             13: '=',
             14: 'BKSP',
             15: 'TAB',
             16: 'Q',
             17: 'W',
             18: 'E',
             19: 'R',
             20: 'T',
             21: 'Y',
             22: '',
             23: 'I',
             24: 'O',
             25: 'P',
             26: '[',
             27: ']',
             28: 'CRLF',
             29: 'LCTRL',
             30: 'A',
             31: 'S',
             32: 'D',
             33: 'F',
             34: 'G',
             35: 'H',
             36: 'J',
             37: 'K',
             38: 'L',
             39: ';',
             40: '"',
             41: '`',
             42: 'LSHIFT',
             43: '\\',
             44: 'Z',
             45: 'X',
             46: 'C',
             47: 'V',
             48: 'B',
             49: 'N',
             50: 'M',
             51: ',',
             52: '.',
             53: '/',
             54: 'RSHIFT',
             56: 'LALT',
             57: 'SPACE',
             58: 'CAPSLOCK',
             100: 'RALT',

             # Function keys
             59: 'F1',
             60: 'F2', 
             61: 'F3', 
             62: 'F4',
             63: 'F5',
             64: 'F6',
             65: 'F7',
             66: 'F8',
             67: 'F9',
             68: 'F10',
             87: 'F11',
             88: 'F12',

             # Keypad
             71: 'KEY_KP7',
	     72: 'KEY_KP8',
	     73: 'KEY_KP9',
	     74: 'KEY_KPMINUS',
	     75: 'KEY_KP4',
	     76: 'KEY_KP5',
	     77: 'KEY_KP6',
	     78: 'KEY_KPPLUS',
	     79: 'KEY_KP1',
	     80: 'KEY_KP2',
	     81: 'KEY_KP3',
	     82: 'KEY_KP0',
	     83: 'KEY_KPDOT',
             98: 'KEY_KPSLASH',

             # Arrow keys
             103: 'UP',
             105: 'LEFT',
             106: 'RIGHT',
             108: 'DOWN'
  }

  return switch.get(int(num))

# Convert value to string.
def val2str(num):

  switch = { 0: 'UP',
             1: 'DOWN',
             2: 'REPEAT',
  }

  return switch.get(int(num), 'UNDEFINED')

####################################
# Get next keyboard input
# Return -1 of all ok, else time, code, val.
def get_key():

  global lines
  global line_number
  
  # Format should be:
  # 816, 4, 4, 458756
  # 816, 1, 30, 1
  # 816, 0, 0, 0
  
  time, typ, code, val = parse_line(line_number)

  # First line is EV_MSC
  if (typ != 'EV_MSC'):
    warn("get_key(): Expected EV_MSC")
    return False
  line_number += 1

  # Second line is EV_KEY
  time, typ, code, val = parse_line(line_number)
  if (typ != 'EV_KEY'):
    warn("get_key(): Expected EV_KEY")
    return False
  line_number += 1

  print(time, code, val)

  # Third line is SYN
  time, typ, code, val = parse_line(line_number)
  if (typ != 'EV_SYN'):
    warn("get_key(): Expected EV_SYN")
    return False
  line_number += 1

  # Handle key repeats
  if (line_number < len(lines)):
    time, typ, code, val = parse_line(line_number)
    while (val == 'REPEAT' and line_number < len(lines)):
      print(time, code, val)
      line_number += 1
      time, typ, code, val = parse_line(line_number)
      if (typ != 'EV_SYN' and code != 'NONE' and code != 'DOWN'):
        warn("get_key(): Expected EV_SYN")
        return False
      line_number += 1
      time, typ, code, val = parse_line(line_number)

  return True
  
####################################

file = open(FILE, 'r')
lines = file.readlines()

line_number = 1 

while (line_number < len(lines)):

  if (get_key() == False):
    warn("main(): Error in get_key(). Exiting.")
    break;
