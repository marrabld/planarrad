# for win32 build, need to know about the qt libs themselves
AC_DEFUN([CHECK_QTLIBS],
[

AC_MSG_CHECKING([QTLIBDIR])
AC_ARG_WITH([qtlibdir], [  --with-qtlibdir=DIR        Qt libraries installation directory [default=$QTLIBDIR]], QTLIBDIR=$withval)
# Check that JUDEDIR is defined or that --with-judedir given
if test x"$QTLIBDIR" = x; 
then
  #AC_MSG(["QT library location not specified, this is probably required for win32 builds so use --with-qtlibdir if so"])
  QTLIBS=""
else 
  QTLIBS=$QTLIBDIR"/libQtCore4.a "$QTLIBDIR"/libQtGui4.a "$QTLIBDIR"/libQt3Support4.a"
fi
AC_MSG_RESULT([$QTLIBDIR])
AC_SUBST(QTLIBS)
])

