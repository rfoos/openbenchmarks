## Process this file with autogen.sh to produce configure files.
# @author Rick Foos
# @file

AC_INIT([Dhrystone],[2.1])
AC_CONFIG_SRCDIR([dhry_2.c])
dnl Automake 1.6 requires AM_CONFIG_HEADER
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])



# Checks for programs

dnl AC_PROG_CC
CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want debug symbols implicitly
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.

AC_HEADER_STDC
AC_HEADER_STDBOOL

# Checks for typedefs, structures


AC_C_BIGENDIAN
AC_CHECK_DECL([malloc])

# Checks for library functions.

dnl do not use ac_func_malloc.
AC_CHECK_FUNCS([malloc scanf printf time times clock])

# Pass optional Simulator to run script.
AC_SUBST([SIMULATOR])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([dhrystone-run.sh],
                [chmod a+x dhrystone-run.sh])
AC_OUTPUT

