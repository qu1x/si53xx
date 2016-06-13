// This file is part of libsi53xx, see <https://qu1x.org/libsi53xx>.
// 
// Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
// 
// libsi53xx is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License version 3
// as published by the Free Software Foundation on 19 November 2007.
// 
// libsi53xx is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with libsi53xx. If not, see <https://www.gnu.org/licenses>.

#ifndef _SI53XX_H
#define _SI53XX_H

#include <string>

class si53xx {
public:
	typedef unsigned char byte;
	static const byte
	PIN0 = 0x70,
	PIN1 = PIN0 + 1;
	static const byte
	SYS_CAL = 1 << 0,
	LOS_CLKIN = 1 << 2,
	LOS_FDBK = 1 << 3,
	PLL_LOL = 1 << 4;
	void disarm();
	void arm();
	bool set_register_map(std::string file);
	std::string get_part_number();
	bool get_interrupt();
	bool get_sticky_interrupt();
	byte get_interrupt_mask();
	void set_interrupt_mask(byte data, byte mask = 0xFF);
	byte get_status();
	byte get_sticky_status();
	void set_sticky_status(byte data, byte mask = 0xFF);
	virtual ~si53xx() {};
protected:
	virtual void get(byte addr, byte* data, byte size) = 0;
	virtual void set(byte addr, const byte* data, byte size) = 0;
private:
	static const timespec rep;
	static const timespec rst;
	byte rx(byte addr) { byte data; get(addr, &data, 1); return data; }
	void tx(byte addr, byte data) { set(addr, &data, 1); }
	void xx(byte addr, byte data, byte mask);
	void pg(byte page) { tx(255, page & 0x01); }
};

#endif
