AC_PREREQ([2.61])
AC_INIT([kvAgregated], [2.2.4], [kvoss@met.no])

AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADER([config.h])

AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S


PKG_CHECK_MODULES(kvcpp, libkvcpp)
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

DOCBOOK_TO_MAN 

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T


AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects foreign])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
