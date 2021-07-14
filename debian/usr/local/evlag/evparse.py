#!/usr/bin/python3

#
# EvParse - parse output from EvLag
#
# Copyright 2020 Mark Claypool, WPI
#
# Last signficantly modified: July 2020
#

import sys
from optparse import OptionParser
import evdev
from evdev import events, categorize

VERSION = 2.0

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
# Print line number and warning.
def warn(str):
  global g_line_number

  print("Line:", g_line_number, " Warning!", str, file=sys.stderr)

###########################################
# Parse line: millisec, event-type, event-code, event-value
def parse_line(line, is_mouse=False):

  global g_lines
  global g_options

  if line >= len(g_lines):
    warn("parse_line(): past end of file")
    return
  
  cell = g_lines[line].split(',')
  if len(cell) != 4:
    warn("parse_line(): too few cells")
    return 'Error'
  
  event_time = int(cell[TIME_COL])
  event_type = int(cell[TYPE_COL])
  event_code = int(cell[CODE_COL])
  event_value = int(cell[VALU_COL])

  if g_options.debug:
    print(line, event_time, event_type, event_code, event_value)
    
  return event_time, event_type, event_code, event_value

###########################################
def main():
  
  global g_lines
  global g_line_number
  global g_outfile
  global g_options

  # Setup command line arguments
  parser = OptionParser(usage="%prog [-h] [-q] {-i IN} {-o OUT}",
                      version="%prog, v" + str(VERSION))
  parser.add_option("-i", "--in", dest="inname",
                    help="read input from IN file.", metavar="IN")
  parser.add_option("-o", "--out", dest="outname",
                    help="Write output to OUT file.", metavar="OUT")
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
  if g_options.inname is None:
    parser.error("Must define \"-i IN\"")
  if g_options.outname is None:
    parser.error("Must define \"-o OUT\"")

  if g_options.verbose:
    print("Verbose: True")
    print("Input file: %s" % g_options.inname)
    print("Output file: %s" % g_options.outname)
    if g_options.debug:
      print("Debug: True")
    else:
      print("Debug: False")

  # Prepare infile and outfile.      
  g_outfile = open(g_options.outname, 'w')
  infile = open(g_options.inname, 'r')
  g_lines = infile.readlines()
  g_line_number = 1 

  # Repeat until consumed all lines.
  while (g_line_number < len(g_lines)):

    # Parse line.
    event_time, event_type, event_code, event_value = parse_line(g_line_number)

    # Get built-in codes.
    ev = events.InputEvent(0, event_time, event_type, event_code, event_value)

    if g_options.debug:
      print(ev)
      
    try:
      g_outfile.write(str(categorize(ev)))
      if (ev.type == evdev.ecodes.EV_REL):
        if (ev.code == evdev.ecodes.REL_X):
          g_outfile.write(", X val: ")
          g_outfile.write(str(ev.value))
        if (ev.code == evdev.ecodes.REL_Y):
          g_outfile.write(", Y val: ")
          g_outfile.write(str(ev.value))
      g_outfile.write("\n")
    except KeyError:
      g_outfile.write("unknown: ")
      g_outfile.write(str(ev))
      g_outfile.write("\n")
      
    g_line_number += 1

###########################################

if __name__ == "__main__":
    main()


