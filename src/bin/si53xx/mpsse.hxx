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

#ifndef _MPSSE_H
#define _MPSSE_H

#include "../../lib/si53xx.hxx"

#include <cstddef>
#include <vector>

class mpsse: public si53xx {
	std::vector<byte> tx;
	void* ctx;
	byte slar;
	byte slaw;
	void ini(byte addr);
public:
	mpsse(byte addr, std::size_t rate = 100000);
	mpsse(byte addr, std::size_t rate, std::size_t vid, std::size_t pid);
	mpsse(byte addr, std::size_t rate, std::size_t vid, std::size_t pid,
		std::size_t ind);
	void get(byte addr, byte* data, byte size);
	void set(byte addr, const byte* data, byte size);
	~mpsse();
};

#endif
