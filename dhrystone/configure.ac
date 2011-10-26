## Process this file with autogen.sh to produce configure files.
# @author Rick Foos 
# @file

AC_INIT([Dhrystone],[2.1],[support@solengtech.com],[dhrystone])
AC_CONFIG_SRCDIR([dhry_2.c])
dnl Automake 1.6 requires AM_CONFIG_HEADER
AM_INIT_AUTOMAKE([dhrystone], 2.1)
AM_CONFIG_HEADER([config.h])


dnl --------------------------------------------------
dnl Checks for programs
dnl --------------------------------------------------

dnl AC_PROG_CC
CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want debug symbols implicitly
dnl AC_PROG_CXX
dnl CXXFLAGS=`echo " $CXXFLAGS " | sed -e "s/ -g / /"` # do not want it implicitly
dnl AC_ISC_POSIX
dnl AC_PROG_INSTALL
AC_PROG_MAKE_SET
dnl AC_PROG_RANLIB
dnl AC_LANG(C)

dnl --------------------------------------------------
dnl Checks for libraries.
dnl --------------------------------------------------

dnl --------------------------------------------------
dnl Checks for header files.
dnl --------------------------------------------------

AC_HEADER_STDC
dnl Cygwin /usr/bin/m4 fails
dnl AC_PREREQ
AC_HEADER_STDBOOL
dnl AC_CHECK_HEADERS([stdlib.h string.h time.h])

dnl --------------------------------------------------
dnl Checks for typedefs, structures
dnl --------------------------------------------------

dnl AC_C_CONST
dnl AC_TYPE_SIZE_T
AC_C_BIGENDIAN
AC_CHECK_DECL([malloc])


dnl --------------------------------------------------
dnl Checks for library functions.
dnl --------------------------------------------------

dnl do not use ac_func_malloc.
AC_CHECK_FUNCS([malloc scanf printf time times clock])

dnl --------------------------------------------------
dnl Architecture Checks
dnl --------------------------------------------------

dnl AC_C_BIGENDIAN
dnl AC_CHECK_SIZEOF(int)

dnl --------------------------------------------------
dnl Check for system services
dnl --------------------------------------------------


dnl --------------------------------------------------
dnl Final configuration file output
dnl --------------------------------------------------

AM_CONDITIONAL([OBJEXT_NOT_O], [test "$OBJEXT" != "o" -a "$OBJEXT" != "obj" ])
# Pick up extra CFLAGS and LDFLAGS, Build with different flags than configure.
AC_SUBST([AM_CFLAGS])dnl
AC_SUBST([AM_LDFLAGS])dnl
# Pass Simulator and executable extension to run script.
AC_SUBST([SIM])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([dhrystone-run.sh],
                [chmod +x dhrystone-run.sh])
AC_OUTPUT
