> This file is part of si53xx, see <https://qu1x.org/si53xx>.
> 
> Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
> 
> si53xx is free software: you can redistribute it and/or modify
> it under the terms of the GNU Affero General Public License version 3
> as published by the Free Software Foundation on 19 November 2007.
> 
> si53xx is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
> GNU Affero General Public License for more details.
> 
> You should have received a copy of the GNU Affero General Public License
> along with si53xx. If not, see <https://www.gnu.org/licenses>.

si53xx
======

Interfacing Si5338 or Si5356 clock generator via i2c-dev or libmpsse.

Installation
============

Either getting a release
------------------------

1. Download stable source distribution tarball.

		wget https://qu1x.org/file/si53xx-1.0.0.tar.xz

2. Extract and enter.

		tar -xJf si53xx-1.0.0.tar.xz
		cd si53xx-1.0.0

Or getting a snapshot
---------------------

Required dependencies:

		git autoreconf libtool

1. Clone repository.

		git clone https://github.com/qu1x/si53xx.git

2. Enter and generate latest source distribution.

		cd si53xx
		./bootstrap

Installing one of them
----------------------

Required dependencies:

		build-essential

Optional dependencies:

		libmpsse libftdi libusb

1. Configure, build, and install.

		./configure --sysconfdir=/etc
		make
		sudo make install

2. Keep to uninstall someday.

		sudo make uninstall

Usage
=====

RTFM:

		man si53xx
		man si53xx-map
		man si53xx-cmp

1. Create one or more register maps as C code header files (.h) with the
   ClockBuilder Desktop Software from Silicon Labs. Such a register map contains
   a whole device configuration.

2. Convert these .h files to .map files with the si53xx-map script. These files
   are parsable by the si53xx-cmp script and si53xx application.

3. Optionally generate transition map pairs (.pos, .neg) from two or more .map
   files with the si53xx-cmp script. A transition map pair contains only the
   differences necessary to be applied by the si53xx application in order to
   switch from one register map to another while a .neg file revokes the changes
   applied by a .pos file.

4. Apply register or transition maps with the si53xx application or use its
   built-in routines to monitor the device status or modify its interrupt-mask.

Version
=======

si53xx-1.0.0 <https://qu1x.org/si53xx>

License
=======

GNU Affero General Public License version 3

Authors
=======

* Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>

