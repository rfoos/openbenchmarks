
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = memtester
noinst_SCRIPTS = memtester-run.sh
man_MANS = memtester.8

AM_CPPFLAGS = -O2 -DPOSIX -D_POSIX_C_SOURCE=200809L -D_FILE_OFFSET_BITS=64 -DTEST_NARROW_WRITES
# -s Strip all symbols.
AM_LDFLAGS = --strip-all

memtester_SOURCES = memtester.c memtester.h sizes.h tests.c tests.h types.h

EXTRA_DIST = $(man_MANS)
	
