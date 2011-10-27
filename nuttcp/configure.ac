## Process this file with autoconf to produce a configure script.
# @author Rick Foos
# @file

AC_INIT([nuttcp], [6.1.2])
AC_CONFIG_SRCDIR([nuttcp-6.1.2.c])
dnl Automake required
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
CFLAGS=`echo " $CFLAGS " | sed -e "s/ -g / /"` # do not want debug symbols implicitly
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lsocket':
AC_CHECK_LIB([socket], [main])

# Checks for header files.
AC_HEADER_RESOLV
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([alarm bzero gethostbyaddr gethostbyname gettimeofday memset select socket strchr strdup strstr strtol strtoull])

# Pass Simulator
AC_SUBST([SIMULATOR])
AC_CONFIG_FILES([Makefile nuttcp-run.sh],[chmod a+rx nuttcp-run.sh])
AC_OUTPUT
