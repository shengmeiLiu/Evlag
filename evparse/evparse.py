#!/usr/bin/python3

#
# EvParse - parse output from EvLag
#
# Copyright 2020 Mark Claypool, WPI
#
# Last signficantly modified: June 2020
#

import sys
from optparse import OptionParser

VERSION = 1.0

###########################################
# Global constants.

# EvLag output: millisec, event-type, event-code, event-value
TIME_COL = 0
TYPE_COL = 1
CODE_COL = 2
VALU_COL = 3

###########################################
# Parse command line.
def parse_command_line():
   if len(sys.argv) > 1:
      usage()

###########################################
# Utility functions

# Print line number and warning.
def warn(str):
  global g_line_number

  print("Line:", g_line_number, " Warning!", str, file=sys.stderr)

# Parse line: millisec, event-type, event-code, event-value
def parse_line(line):

  global g_lines
  global g_options

  if (line >= len(g_lines)):
    warn("parse_line(): past end of file")
    return
  
  cell = g_lines[line].split(',')
  if (len(cell) != 4):
    warn("parse_line(): too few cells")
    return 'Error'
  
  event_time = cell[TIME_COL]
  event_type = type2str(cell[TYPE_COL])
  event_code = key2str(cell[CODE_COL])
  event_value = val2str(cell[VALU_COL])

  if (g_options.debug):
    print(line, event_time, event_type, event_code, event_value)
    
  return event_time, event_type, event_code, event_value

###########################################
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

###########################################
# Get next keyboard input
# Return -1 of all ok, else time, code, val.
def get_key():

  global g_lines
  global g_line_number
  
  # Format should be:
  # 816, 4, 4, 458756
  # 816, 1, 30, 1
  # 816, 0, 0, 0

  time, typ, code, val = parse_line(g_line_number)

  # First line is EV_MSC
  if (typ != 'EV_MSC'):
    warn("get_key(): Expected EV_MSC")
    return False
  g_line_number += 1

  # Second line is EV_KEY
  time, typ, code, val = parse_line(g_line_number)
  if (typ != 'EV_KEY'):
    warn("get_key(): Expected EV_KEY")
    return False
  g_line_number += 1

  if g_options.debug:
    print(time, code, val)

  if val == 'DOWN' or val == 'UP':
    g_outfile.write(time + "," + code + "," + val + "\n")
    
  # Third line is SYN
  time, typ, code, val = parse_line(g_line_number)
  if (typ != 'EV_SYN'):
    warn("get_key(): Expected EV_SYN")
    return False
  g_line_number += 1

  # Handle key repeats
  if (g_line_number < len(g_lines)):
    time, typ, code, val = parse_line(g_line_number)
    while (val == 'REPEAT' and g_line_number < len(g_lines)):
      if g_options.debug:
        print(time, code, val)
      g_line_number += 1
      time, typ, code, val = parse_line(g_line_number)
      if (typ != 'EV_SYN' and code != 'NONE' and code != 'DOWN'):
        warn("get_key(): Expected EV_SYN")
        return False
      g_line_number += 1
      time, typ, code, val = parse_line(g_line_number)

  return True
  
###########################################
def main():
  
  global g_lines
  global g_line_number
  global g_outfile
  global g_options

  # Setup command line arguments
  parser = OptionParser(usage="%prog [-h] [-q] {-k|-m} {-i IN} {-o OUT}",
                      version="%prog, v" + str(VERSION))
  parser.add_option("-i", "--in", dest="inname",
                    help="read input from IN file.", metavar="IN")
  parser.add_option("-o", "--out", dest="outname",
                    help="Write output to OUT file.", metavar="OUT")
  parser.add_option("-k", action="store_true", dest="keyboard",
                    help="parse keyboard output.")
  parser.add_option("-m", action="store_true", dest="mouse",
                    help="parse mouse output.")
  parser.add_option("-d", "--debug",
                    action="store_true", dest="debug", default=False,
                    help="display debug messages")
  parser.add_option("-q", "--quiet",
                    action="store_false", dest="verbose", default=True,
                    help="quiet, not printing status messages to stdout")
  (g_options, args) = parser.parse_args()

  # Check if command line options correct.
  if len(args) != 0:
    parser.error("Incorrect number of arguments: " + str(len(args)))
  if g_options.mouse is None and g_options.keyboard is None:
    parser.error("Must define -m or -k")
  if not g_options.mouse is None and not g_options.keyboard is None:
    parser.error("Must define only one of -m or -k")
  if g_options.inname is None:
    parser.error("Must define \"-i IN\"")
  if g_options.outname is None:
    parser.error("Must define \"-o OUT\"")

  if g_options.verbose:
    print("Verbose: True")
    print("Input file: %s" % g_options.inname)
    print("Output file: %s" % g_options.outname)
    if g_options.mouse:
      print("Mouse: True")
    if g_options.keyboard:
      print("Keyboard: True")
    if g_options.debug:
      print("Debug: True")
    else:
      print("Debug: False")

  # Prepare infile and outfile.      
  g_outfile = open(g_options.outname, 'w')
  infile = open(g_options.inname, 'r')
  g_lines = infile.readlines()
  g_line_number = 1 

  # Header for CSV file.
  if g_options.keyboard:
    g_outfile.write("Time,Key,Status" + "\n")
  if g_options.mouse:
    g_outfile.write("FIX!")

  # Repeat until consumed all lines.
  while (g_line_number < len(g_lines)):

    if g_options.keyboard:
      if (get_key() == False):
        warn("main(): Error in get_key().")

###########################################

if __name__ == "__main__":
    main()


