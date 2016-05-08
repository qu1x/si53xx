# This file is part of si53xx, see <https://qu1x.org/si53xx>.
# 
# Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
# 
# si53xx is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License version 3
# as published by the Free Software Foundation on 19 November 2007.
# 
# si53xx is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with si53xx. If not, see <https://www.gnu.org/licenses>.

AC_INIT([si53xx], [1.0.0],
	[n3vu0r@qu1x.org], [si53xx], [https://qu1x.org/si53xx])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/bin/si53xx.cxx])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

LT_INIT([disable-static])

AC_PROG_CXX

AC_LANG([C++])

CXX="$CXX -std=c++14"

AC_CHECK_HEADERS([\
getopt.h], [], [AC_MSG_ERROR([header not found])])

AC_CHECK_FUNCS([\
getsubopt \
getopt_long], [], [AC_MSG_ERROR([function not found])])

AC_CHECK_HEADER([linux/i2c-dev.h], [HAVE_I2C2L=yes], [HAVE_I2C2L=no])
AM_CONDITIONAL([HAVE_I2C2L], [test x$HAVE_I2C2L = xyes])
AM_COND_IF([HAVE_I2C2L], [AC_DEFINE([HAVE_I2C2L], [1], [Have i2c2l driver.])])
AM_COND_IF([HAVE_I2C2L], [AC_CHECK_HEADERS([\
sys/types.h \
sys/stat.h \
fcntl.h \
sys/ioctl.h \
unistd.h \
dirent.h \
fnmatch.h], [], [AC_MSG_ERROR([header not found])])])
AM_COND_IF([HAVE_I2C2L], [AC_CHECK_FUNCS([\
fnmatch \
scandir \
alphasort \
open \
ioctl \
write \
read \
close], [], [AC_MSG_ERROR([function not found])])])

AC_CHECK_HEADER([mpsse.h], [HAVE_MPSSE=yes], [HAVE_MPSSE=no])
AM_CONDITIONAL([HAVE_MPSSE], [test x$HAVE_MPSSE = xyes])
AM_COND_IF([HAVE_MPSSE], [AC_DEFINE([HAVE_MPSSE], [1], [Have mpsse driver.])])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

printf "
$PACKAGE_STRING

i2c2l: $HAVE_I2C2L
mpsse: $HAVE_MPSSE

"
