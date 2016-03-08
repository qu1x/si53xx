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

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libsi53xx.la
libsi53xx_ladir = $(includedir)/si53xx
libsi53xx_la_LDFLAGS = -version-info 0:0:0

libsi53xx_la_HEADERS = src/lib/si53xx.hxx
libsi53xx_la_SOURCES = $(libsi53xx_la_HEADERS) src/lib/si53xx.cxx

bin_PROGRAMS = si53xx
si53xx_SOURCES = src/bin/si53xx.cxx
si53xx_LDFLAGS = -lsi53xx
if HAVE_I2C2L
si53xx_SOURCES += src/bin/si53xx/i2c2l.hxx src/bin/si53xx/i2c2l.cxx
endif
if HAVE_MPSSE
si53xx_SOURCES += src/bin/si53xx/mpsse.hxx src/bin/si53xx/mpsse.cxx
si53xx_LDFLAGS += -lmpsse
endif

dist_bin_SCRIPTS = $(srcdir)/src/bin/si53xx-*

bash_completiondir = $(sysconfdir)/bash_completion.d
dist_bash_completion_DATA = $(srcdir)/src/bash_completion/si53xx*

dist_man_MANS = $(srcdir)/src/man/si53xx*.1
