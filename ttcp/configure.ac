
AC_INIT([ttcp],[1.0])
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([ttcp.c])
# Add Automake
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

dnl Set for automake.
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.

AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([signal.h arpa/inet.h netdb.h netinet/in.h netinet/tcp.h sys/param.h sys/socket.h sys/resource.h])


# Checks for typedefs, structures, and compiler characteristics.



# Checks for library functions.


AC_FUNC_SELECT_ARGTYPES
dnl am_func_malloc generates rtl_malloc that we do not need.
AC_CHECK_FUNCS([malloc bzero gethostbyname gettimeofday inet_ntoa select socket])

# Checks for System Services

AC_TYPE_SIGNAL


# Final configuration file output

case $host in
	*-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
	CFLAGS="$CFLAGS -DCYGWIN"
          ;;
	esac

# Pass Simulator
AC_SUBST([SIMULATOR])

AC_CONFIG_FILES([Makefile ttcp-run.sh],
                [chmod a+x ttcp-run.sh])
AC_OUTPUT

