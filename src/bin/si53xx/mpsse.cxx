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

#include "mpsse.hxx"

#include <string>
#include <cstring>
#include <cstdio>

extern "C" {
#include <mpsse.h>
}

mpsse::mpsse(byte addr, std::size_t rate) {
	ctx = MPSSE(I2C, rate, MSB);
	ini(addr);
}

mpsse::mpsse(byte addr, std::size_t rate, std::size_t vid, std::size_t pid) {
	ctx = Open(vid, pid, I2C, rate, MSB, IFACE_ANY, nullptr, nullptr);
	ini(addr);
}

mpsse::mpsse(byte addr, std::size_t rate, std::size_t vid, std::size_t pid,
	std::size_t ind) {
	ctx = OpenIndex(vid, pid, I2C, rate, MSB, IFACE_ANY, nullptr, nullptr, ind);
	ini(addr);
}

void mpsse::ini(byte addr) {
	auto ctx = (struct mpsse_context*)this->ctx;
	if (!(ctx && ctx->open))
		throw (std::string)"Cannot initialize MPSSE (" + ErrorString(ctx) + ")";
	tx.reserve(64);
	slaw = addr << 1;
	slar = slaw + 1;
}

void mpsse::get(byte addr, byte* data, byte size) {
	auto ctx = (struct mpsse_context*)this->ctx;
	tx.clear();
	tx.emplace_back(slaw);
	tx.emplace_back(addr);
	Start(ctx);
	Write(ctx, (char*)tx.data(), tx.size());
	for (auto byte: tx)
		std::printf("%02x ", byte);
	std::printf("\n");
	Start(ctx);
	Write(ctx, (char*)&slar, 1);
	std::printf("%02x ", slar);
	if (size > 1) {
		SendAcks(ctx);
		if (char* rx = Read(ctx, size - 1)) {
			std::memcpy(data, rx, size - 1);
			std::free(rx);
		}
	}
	SendNacks(ctx);
	if (char* rx = Read(ctx, 1)) {
		data[size - 1] = *rx;
		std::free(rx);
	}
	for (std::size_t i = 0; i < size; ++i)
		std::printf("%02x ", data[i]);
	std::printf("\n");
}

void mpsse::set(byte addr, const byte* data, byte size) {
	auto ctx = (struct mpsse_context*)this->ctx;
	tx.clear();
	tx.emplace_back(slaw);
	tx.emplace_back(addr);
	tx.insert(tx.end(), data, data + size);
	Start(ctx);
	Write(ctx, (char*)tx.data(), tx.size());
	for (auto data : tx)
		std::printf("%02x ", data);
	std::printf("\n");
}

mpsse::~mpsse() {
	auto ctx = (struct mpsse_context*)this->ctx;
	Stop(ctx);
	Close(ctx);
}
