#!/usr/bin/python3

#
# EvParse - parse output from EvLag
#
# Copyright 2020 Mark Claypool, WPI
#
# Last signficantly modified: June 2020
#
#

FILE = "single-key.log"

# EvLag output: millisec, event-type, event-code, event-value
TIME_COL = 0
TYPE_COL = 1
CODE_COL = 2
VALUE_COL = 3
  
####################################
#

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
             100: 'RALT',
             103: 'UP',
             105: 'LEFT',
             106: 'RIGHT',
             108: 'DOWN'
  }

  return switch.get(int(num))

####################################

file = open(FILE, 'r')
line = file.readlines()

index = 1 

while (index < len(line)):

  # Next line should be EV_MSC 
  #7633, 4, 4, 458773

  # Split based on commas.
  cell = line[index].split(',')

  print(index, cell[TIME_COL], type2str(cell[TYPE_COL]))
  index += 1



  
