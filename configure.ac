AC_INIT([openbenchmarks],[1.0])
## @warning Buildconf may need to be brought in by git submodule update.
AC_CONFIG_SRCDIR([README])
AM_INIT_AUTOMAKE([foreign])
AC_ARG_VAR([SUBDIRS],[benchmarks])

# If running with --config-cache, save a little time here.
AC_PROG_MAKE_SET

# netperf - needs getsocket conditionals fixed.
# httperf - 0.9.0 configure problems...
# iozone3 - new
# lttng - new

AC_CONFIG_SUBDIRS([dhrystone
bonnie
iperf
netpipe
nuttcp
memtester
postmark
ttcp
whetstone ])

# Pass Simulator
AC_SUBST([SIMULATOR])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
