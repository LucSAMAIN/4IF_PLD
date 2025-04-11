#!/bin/sh

# This wrapper script will be invoked from the test harness every time it needs to run your compiler.
#
# The idea is to let you adapt command-line arguments and options so that everything works smoothly together.


# Our test harness will always execute the wrapper script with the following CLI arguments:
#
#     ifcc-wrapper.sh DESTNAME SOURCENAME
#
# where:
# - SOURCENAME indicates the .c file to be compiled (in the current directory).
# - DESTNAME indicates the assembly file to be produced (also in the current dir)

# Warning: you have to forward the exit status of your compiler back to the harness

DESTNAME=$1
SOURCENAME=$2

# Shift the first two arguments to get any additional options
shift 2

# Pass all remaining arguments to ifcc, followed by -o and the destination file
$(dirname $0)/compiler/ifcc "$@" -o "$DESTNAME" "$SOURCENAME"

retcode=$?
exit $retcode