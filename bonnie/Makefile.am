## Automake for Bonnie
# @author Rick Foos
# @file

AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -O2  -DNDEBUG -Wall -W -Wshadow -Wpointer-arith -Wwrite-strings -pedantic -ffor-scope

bin_PROGRAMS = bonnie zcav
bin_SCRIPTS = bon_csv2html bon_csv2txt
noinst_SCRIPTS = bonnie-run.sh

bonnie_SOURCES= \
	bon_file.cpp  bon_time.cpp \
	bon_io.cpp    forkit.cpp \
	bonnie++.cpp  semaphore.cpp \
	bon_suid.cpp  \
	bon_file.h  bon_time.h  forkit.h \
	bon_io.h    conf.h      port.h \
	bonnie.h    config.h    semaphore.h

zcav_SOURCES = \
	zcav.cpp bon_suid.cpp bonnie.h conf.h config.h port.h

man_MANS = bon_csv2html.1  bon_csv2txt.1  bonnie.8  bonnie++.8  zcav.8

EXTRA_DIST = $(man_MANS)
