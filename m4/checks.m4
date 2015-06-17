AC_DEFUN([get_defines],
[
dnl See if posix_memalign is available
AC_CHECK_FUNC([posix_memalign], [AC_DEFINE([HAVE_POSIX_MEMALIGN],[1],[Define if posix_memalign exists.])])
AC_CHECK_FUNC([ioperm], [AC_DEFINE([HAVE_IOPERM],[1],[Define if ioperm exists.])])
])
