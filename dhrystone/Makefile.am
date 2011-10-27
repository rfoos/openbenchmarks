## Automake definitions
# @author Rick Foos
# @file

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = dhrystone
bin_SCRIPTS = dhrystone-run.sh

# Standard Dhrystone flags
AM_CPPFLAGS = -O2 -DHZ=60 -DREG=register -DTIME -DNDEBUG 


dhrystone_SOURCES = \
		dhry_1.c \
		dhry_2.c \
		dhry.h

EXTRA_DIST = $(bin_SCRIPTS)

