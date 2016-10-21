AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS = nsieve
bin_SCRIPTS = nsieve-run.sh
AM_CFLAGS = -DUNIX
#AM_LDFLAGS = -static

nsieve_SOURCES = \
	nsieve.c

EXTRA_DIST = $(bin_SCRIPTS)
