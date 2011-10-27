#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([memtester], [4.2.2])
AC_CONFIG_SRCDIR([memtester.c])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MAKE_SET

# Checks for libraries.
# OSF/1 (Tru64) needs /usr/lib/librt.a for mlock()
AC_CHECK_LIB([rt], [mlock])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
# rpl_malloc undefined AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([mlock strtoul strtoull])

# Optional Simulator
AC_SUBST([SIMULATOR])
AC_CONFIG_FILES([Makefile memtester-run.sh],[chmod a+x memtester-run.sh])
AC_OUTPUT
