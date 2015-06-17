# Check for Qt compiler flags, linker flags, and binary packages
AC_DEFUN([gw_CHECK_QT],
[
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([AC_PATH_X])

AC_MSG_CHECKING([QTDIR])
AC_ARG_WITH([qtdir], [  --with-qtdir=DIR        Qt installation directory [default=$QTDIR]], QTDIR=$withval)
# Check that QTDIR is defined or that --with-qtdir given
if test x"$QTDIR" = x ; then
    QT_SEARCH="/usr/lib/qt31 /usr/local/qt31 /usr/lib/qt3 /usr/local/qt3 /usr/lib/qt2 /usr/local/qt2 /usr/lib/qt /usr/local/qt /usr/local/qt4 /usr/local/TrollTech/Qt-4.4.1"
    for i in $QT_SEARCH; do
        if test -f $i/include/qglobal.h -a x$QTDIR = x; then QTDIR=$i; fi
    done
fi
if test x"$QTDIR" = x ; then
    AC_MSG_ERROR([*** QTDIR must be defined, or --with-qtdir option given])
fi
AC_MSG_RESULT([$QTDIR])

# Change backslashes in QTDIR to forward slashes to prevent escaping
# problems later on in the build process, mainly for Cygwin build
# environment using MSVC as the compiler
# TODO: Use sed instead of perl
QTDIR=`echo $QTDIR | perl -p -e 's/\\\\/\\//g'`

# Figure out which version of Qt we are using
AC_MSG_CHECKING([Qt version])
AC_ARG_WITH([qtver], [  --with-qtver=VER        Qt version [default=$QT_VER]], QT_VER=$withval)
if test x"$QT_VER" = x ; then
  if test -f $QTDIR/include/qglobal.h; 
  then
    QT_VER=`grep 'define.*QT_VERSION_STR\W' $QTDIR/include/qglobal.h | perl -p -e 's/\D//g'`
  #else
    #QT_VER=`grep 'define.*QT_VERSION_STR\W' $QTDIR/src/corelib/global/qglobal.h | perl -p -e 's/\D//g'`
  fi
fi
if test x"$QT_VER" = x ; then
  if test -f $QTDIR/include/QtCore/qglobal.h; 
  then
    QT_VER=`grep 'define.*QT_VERSION_STR\W' $QTDIR/include/QtCore/qglobal.h | perl -p -e 's/\D//g'`
  else
    QT_VER=`grep 'define.*QT_VERSION_STR\W' $QTDIR/src/corelib/global/qglobal.h | perl -p -e 's/\D//g'`
  fi
fi
case "${QT_VER}" in
    2*)
        QT_MAJOR="2"
    ;;
    3*)
        QT_MAJOR="3"
    ;;
    4*)
        QT_MAJOR="4"
    ;;
    *)
        AC_MSG_ERROR([*** Don't know how to handle this Qt major version])
    ;;
esac
AC_MSG_RESULT([$QT_VER ($QT_MAJOR)])

# find path for moc
AC_MSG_CHECKING([moc path])
AC_ARG_WITH([mocpath], [  --with-mocpath=MOC        moc path [default=$MOC]], MOC=$withval)
if test x"$MOC" = x ; then
  AC_CHECK_PROG(MOC, moc, $QTDIR/bin/moc, , $QTDIR/bin)
  if test x$MOC = x ; then
          AC_MSG_ERROR([*** moc not found])
  fi
fi
AC_MSG_RESULT([$MOC])


# uic is the Qt user interface compiler
AC_MSG_CHECKING([uic path])
AC_ARG_WITH([uicpath], [  --with-uicpath=UIC        uic path [default=$UIC]], UIC=$withval)
if test x"$UIC" = x ; then
  AC_CHECK_PROG(UIC, uic, $QTDIR/bin/uic, , $QTDIR/bin)
  if test x$UIC = x ; then
          AC_MSG_ERROR([*** uic not found])
  fi
fi
AC_MSG_RESULT([$UIC])


# qembed is the Qt data embedding utility.
# It is located in $QTDIR/tools/qembed, and must be compiled and installed manually
#AC_CHECK_PROG(QEMBED, qembed, $QTDIR/tools/qembed/qembed, , $QTDIR/tools/qembed)
#if test x$QEMBED = x ; then
#        AC_MSG_ERROR([*** qembed not found])
#fi


# Calculate Qt include path
if test $QT_MAJOR = "4" ; then
	QT_CXXFLAGS="-I$QTDIR/include -I$QTDIR/include/QtCore -I$QTDIR/include/QtGui -I$QTDIR/include/Qt3Support -DQT3_SUPPORT"
        QT_STATIC_LIB="$QTDIR/lib/QtCore4.a $QTDIR/lib/QtGui4.a $QTDIR/lib/Qt3Support.a"
else
	QT_CXXFLAGS="-I$QTDIR/include"
	QT_STATIC_LIB=""
fi

QT_IS_EMBEDDED="no"
# On unix, figure out if we're doing a static or dynamic link
case "${host}" in
    *-cygwin)
	AC_DEFINE_UNQUOTED(WIN32, "", Defined if on Win32 platform)
        if test -f "$QTDIR/lib/qt.lib" ; then
            QT_LIB="qt.lib"
            QT_IS_STATIC="yes"
            QT_IS_MT="no"
        elif test -f "$QTDIR/lib/qt-mt.lib" ; then
            QT_LIB="qt-mt.lib" 
            QT_IS_STATIC="yes"
            QT_IS_MT="yes"
        elif test -f "$QTDIR/lib/qt$QT_VER.lib" ; then
            QT_LIB="qt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="no"
        elif test -f "$QTDIR/lib/qt-mt$QT_VER.lib" ; then
            QT_LIB="qt-mt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="yes"
        fi
        ;;

    *)
        QT_IS_STATIC=`ls $QTDIR/lib/*.a 2> /dev/null`
        if test "x$QT_IS_STATIC" = x; then
            QT_IS_STATIC="no"
        else
            QT_IS_STATIC="yes"
        fi
        if test x$QT_IS_STATIC = xno ; then
            QT_IS_DYNAMIC=`ls $QTDIR/lib/*.so 2> /dev/null` 
            if test "x$QT_IS_DYNAMIC" = x;  then
                AC_MSG_ERROR([*** Couldn't find any Qt libraries])
            fi
        fi

        if test $QT_MAJOR = "4" ; then
	  QT_LIB="-lQtCore -lQtGui -lQt3Support"
          QT_IS_MT="yes"
        else

          if test "x`ls $QTDIR/lib/libqt.* 2> /dev/null`" != x ; then
              QT_LIB="-lqt"
              QT_IS_MT="no"
          elif test "x`ls $QTDIR/lib/libqt-mt.* 2> /dev/null`" != x ; then
              QT_LIB="-lqt-mt"
              QT_IS_MT="yes"
          elif test "x`ls $QTDIR/lib/libqte.* 2> /dev/null`" != x ; then
              QT_LIB="-lqte"
              QT_IS_MT="no"
              QT_IS_EMBEDDED="yes"
          elif test "x`ls $QTDIR/lib/libqte-mt.* 2> /dev/null`" != x ; then
              QT_LIB="-lqte-mt"
              QT_IS_MT="yes"
              QT_IS_EMBEDDED="yes"
          fi
        fi
        ;;
esac
AC_MSG_CHECKING([if Qt is static])
AC_MSG_RESULT([$QT_IS_STATIC])
AC_MSG_CHECKING([if Qt is multithreaded])
AC_MSG_RESULT([$QT_IS_MT])
AC_MSG_CHECKING([if Qt is embedded])
AC_MSG_RESULT([$QT_IS_EMBEDDED])

QT_GUILINK=""
QASSISTANTCLIENT_LDADD="-lqassistantclient"
case "${host}" in
    *irix*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *linux*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes && test $QT_IS_EMBEDDED = no; then
            if test "x$x_libraries" != x ; then
                QT_LIBS="$QT_LIBS -L$x_libraries"
            fi
            QT_LIBS="$QT_LIBS -lXext -lX11 -lm -lSM -lICE -ldl -ljpeg"
        fi
        ;;


    *osf*) 
        # Digital Unix (aka DGUX aka Tru64)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *solaris*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -lresolv -lsocket -lnsl"
        fi
        ;;


    *win*)
        # linker flag to suppress console when linking a GUI app on Win32
        QT_GUILINK="/subsystem:windows"

	if test $QT_MAJOR = "3" ; then
	    if test $QT_IS_MT = yes ; then
        	QT_LIBS="/nodefaultlib:libcmt"
            else
            	QT_LIBS="/nodefaultlib:libc"
            fi
        fi

        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS $QT_LIB kernel32.lib user32.lib gdi32.lib comdlg32.lib ole32.lib shell32.lib imm32.lib advapi32.lib wsock32.lib winspool.lib winmm.lib netapi32.lib"
            if test $QT_MAJOR = "3" ; then
                QT_LIBS="$QT_LIBS qtmain.lib"
            fi
        else
            QT_LIBS="$QT_LIBS $QT_LIB"        
            if test $QT_MAJOR = "3" ; then
                QT_CXXFLAGS="$QT_CXXFLAGS -DQT_DLL"
                QT_LIBS="$QT_LIBS qtmain.lib qui.lib user32.lib netapi32.lib"
            fi
        fi
        QASSISTANTCLIENT_LDADD="qassistantclient.lib"
        ;;

esac


if test x"$QT_IS_EMBEDDED" = "xyes" ; then
        QT_CXXFLAGS="-DQWS $QT_CXXFLAGS"
fi

if test x"$QT_IS_MT" = "xyes" ; then
        QT_CXXFLAGS="$QT_CXXFLAGS -D_REENTRANT -DQT_THREAD_SUPPORT"
fi

QT_LDADD="-L$QTDIR/lib $QT_LIBS"

if test x$QT_IS_STATIC = xyes ; then
    OLDLIBS="$LIBS"
    LIBS="$QT_LDADD"
    AC_CHECK_LIB(Xft, XftFontOpen, QT_LDADD="$QT_LDADD -lXft")
    LIBS="$LIBS"
fi

AC_MSG_CHECKING([QT_CXXFLAGS])
AC_MSG_RESULT([$QT_CXXFLAGS])
AC_MSG_CHECKING([QT_LDADD])
AC_MSG_RESULT([$QT_LDADD])

AC_SUBST(QTDIR)
AC_SUBST(QT_MAJOR)
AC_SUBST(QT_CXXFLAGS)
AC_SUBST(QT_STATIC_LIB)
AC_SUBST(QT_LDADD)
AC_SUBST(QT_GUILINK)
AC_SUBST(QASSISTANTCLIENT_LDADD)

])
