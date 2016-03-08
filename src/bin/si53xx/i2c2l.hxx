// This file is part of si53xx, see <https://qu1x.org/si53xx>.
// 
// Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
// 
// si53xx is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License version 3
// as published by the Free Software Foundation on 19 November 2007.
// 
// si53xx is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with si53xx. If not, see <https://www.gnu.org/licenses>.

#ifndef _I2C2L_H
#define _I2C2L_H

#include "../../lib/si53xx.hxx"

#include <string>
#include <vector>

class i2c2l: public si53xx {
	std::vector<byte> tx;
	int fd;
	byte slar;
	byte slaw;
	void ini(byte addr, std::string file);
public:
	i2c2l(byte addr);
	i2c2l(byte addr, byte ind);
	void get(byte addr, byte* data, byte size);
	void set(byte addr, const byte* data, byte size);
	~i2c2l();
};

#endif
