#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([libdoozer], [0.1], [caoimhechaos@protonmail.com])
AC_CONFIG_SRCDIR([conn.cc])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_WITH([googletest], [AC_HELP_STRING([--with-googletest=DIR],
	[Path to the GoogleTest library])],
	[INCLUDES="$INCLUDES -I${withval}/include";
	 LDFLAGS="${LDFLAGS} -L${withval}/lib -L${withval}/lib"])
AC_ARG_WITH([protobuf], [AC_HELP_STRING([--with-protobuf=DIR],
	[Path to the protobuf library])],
	[INCLUDES="$INCLUDES -I${withval}/include/google/protobuf";
	 LDFLAGS="${LDFLAGS} -L${withval}/lib -L${withval}/lib"])
AC_ARG_WITH([qt-includes], [AC_HELP_STRING([--with-qt-includes=DIR],
	[Path to the QT headers])],
	[INCLUDES="$INCLUDES -I${withval}"])
AC_ARG_WITH([qt-libs], [AC_HELP_STRING([--with-qt-libs=DIR],
	[Path to the QT libraries])],
	 LDFLAGS="${LDFLAGS} -L${withval} -L${withval}"])
CPPFLAGS="$CPPFLAGS $INCLUDES"

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_LANG(C++)

LIBRARY_VERSION=`echo $VERSION | $SED -e's/\./:/g'`
AC_SUBST(LIBRARY_VERSION)


OLDCXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=c++0x"
AC_MSG_CHECKING([whether $CXX accepts -std=c++0x])
AC_LINK_IFELSE([AC_LANG_CALL([], [main])], [AC_MSG_RESULT(yes)],
	[AC_MSG_RESULT(no); CXXFLAGS="$OLDCXXFLAGS"])
unset OLDCXXFLAGS
CXXFLAGS="${CXXFLAGS}"
AC_SUBST(CXXFLAGS)

# Checks for libraries.
PROTO_LIBS=""
GLOG_LIBS=""
GTEST_LIBS=""
QT_LIBS=""
QT_SUBDIR=""
AC_CHECK_LIB([protobuf], [main], [PROTO_LIBS="-lprotobuf"],
	AC_ERROR([libprotobuf is required]))
AC_CHECK_LIB([gtest_main], [main], [GTEST_LIBS="$GTEST_LIBS -lgtest_main"])
AC_CHECK_LIB([gtest], [main], [GTEST_LIBS="$GTEST_LIBS -lgtest"])
AC_CHECK_LIB([glog], [main],
	[GLOG_LIBS="$GLOG_LIBS -lglog"],
	AC_ERROR([libglog is required]))
AC_CHECK_LIB([QtNetwork], [main],
	[QT_LIBS="$QT_LIBS -lQtNetwork"])
AC_CHECK_LIB([QtCore], [main],
	[QT_LIBS="$QT_LIBS -lQtCore"])
AC_CHECK_LIB([pthread], [pthread_once],
	     [AC_LIBS="$AC_LIBS -lpthread"])
LIBS="$LIBS $AC_LIBS $PROTO_LIBS $GLOG_LIBS"
AC_SUBST(GTEST_LIBS)
AC_SUBST(QT_LIBS)
AC_SUBST(AC_LIBS)
AC_SUBST(LIBS)

# Checks for header files.
AC_CHECK_HEADERS([QtNetwork/QTcpSocket])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile cli/Makefile])
AC_OUTPUT
