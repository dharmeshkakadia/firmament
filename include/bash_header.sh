#!/bin/bash

PROJECT="Firmament"

export PROJECT

function print_hdr {
  echo "+---------------------------------------------------------------------"
  echo "| $1 "
  echo "+---------------------------------------------------------------------"
}

function print_subhdr {
  echo "--> $1"
}

# Column number to place the status message
RES_COL=60
# Command to move out to the configured column number
MOVE_TO_COL="echo -en \\033[${RES_COL}G"
# Command to set the color to SUCCESS (Green)
SETCOLOR_SUCCESS="echo -en \\033[1;32m"
# Command to set the color to FAILED (Red)
SETCOLOR_FAILURE="echo -en \\033[1;31m"
# Command to set the color back to normal
SETCOLOR_NORMAL="echo -en \\033[0;39m"
 
# Function to print the SUCCESS status
echo_success() {
  $MOVE_TO_COL
  echo -n "["
  $SETCOLOR_SUCCESS
  echo -n $"  OK  "
  $SETCOLOR_NORMAL
  echo -n "]"
  echo -ne "\r"
  return 0
}
 
# Function to print the FAILED status message
echo_failure() {
  $MOVE_TO_COL
  echo -n "["
  $SETCOLOR_FAILURE
  echo -n $"FAILED"
  $SETCOLOR_NORMAL
  echo -n "]"
  echo -ne "\r"
  return 1
}

print_succ_or_fail() {
  if [ $1 ]; then
    echo_success
  else
    echo_failure
  fi
  echo
}