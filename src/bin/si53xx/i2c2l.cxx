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

#include "i2c2l.hxx"

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>

i2c2l::i2c2l(byte addr) {
	struct dirent** namelist;
	auto filter = [](const struct dirent* entry) -> int {
		return
		!fnmatch("i2c-[0-9]", entry->d_name, 0) ||
		!fnmatch("i2c-[0-9][0-9]", entry->d_name, 0) ||
		!fnmatch("i2c-[0-9][0-9][0-9]", entry->d_name, 0);
	};
	int n = scandir("/dev", &namelist, filter, alphasort);
	if (n < 0)
		throw std::string("Cannot scan busses: ") + std::strerror(errno);
	if (!n)
		throw std::string("No bus found");
	std::string file("/dev/");
	file += namelist[0]->d_name;
	while (n--)
		std::free(namelist[n]);
	std::free(namelist);
	ini(addr, file);
}

i2c2l::i2c2l(byte addr, byte ind) {
	ini(addr, std::string("/dev/i2c-") + std::to_string(ind));
}

void i2c2l::ini(byte addr, std::string file) {
	if (0 > (fd = open(file.data(), O_RDWR)))
		throw std::string("Cannot open bus '") + file + "': "
			+ std::strerror(errno);
	if (0 > ioctl(fd, I2C_SLAVE, addr))
		throw std::string("Cannot set slave address: ")
			+ std::strerror(errno);
	tx.reserve(64);
	slaw = addr << 1;
	slar = slaw + 1;
}

void i2c2l::get(byte addr, byte* data, byte size) {
	if (1 != write(fd, &addr, 1))
		throw std::string("Cannot get: Cannot write address: ")
			+ std::strerror(errno);
	std::printf("%02x %02x\n", slaw, addr);
	if ((ssize_t)size != read(fd, data, size))
		throw std::string("Cannot get: Cannot read data: ")
			+ std::strerror(errno);
	std::printf("%02x ", slar);
	for (std::size_t i = 0; i < size; ++i)
		std::printf("%02x ", data[i]);
	std::printf("\n");
}

void i2c2l::set(byte addr, const byte* data, byte size) {
	tx.clear();
	tx.emplace_back(addr);
	tx.insert(tx.end(), data, data + size);
	if ((ssize_t)tx.size() != write(fd, tx.data(), tx.size()))
		throw std::string("Cannot set: Cannot write address plus data: ")
			+ std::strerror(errno);
	std::printf("%02x ", slaw);
	for (auto byte : tx)
		std::printf("%02x ", byte);
	std::printf("\n");
}

i2c2l::~i2c2l() {
	close(fd);
}
