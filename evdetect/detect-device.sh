#!/bin/bash

#
# Mark Claypool, WPI
#
# Last signficantly modified: July 2020.
#
# Script to detect input device for mouse or keyboard.
# 

#DEV_FILE=/proc/bus/input/devices
DEV_FILE=intel.devices

VERBOSE=1

# Print usage message and quit.
function usage {
  echo "detect-device - detect event number for specific device"
  echo "  usage: dev-ev.sh [-qh] {device-string}"
  echo "         device string to match"
  echo "         -q quiet"
  echo "         -h this help message"
  echo "  example: dev-ev.sh -q Keyboard"
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
  echo "Device: $1"
fi

# Find matching device.
line=`cat $DEV_FILE | \
  grep 'Name' | \
  cat -n | \
  grep -i $1`

if [ "$VERBOSE" = 1 ] ; then
  echo "Line: $line"
fi

# Parse
IFS=' '
read -a tokens <<< "$line"
first=${#tokens[0]}
num="$(($first-1))"

echo $first
echo $num

echo "/dev/input/event$num"

exit 0

if [ "$VERBOSE" = 1 ] ; then
  echo "device line: $line"
fi

if [ "$VERBOSE" = 1 ] ; then
  echo "device event: $event"
fi

echo "/dev/input/$event"


    
