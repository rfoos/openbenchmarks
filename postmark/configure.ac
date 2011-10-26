#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([Postmark],[1.5],[support@solengtech.com],[postmark])
AC_CONFIG_SRCDIR([postmark-1_5.c])
dnl Automake required
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

dnl Checks for programs

AC_PROG_CC
CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want debug symbols implicitly
#AC_PROG_CXX
#CXXFLAGS=`echo " $CXXFLAGS " | sed -e "s/ -g / /"` # do not want it implicitly
dnl AC_ISC_POSIX
dnl AC_PROG_INSTALL
dnl AC_PROG_RANLIB
AC_PROG_MAKE_SET

dnl Checks for libraries.

dnl Checks for header files.

AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

dnl Checks for library functions

dnl do not use ac_func_malloc.
AC_CHECK_FUNCS([malloc rmdir strchr strrchr])

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Architecture Checks

dnl AC_C_BIGENDIAN
dnl AC_CHECK_SIZEOF(int)

dnl Check for system services


dnl Final configuration file output

AC_CONFIG_FILES([Makefile postmark-cmds.txt run.sh],
                [chmod +x run.sh])
AC_OUTPUT
