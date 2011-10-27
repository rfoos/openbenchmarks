
AC_INIT([Whetstone],[1.2])
AC_CONFIG_SRCDIR([whetstone.c])
# Automake
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])


# Checks for programs.
AC_PROG_CC
CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want debug symbols implicitly
AC_PROG_MAKE_SET


# Checks for libraries.

AC_CHECK_LIB([m], [cos])


# Checks for header files.

AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h time.h])


# Checks for typedefs, structures
AC_C_CONST



# Checks for library functions.


# do not use ac_func_malloc causes undefined rpl_malloc
AC_CHECK_FUNCS([time times clock])

# Checks for System Services

# Pass Simulator
AC_SUBST([SIMULATOR])
AC_CONFIG_FILES([Makefile whetstone-run.sh],
                [chmod a+x whetstone-run.sh])
AC_OUTPUT

