#!/bin/bash

#
# Mark Claypool, WPI
#
# Last signficantly modified: July 2020.
#
# Script to detect input device for mouse or keyboard.
# 

VERSION=v1.0

# Device input file.
DEV_FILE=/proc/bus/input/devices
#DEV_FILE=devices.sample

# Defaults to verbose.
VERBOSE=1

# Print usage message and quit.
function usage {
  echo "EvDetect ($VERSION) - detect event number for particular device"
  echo "  usage: evdetect.sh [-qh] {str}"
  echo "         str - device string to match"
  echo "         -q quiet"
  echo "         -h this help message"
  echo "  example: evdetect.sh -q \"Gaming Keyboard\""
  exit 0
}

# Parse command line args.
while getopts "h?q" opt; do
  case "$opt" in
    h|\?)
    usage
    exit 0
    ;;
  q)  VERBOSE=0
    ;;
  esac
done
shift $((OPTIND-1))
if [ $# != 1 ] ; then
  usage
fi

# Print options.
if [ "$VERBOSE" = 1 ] ; then
  echo "Option: verbose"
  echo "Device: '$1'"
fi

# Find matching device, numbered starting at 0.
names=$(cat $DEV_FILE | \
  grep 'Name' | \
  cat -n)
line=$(echo "$names" | \
  grep 'Name' | \
  grep -i "$1" | \
  head -n 1)

# Parse number.
num=$(echo $line | cut -f1 -d' ')
num="$(($num-1))"

if [ "$VERBOSE" = 1 ] ; then
  echo "Names:"
  echo "$names"
  echo "Match: " "$line"
  echo "Event: $num"
fi

echo "/dev/input/event$num"

exit 0
