AC_INIT([nssandbox], [0.1], [https://github.com/etbuira/nssandbox/issues])

AC_PREREQ([2.69])
AC_CONFIG_SRCDIR([src/nssandbox.c])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CC
AC_PROG_CC_C99
if test x$ac_cv_prog_cc_c99 = xno; then
	AC_MSG_ERROR([No C99 compiler found])
fi
AM_PROG_CC_C_O

AC_CHECK_PROGS([CHMOD], [chmod], [AC_MSG_ERROR([chmod not found])])

AC_CHECK_FUNCS([getpwuid initgroups setgroups unshare], [], [AC_MSG_ERROR([Not all needed functions have been found])])

AC_USE_SYSTEM_EXTENSIONS

AC_HEADER_STDC
AC_CHECK_HEADERS([sched.h sys/types.h sys/socket.h sys/ioctl.h linux/if.h], [], [AC_MSG_ERROR([A required header was not found])], [#include <sys/socket.h>])

AC_TYPE_INTMAX_T
AC_TYPE_UID_T

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

