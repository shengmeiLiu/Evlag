#!/bin/bash

#
# Mark Claypool, WPI
#
# Last signficantly modified: July 2020.
#
# Script to detect input device for mouse or keyboard.
# 

VERSION=v1.2

# Device input file.
DEV_FILE=/proc/bus/input/devices
#DEV_FILE=devices.sample.1
#DEV_FILE=devices.sample.2

# Devices: Number of lines after "Name" for the "Handlers".
HANDLERS_OFFSET=6

# Defaults to verbose (-q for quiet).
VERBOSE=1

# Print usage message and quit.
function usage {
  echo "EvDetect ($VERSION) - detect event number for particular device"
  echo "  usage: evdetect.sh [-qh] {str}"
  echo "         str - device string to match (case sensitive)"
  echo "         -q quiet"
  echo "         -h this help message"
  echo "  example: evdetect.sh -q \"Keyboard\""
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

# Find first match for str.
match=$(cat $DEV_FILE | \
  grep "$1" -A $HANDLERS_OFFSET | \
  head -n 5 | \
  tail -n 1)

# Parse out device event.
words=($(echo $match | sed 's/\s/\n/g'))
event=($(
  for i in "${words[@]}" ; do  
    echo $i | grep event
  done
))

if [ "$VERBOSE" = 1 ] ; then
  echo "Match: " "$match"
  echo "Event: $event"
fi

# Display string.
echo "/dev/input/$event"

exit 0
