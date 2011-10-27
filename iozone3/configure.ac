#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([iozone3], [3.397])
AC_CONFIG_SRCDIR([src/current/fileop.c])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lail':
#AC_CHECK_LIB([ail], [main])
AC_HAVE_LIBRARY([ail])
# FIXME: Replace `main' with a function in `-laio':
#AC_CHECK_LIB([aio], [main])
AC_HAVE_LIBRARY([aio])
# FIXME: Replace `main' with a function in `-lcnx_syscall':
#AC_CHECK_LIB([cnx_syscall], [main])
AC_HAVE_LIBRARY([cnx_syscall])
# FIXME: Replace `main' with a function in `-lnsl':
#AC_CHECK_LIB([nsl], [main])
AC_HAVE_LIBRARY([nsl])
# FIXME: Replace `main' with a function in `-lposix4':
#AC_CHECK_LIB([posix4], [main])
AC_HAVE_LIBRARY([posix4])
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
AC_HAVE_LIBRARY([pthread])
# FIXME: Replace `main' with a function in `-lpthreads':
#AC_CHECK_LIB([pthreads], [main])
AC_HAVE_LIBRARY([pthreads])
# FIXME: Replace `main' with a function in `-lrt':
#AC_CHECK_LIB([rt], [main])
AC_HAVE_LIBRARY([rt])
# FIXME: Replace `main' with a function in `-lsocket':
#AC_CHECK_LIB([socket], [main])
AC_HAVE_LIBRARY([socket])
# FIXME: Replace `main' with a function in `-lthread':
#AC_CHECK_LIB([thread], [main])
AC_HAVE_LIBRARY([thread])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/file.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
# rpl_malloc problem AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([malloc bzero ftruncate getcwd gethostbyname gethostname getpagesize gettimeofday inet_ntoa memset mkdir munmap rmdir select socket strerror strrchr])

# Pass Simulator
AC_SUBST([SIMULATOR])
AC_CONFIG_FILES([Makefile iozone3-run.sh],[chmod a+x iozone3-run.sh])
AC_OUTPUT

