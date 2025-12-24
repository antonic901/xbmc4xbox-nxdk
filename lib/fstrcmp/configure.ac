dnl!
dnl! fstrcmp - fuzzy string compare library
dnl! Copyright (C) 2009, 2012, 2014 Peter Miller
dnl! Written by Peter Miller <pmiller@opensource.org.au>
dnl!
dnl! This program is free software; you can redistribute it and/or
dnl! modify it under the terms of the GNU General Public License as
dnl! published by the Free Software Foundation; either version 3 of the
dnl! License, or (at your option) any later version.
dnl!
dnl! This program is distributed in the hope that it will be useful,
dnl! but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl! General Public License for more details.
dnl!
dnl! You should have received a copy of the GNU General Public License
dnl! along with this program. If not, see <http://www.gnu.org/licenses/>.
dnl!
AC_INIT(install-sh)
AC_CONFIG_HEADER(lib/config.h)
AC_PROG_CC
AC_CANONICAL_HOST
AC_GNU_SOURCE
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_CHECK_PROGS(AR, ar)

AC_USE_SYSTEM_EXTENSIONS
AC_C_CONST
AC_MINIX
AC_ISC_POSIX
AC_OBJEXT
AC_EXEEXT

dnl @synopsis AC_ADD_CFLAGS
dnl
dnl Add the given option to CFLAGS, if it doesn't break the compiler
dnl
AC_DEFUN([AC_ADD_CFLAGS],
[AC_MSG_CHECKING([if $CC accepts $1])
  ac_add_cflags__old_cflags="$CFLAGS"
  CFLAGS="$CFLAGS $1"
  AC_TRY_LINK([#include <stdio.h>],
    [printf("Hello, World!\n"); return 0;],
    AC_MSG_RESULT([yes]),
    AC_MSG_RESULT([no])
    CFLAGS="$ac_add_cflags__old_cflags")
])

AC_ADD_CFLAGS(-Wall)
AC_ADD_CFLAGS(-Wextra)
AC_ADD_CFLAGS(-Wshadow)
dnl! AC_ADD_CFLAGS(-Werror)
dnl! AC_ADD_CFLAGS([-Wl,--as-needed])

AC_CHECK_PROGS(LIBTOOL, libtool)

if test -z "$LIBTOOL"
then
  AC_MSG_RESULT([
        You must have GNU Libtool installed to build fstrcmp.
        Homepage: http://www.gnu.org/software/libtool/])
  OK=no
  if apt-get --version > /dev/null 2> /dev/null; then
    AC_MSG_RESULT([
        The following command may be used to install it:
        sudo apt-get install libtool
    ])
    OK=yes
  fi
  if yum --version > /dev/null 2> /dev/null; then
    AC_MSG_RESULT([
        The following command may be used to install it:
        sudo yum install libtool
    ])
    OK=yes
  fi
  if test "$OK" != "yes"; then
    AC_MSG_RESULT([
        If you are using a package based install, you will need the
        libtool package.
    ])
  fi
  exit 1
fi

AC_CHECK_PROGS(GROFF, groff roff)
AC_CHECK_PROGS(SOELIM, gsoelim soelim)
AC_CHECK_PROGS(REFER, refer grefer)

dnl
dnl We need some way to determine the value of the default MANPATH
dnl setting, so that we can append to it.  There are (at least) two
dnl incompatible man implementations out there.  The one used by Debian
dnl Linux has a "manpath -q" command, the one used by RedHat Linux has
dnl a "man -w" command.  If neither works, assume a constant string.
dnl
AC_CHECK_PROGS(MANPATH_PROG, manpath)

if test -n "$MANPATH_PROG"
then
    dnl
    dnl Sometimes manpath prints irritating warnings,
    dnl but not all versions have -q to silence them.
    dnl
    MANPATH_PROG="$MANPATH_PROG 2>/dev/null"
else
    dnl
    dnl Some versions of man have a -w option, but it's short for --whatis
    dnl and so they print nothing on stdout, and an error message about
    dnl the missing argument on stderr.  Some versions of man -w do the
    dnl same thing as manpath.
    dnl
    temp_str=`man -w 2>/dev/null`
    if test -n "$temp_str"
    then
        MANPATH_PROG="man -w 2>/dev/null"
    else
        MANPATH_PROG="echo /usr/man:/usr/share/man"
    fi
fi

dnl
dnl     Test if groff takes -ms or -mgs for the macro package.
dnl
AC_MSG_CHECKING([for $GROFF -ms macros])
if test `echo ' ' | groff -mgs 2> /dev/null | wc -l` -gt 0
then
    GROFF_MS_MACROS=gs
else
    GROFF_MS_MACROS=s
fi
AC_SUBST(GROFF_MS_MACROS)
AC_MSG_RESULT([-m$GROFF_MS_MACROS])

dnl
dnl     Test if groff takes -mm or -mgm for the macro package.
dnl
AC_MSG_CHECKING([for $GROFF -mm macros])
if test `echo ' ' | groff -mgm 2> /dev/null | wc -l` -gt 0
then
    GROFF_MM_MACROS=gm
else
    GROFF_MM_MACROS=m
fi
AC_SUBST(GROFF_MM_MACROS)
AC_MSG_RESULT([-m$GROFF_MM_MACROS])

AC_CHECK_HEADERS(          \
    assert.h               \
    ctype.h                \
    fcntl.h                \
    getopt.h               \
    memory.h               \
    pwd.h                  \
    stddef.h               \
    stdlib.h               \
    string.h               \
    sys/stat.h             \
    sys/time.h             \
    sys/types.h            \
    unistd.h               \
    wchar.h                \
    )

AC_TYPE_SIZE_T

AC_CHECK_FUNCS(  \
    snprintf     \
    strerror     \
    vsnprintf    \
    )

dnl
dnl   Test to find a Bourne shell which understands functions
dnl
AC_MSG_CHECKING([for a Bourne shell which understands functions])
if test "z$SH" = "z"; then
    if test -f /bin/sh5; then
        SH=/bin/sh5
    else
        SH=/bin/sh
    fi
fi
AC_SUBST(SH)
AC_DEFINE_UNQUOTED(CONF_SHELL, ["$SH"],
[Set this to be the absolute path of a Bourne shell
which understands functions.])
AC_MSG_RESULT($SH)

dnl
dnl Evaluate some of the variables, to remove ${prefix} references.
dnl This way, they can be used in C programs and Roff input.
dnl Make sure that fstrcmp is mentioned in the libdir and datadir paths;
dnl add it if it is not already there.
dnl
test "x$prefix" = xNONE && prefix="${ac_default_prefix-/usr/local}"
test "x$exec_prefix" = xNONE && exec_prefix="$prefix"
eval "bindir=$bindir"
eval "datarootdir=$datarootdir"
eval "includedir=$includedir"
eval "datadir=$datadir"
eval "libdir=$libdir"
eval "mandir=$mandir"
eval "sharedstatedir=$sharedstatedir"
eval "sysconfdir=$sysconfdir"

dnl
dnl     If the soelim program exists, and understands the -I option,
dnl     arrange for the Makefile to install the .ps files from the
dnl     documentation source files.
dnl
if test -n "$SOELIM"
then
  if $SOELIM -I. /dev/null > /dev/null 2>&1
  then
    : nothing
  else
    AC_MSG_RESULT([
        The $SOELIM program does not understand the -I option.
        GNU Groff 1.15 or later works correctly.
        See the BUILDING file for details.])
    GROFF=
  fi
else
  GROFF=
fi
if test -n "$REFER"
then
    : nothing
else
    # It turns out that somehow Debian systems get a half-assed groff
    # install from build-essential.  Looking for "refer" is a better
    # test of the completeness of the groff install than either
    # "groff" or "soelim" on their own, plus it's needed for the build.
    GROFF=
fi
if test -n "$GROFF"
then
  HAVE_GROFF=yes
else
  HAVE_GROFF=no
  AC_MSG_RESULT([
        The fstrcmp documentation set and manual pages will
        be formatted and installed if you first install
        GNU Groff 1.15 or later and then re-run this ./configure script.
        Remember to use \`\`rm config.cache'' before you do.
        Homepage: http://www.gnu.org/software/groff/])
  OK=no
  if apt-get --version > /dev/null 2> /dev/null; then
    AC_MSG_RESULT([
        The following command may be used to install it:
        sudo apt-get install groff
    ])
    OK=yes
  fi
  if yum --version > /dev/null 2> /dev/null; then
    AC_MSG_RESULT([
        The following command may be used to install it:
        sudo yum install groff
    ])
    OK=yes
  fi
  if test "$OK" != "yes"; then
    AC_MSG_RESULT([
        If you are using a package based install, you will need the
        groff package.
    ])
  fi
fi
AC_SUBST(HAVE_GROFF)

# program prefix is the bit to add to the start of the name
if test ${PROGRAM_PREFIX-NONE} != NONE -a ${program_prefix-NONE} != NONE; then
PROGRAM_PREFIX=${program_prefix}
fi
AC_SUBST(PROGRAM_PREFIX)

# program suffix is the bit to add to the end of the name (before .exe)
if test ${PROGRAM_SUFFIX-NONE} != NONE -a ${program_suffix-NONE} != NONE; then
PROGRAM_SUFFIX=${program_suffix}
fi
AC_SUBST(PROGRAM_SUFFIX)

AH_TOP(
[/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009, 2012, 2014 Peter Miller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIB_CONFIG_H
#define LIB_CONFIG_H
])

AH_BOTTOM([
/*
 * There is more to do, but we need to insulate it from config.status,
 * because it screws up the #undef lines.  They are all implications of
 * the above information, so there is no need for you to edit the file,
 * if you are configuring fstrcmp manually.
 */
#include <lib/config.messy.h>

#endif /* LIB_CONFIG_H */])

AC_OUTPUT(Makefile)
