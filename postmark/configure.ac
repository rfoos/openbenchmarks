## Process this file with autoconf to produce a configure script.
# @author Rick Foos
# @file

AC_INIT([Postmark],[1.5])
AC_CONFIG_SRCDIR([postmark-1_5.c])
dnl Automake required
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])

# Checks for programs

AC_PROG_CC
CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want debug symbols implicitly
#AC_PROG_CXX
#CXXFLAGS=`echo " $CXXFLAGS " | sed -e "s/ -g / /"` # do not want it implicitly
dnl AC_ISC_POSIX
dnl AC_PROG_INSTALL
dnl AC_PROG_RANLIB
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.

AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for library functions

dnl do not use ac_func_malloc.
AC_CHECK_FUNCS([malloc rmdir strchr strrchr])

# Checks for typedefs, structures, and compiler characteristics.

# Pass Simulator
AC_SUBST([SIMULATOR])

AC_CONFIG_FILES([Makefile postmark-cmds.txt postmark-run.sh],
                [chmod +x postmark-run.sh])
AC_OUTPUT
