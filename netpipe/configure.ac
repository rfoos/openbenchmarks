## Process this file with autoconf to produce a configure script.
# @author Rick Foos
# @file

AC_INIT([NetPIPE], [3.6.2])
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/netpipe.c])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([src/config.h])


# Checks for programs.
AC_PROG_CC
AM_PROG_AS
AC_PROG_MAKE_SET

# Required for memcpy.c
# Test if the C compiler accepts the `-c' and `-o' options simultaneously,
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/module.h sys/errno.h netinet/ip_fw.h errno.h arpa/inet.h fcntl.h inttypes.h limits.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h sys/resource.h])

# Checks for Architecture
AC_C_BIGENDIAN

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

# Checks for library functions.
# rpl_malloc undefined problem AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([bcopy bzero gethostbyname gethostname gettimeofday inet_ntoa memmove memset setenv socket strcasecmp strchr strerror strstr strtol])

# NOTE: Do NOT determine 32 | 64 bit from target cpu. Compiler may be set to
# generate 32 bit code on 64 bit target.
case $target_cpu in
	ppc* | powerpc*) CPPFLAGS="$CPPFLAGS -DARCH_IS_PPC"
esac

AM_CONDITIONAL([OBJEXT_NOT_O], [test "$OBJEXT" != "o" -a "$OBJEXT" != "obj" ])

# Pass Simulator
AC_SUBST([SIMULATOR])

AC_CONFIG_SUBDIRS([openmpi])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([netpipe-run.sh],
                [chmod +x netpipe-run.sh])

AC_OUTPUT

