AUTOMAKE_OPTIONS = foreign
SUBDIRS = compat doc include src man

AM_CPPFLAGS = @STRIP_BEGIN@ \
	      -I$(top_srcdir)/include \
	      -I$(top_builddir)/include \
	      @STRIP_END@

noinst_SCRIPTS = m4/acx_pthread.m4 m4/dast.m4 iperf-run.sh
ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -Wall
AM_FLAGS = -Wall
