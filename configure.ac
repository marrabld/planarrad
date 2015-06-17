
AC_CONFIG_MACRO_DIR([m4])

# disable configure caching
define([AC_CACHE_LOAD], )
define([AC_CACHE_SAVE], )

AC_INIT([jude], [2.0], [j.d.hedley@planarrad.com])

# autoconf 2.52+ required for libtool support on cygwin/mingw hosts
AC_PREREQ(2.52)

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([base/jtypes.h])

AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

FREE_VERSION="1"
if test "x${FREE_VERSION}" = x1; then 
AC_DEFINE([FREE_VERSION], [1], [Define to 1 to compile free version.])  
fi



OPTIMISE

AC_ARG_ENABLE([qt],
[  --disable-qt    Disable use of Qt and hence all GUI compilation],
[case "${enableval}" in
  yes) qt=true ;;
  no)  qt=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-qt]) ;;
esac],[qt=true])
AM_CONDITIONAL([WITH_QT], [test x$qt = xtrue])

if test "x${qt}" = xtrue; then
  gw_CHECK_QT
  CHECK_QTLIBS
fi


AC_ARG_ENABLE([nonwin32],
[  --disable-nonwin32    Disable things that don't currently compile under windows],
[case "${enableval}" in
  yes) nonwin32=true ;;
  no)  nonwin32=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-nonwin32]) ;;
esac],[nonwin32=true])
AM_CONDITIONAL([WIN32], [test x$nonwin32 = xfalse])

get_defines

AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([pow]) 
AC_CHECK_FUNCS([sqrt])
AC_CHECK_HEADERS([sys/time.h])
AC_C_CONST
AC_C_INLINE
AC_FUNC_MEMCMP
AC_FUNC_VPRINTF
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T


AM_CONFIG_HEADER(config.h:config.in)

AM_CONDITIONAL([QT_MAJOR_IS_4], [test x$QT_MAJOR = x4])







AC_OUTPUT(Makefile 
	base/Makefile
	base/testing/Makefile
	math/Makefile
	math/testing/Makefile
	file/Makefile
	file/testing/Makefile
	ds/Makefile
	ds/testing/Makefile
	S2/Makefile
	drawS2/Makefile
	drawS2/testing/Makefile
	S3/Makefile
	S3/testing/Makefile
	table/Makefile
	table/testing/Makefile
	rad/Makefile
	tool/Makefile
	chartS2/Makefile
	chartS2/testing/Makefile
	fe/Makefile
	wl/Makefile
	wl/testing/Makefile
	filetool/Makefile
	skytool/Makefile
	phasetool/Makefile
	slabtool/Makefile
	surftool/Makefile
	calctool/Makefile
	wltool/Makefile
	wltool/gui/Makefile
	planarrad/gui/Makefile
	testing/Makefile)





