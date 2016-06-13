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

#include "si53xx.hxx"

#include <fstream>
#include <cstdlib>
#include <vector>

#include <time.h>

const timespec si53xx::rep = { 0, 5'000'000 };

const timespec si53xx::rst = { 0, 25'000'000 };

void si53xx::xx(byte addr, byte data, byte mask) {
	if (mask == 0xFF)
		tx(addr, data);
	else
	if (mask != 0x00)
		tx(addr, (rx(addr) & ~mask) | (data & mask));
}

void si53xx::disarm() {
	tx(230, 0x10);
	tx(241, 0xE5);
}

void si53xx::arm(byte mask) {
	const byte base = rx(2) & 0x3F;
	if (base == 38) {
		for (std::size_t reps = 5; reps
		&& mask && rx(218) & mask; --reps)
			nanosleep(&rep, nullptr);
		tx(49, rx(49) & 0x7F);
	}
	tx(246, 0x02);
	if (base == 38) {
		nanosleep(&rst, nullptr);
		tx(241, 0x65);
		for (std::size_t reps = 5; reps
		&& mask && rx(218) & (mask | SYS_CAL | PLL_LOL); --reps)
			nanosleep(&rep, nullptr);
		byte data[3];
		get(235, data, sizeof data);
		data[2] = (rx(47) & 0xFC) | (data[2] & 0x03);
		set(45, data, sizeof data);
		tx(49, rx(49) | 0x80);
	}
	tx(230, 0x00);
}

bool si53xx::set_register_map(std::string file) {
	std::fstream fs(file);
	byte burst_addr = 0;
	std::vector<byte> burst_data;
	burst_data.reserve(64);
	for (std::string ln; std::getline(fs, ln);) {
		auto pos = (char*)ln.data();
		byte addr = std::strtoul(pos, &pos, 16);
		byte data = std::strtoul(pos, &pos, 16);
		byte mask = std::strtoul(pos, &pos, 16);
		if (mask == 0xFF
		&& (burst_data.empty() || addr == burst_addr + burst_data.size())) {
			if (burst_data.empty())
				burst_addr = addr;
			burst_data.emplace_back(data);
		}
		else {
			if (burst_data.size()) {
				set(burst_addr, burst_data.data(), burst_data.size());
				burst_data.clear();
			}
			xx(addr, data, mask);
		}
	}
	if (burst_data.size())
		set(burst_addr, burst_data.data(), burst_data.size());
	return fs.eof();
}

std::string si53xx::get_part_number() {
	byte data[6];
	get(0, data, sizeof data);
	std::string part_number;
	part_number.reserve(17);
	part_number += "Si53";
	auto base_number = std::to_string(data[2] & 0x3F);
	if (base_number.size() > 2)
		part_number += "??";
	else
		part_number.append(2 - base_number.size(), '0') += base_number;
	byte grade = ((data[3] & 0xF8) >> 3) + 0x40;
	part_number += grade;
	part_number += '-';
	part_number += (data[0] & 0x07) + 0x41;
	auto nvm_code = std::to_string(
		((data[2] & 0x01) << 16) + (data[4] << 8) + data[5]);
	if (nvm_code.size() > 5)
		part_number += "?????";
	else
		part_number.append(5 - nvm_code.size(), '0') += nvm_code;
	return part_number += "-GM";
}

bool si53xx::get_interrupt() {
	return (rx(218) & 0x1D) & ~(rx(6) & 0x1D);
}

bool si53xx::get_sticky_interrupt() {
	return (rx(247) & 0x1D) & ~(rx(6) & 0x1D);
}

si53xx::byte si53xx::get_interrupt_mask() {
	return ~(rx(6) & 0x1D);
}

void si53xx::set_interrupt_mask(byte data, byte mask) {
	xx(6, ~data, mask & 0x1D);
}

si53xx::byte si53xx::get_status() {
	return rx(218) & 0x1D;
}

si53xx::byte si53xx::get_sticky_status() {
	return rx(247) & 0x1D;
}

void si53xx::set_sticky_status(byte data, byte mask) {
	xx(247, data, mask & 0x1D);
}
