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

Interfacing si5338 or si5356 clock generator via i2c-dev or libmpsse.

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

Version
=======

si53xx-1.0.0 <https://qu1x.org/si53xx>

License
=======

GNU Affero General Public License version 3

Authors
=======

* Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>

