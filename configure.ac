AC_PREREQ([2.59])
AC_INIT([kvAgregated], [2.0.7], [kvoss@met.no])


AC_CONFIG_SRCDIR([src/main.cpp])
#AC_CONFIG_HEADER([config.h])

AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S


AM_INIT_AUTOMAKE

AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_INSTALL


AC_CONFIG_FILES([Makefile])

PKG_CHECK_MODULES(kvcpp, kvcpp)
PKG_CHECK_MODULES(sqlite3, sqlite3)

GTEST_CHECK
GMOCK_CHECK

AX_BOOST_BASE
AX_BOOST_THREAD
if test -z $BOOST_THREAD_LIB; then AC_MSG_ERROR([Unable to find boost thread library]); fi 
AX_BOOST_REGEX
if test -z $BOOST_REGEX_LIB; then AC_MSG_ERROR([Unable to find boost regex library]); fi 
AX_BOOST_FILESYSTEM
if test -z $BOOST_FILESYSTEM_LIB; then AC_MSG_ERROR([Unable to find boost filesystem library]); fi
AX_BOOST_PROGRAM_OPTIONS
if test -z $BOOST_PROGRAM_OPTIONS_LIB; then AC_MSG_ERROR([Unable to find boost program_options library]); fi
 
AC_CONFIG_HEADERS(config.h)

AC_OUTPUT
