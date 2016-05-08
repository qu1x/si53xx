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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../lib/si53xx.hxx"

#ifdef HAVE_I2C2L
#include "si53xx/i2c2l.hxx"
#endif

#ifdef HAVE_MPSSE
#include "si53xx/mpsse.hxx"
#endif

#include <memory>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cerrno>
#include <vector>

#include <getopt.h>

const char* INFO =
PACKAGE_STRING "  <" PACKAGE_URL ">\n"
"\n"
"Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>\n"
"\n"
PACKAGE_TARNAME" is free software: you can redistribute it and/or modify\n"
"it under the terms of the GNU Affero General Public License version 3\n"
"as published by the Free Software Foundation on 19 November 2007.\n"
"\n"
PACKAGE_TARNAME " is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
"GNU Affero General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU Affero General Public License\n"
"along with " PACKAGE_TARNAME ". If not, see <https://www.gnu.org/licenses>.";

const char* HELP =
"si53xx - si53xx controller\n"
"\n"
"si53xx [OPTION]... [--] [MAP|POS|NEG]...\n"
"  -p, --port PORT                Bus interface driver.\n"
"  -a, --addr ADDR                Slave device address from 08 to 77.\n"
"  -r, --rate RATE                Transfer rate in kHz.\n"
"  -n, --get-part-number          Get part number.\n"
"  -i, --get-interrupt            Get interrupt against status.\n"
"  -I, --get-sticky-interrupt     Get interrupt against sticky status.\n"
"  -m, --get-interrupt-mask       Get interrupt mask.\n"
"  -M, --set-interrupt-mask DATA  Set interrupt mask to DATA.\n"
"  -s, --get-status               Get status.\n"
"  -t, --get-sticky-status        Get sticky status.\n"
"  -T, --set-sticky-status DATA   Set sticky status to DATA.\n"
"  -h, --help                     Print this help.\n"
"  -v, --version                  Print version information.\n"
"\n"
"  Control si53xx device by OPTIONs and MAP, POS, NEG files in given order.\n"
"\n"
"For complete documentation, run `man si53xx'.";

const char* HINT =
"\n"
"Try `si53xx --help' for more information.";

typedef si53xx::byte byte;

class opts {
public:
	int help, info;
	class cmd {
	public:
		enum what: int {
			get_part_number,
			get_interrupt,
			get_sticky_interrupt,
			get_interrupt_mask,
			set_interrupt_mask,
			get_status,
			get_sticky_status,
			set_sticky_status
		} type;
		byte data;
		byte mask;
		cmd(what type, byte data = 0x00, byte mask = 0x00):
			type(type), data(data), mask(mask) {}
	};
	std::vector<cmd> cmds;
	std::vector<std::string> maps;
	std::string port;
	std::size_t rate;
	byte addr;
	opts(): help(0), info(0), port("i2c2l"), rate(100000), addr(si53xx::PIN0) {
		cmds.reserve(10);
		maps.reserve(10);
	}
	bool parse(int& argc, char**& argv);
};

bool opts::parse(int& argc, char**& argv) {
	opterr = 0;
	const char* opts = "-:p:a:r:niImM:stT:hv";
	option optl[] = {
		{ "port", required_argument, nullptr, 'p' },
		{ "addr", required_argument, nullptr, 'a' },
		{ "rate", required_argument, nullptr, 'r' },
		{ "get-part-number", no_argument, nullptr, 'n' },
		{ "get-interrupt", no_argument, nullptr, 'i' },
		{ "get-sticky-interrupt", no_argument, nullptr, 'I' },
		{ "get-interrupt-mask", no_argument, nullptr, 'm' },
		{ "set-interrupt-mask", required_argument, nullptr, 'M' },
		{ "get-status", no_argument, nullptr, 's' },
		{ "get-sticky-status", no_argument, nullptr, 't' },
		{ "set-sticky-status", required_argument, nullptr, 'T' },
		{ "help", no_argument, &help, 1 },
		{ "version", no_argument, &info, 1 },
		{ nullptr, 0, nullptr, 0 }
	};
	int optv;
	auto fail = [&optv, &argv] (std::string msg) {
		msg += " option '";
		if (optv)
			msg.append("-").push_back((char)optv);
		else
			msg += argv[optind-1];
		msg += "'";
		throw msg;
	};
	auto to_number = [&fail] (const std::string& str, int base) {
		std::size_t num = 0;
		try {
			num = std::stoul(str, 0, base);
		}
		catch (const std::invalid_argument&) {
			fail("Invalid argument for");
		}
		catch (const std::out_of_range&) {
			fail("Too large argument for");
		}
		return num;
	};
	auto get = [this] (cmd::what type) {
		cmds.emplace_back(type);
	};
	auto set = [this, &optv, &fail] (cmd::what type) {
		enum: int {
			s, sys_cal,
			c, los_clkin,
			f, los_fdbk,
			p, pll_lol
		};
		byte data = 0x00;
		byte mask = 0x00;
		char* subopts = optarg;
		char* value;
		char* const token[] = {
			(char*)"s", (char*)"sys_cal",
			(char*)"c", (char*)"los_clkin",
			(char*)"f", (char*)"los_fdbk",
			(char*)"p", (char*)"pll_lol",
			nullptr
		};
		auto get = [&value, &data, &mask, &fail] (const char* sopt, byte flag) {
			if (!value)
				fail(std::string("No value for suboption '") + sopt + "' of");
			switch (value[0]) {
				case '1': data |= flag;
				case '0': mask |= flag; break;
				default: fail(std::string("Invalid value '") + value +
					"' for suboption '" + sopt + "' of");
			}
		};
		while (subopts[0])
			switch (getsubopt(&subopts, token, &value)) {
				case s: case sys_cal: get(token[s], si53xx::SYS_CAL); break;
				case c: case los_clkin: get(token[c], si53xx::LOS_CLKIN); break;
				case f: case los_fdbk: get(token[f], si53xx::LOS_FDBK); break;
				case p: case pll_lol: get(token[p], si53xx::PLL_LOL); break;
				default: fail(std::string("Unknown suboption '") +
					value + "' of");
			}
		cmds.emplace_back(type, data, mask);
	};
	while (-1 != (optv = getopt_long(argc, argv, opts, optl, nullptr))) {
		switch (optv) {
			case 'p': port = optarg; break;
			case 'a':
				if (!std::strcmp(optarg, "p0"))
					addr = si53xx::PIN0;
				else
				if (!std::strcmp(optarg, "p1"))
					addr = si53xx::PIN1;
				else
				if (2 == std::strlen(optarg))
					addr = to_number(optarg, 16);
				else
					fail("Invalid argument for");
			break;
			case 'r': rate = to_number(optarg, 10) * 1000; break;
			case 'n': get(cmd::get_part_number); break;
			case 'i': get(cmd::get_interrupt); break;
			case 'I': get(cmd::get_sticky_interrupt); break;
			case 'm': get(cmd::get_interrupt_mask); break;
			case 'M': set(cmd::set_interrupt_mask); break;
			case 's': get(cmd::get_status); break;
			case 't': get(cmd::get_sticky_status); break;
			case 'T': set(cmd::set_sticky_status); break;
			case 'h': help = 1; break;
			case 'v': info = 1; break;
			case  0 : break;
			case  1 : maps.emplace_back(optarg); break;
			case ':': optv = optopt; fail("Missing argument for"); break;
			case '?': optv = optopt; fail("Unknown"); break;
			default: throw std::string("Unknown error");
		}
	}
	return help || info;
}

int main(int argc, char** argv) {
	try {
		opts args = opts();
		try {
			if (args.parse(argc, argv)) {
				if (args.help)
					std::cout << HELP << "\n";
				else
				if (args.info)
					std::cout << INFO << "\n";
				return 0;
			}
		}
		catch (const std::string& err) {
			std::cerr << "Invalid usage: " << err << "\n";
			std::cerr << HINT << "\n";
			return 64;
		}
		try {
			if (!(0x08 <= args.addr && args.addr <= 0x77))
				throw std::string("Not within valid address range");
			if (args.rate > 400000)
				throw std::string("Maximum transfer rate of 400 kHz exceeded");
			std::istringstream port(args.port);
			std::string drv;
			std::getline(port, drv, ':');
			char sls[3];
			port.get(sls, sizeof sls);
			if (*sls && strcmp(sls, "//"))
				throw std::string("Missing double slashes of port");
			std::unique_ptr<si53xx> dev;
#ifdef HAVE_I2C2L
			if (!dev && drv == "i2c2l") {
				std::string ind;
				std::getline(port, ind);
				if (ind.empty())
					dev = std::make_unique<i2c2l>(args.addr);
				else {
					std::size_t ind_num;
					try {
						ind_num = std::stoul(ind, 0, 10);
					}
					catch (...) {
						throw std::string("Port of invalid IND number");
					}
					dev = std::make_unique<i2c2l>(args.addr, ind_num);
				}
			}
#endif
#ifdef HAVE_MPSSE
			if (!dev && drv == "mpsse") {
				std::string vid, pid, ind;
				std::getline(port, vid, ':');
				std::getline(port, pid, ':');
				std::getline(port, ind);
				if (vid.empty())
					dev = std::make_unique<mpsse>(args.addr, args.rate);
				else {
					if (pid.empty())
						throw std::string("Port of incomplete VID:PID pair");
					std::size_t vid_num, pid_num, ind_num;
					try {
						vid_num = std::stoul(vid, 0, 16);
						pid_num = std::stoul(pid, 0, 16);
					}
					catch (...) {
						throw std::string("Port of invalid VID:PID pair");
					}
					if (ind.empty())
						dev = std::make_unique<mpsse>(args.addr, args.rate,
							vid_num, pid_num);
					else {
						try {
							ind_num = std::stoul(ind, 0, 10);
						}
						catch (...) {
							throw std::string("Port of invalid IND number");
						}
						dev = std::make_unique<mpsse>(args.addr, args.rate,
							vid_num, pid_num, ind_num);
					}
				}
			}
#endif
			if (!dev)
				throw std::string("Unavailavle port driver '") + drv + "'";
			auto flag = [] (byte data) {
				return (std::string)
					"sys_cal=" + (data & si53xx::SYS_CAL ? '1' : '0') +
					",los_clkin=" + (data & si53xx::LOS_CLKIN ? '1' : '0') +
					",los_fdbk=" + (data & si53xx::LOS_FDBK ? '1' : '0') +
					",pll_lol=" + (data & si53xx::PLL_LOL ? '1' : '0');
			};
			for (auto cmd: args.cmds) {
				switch (cmd.type) {
					case opts::cmd::get_part_number:
						std::cout << std::string("## ") +
							dev->get_part_number() + "\n";
						break;
					case opts::cmd::get_interrupt:
						std::cout << std::string("## intr=") +
							(dev->get_interrupt() ? '1' : '0') + "\n";
						break;
					case opts::cmd::get_sticky_interrupt:
						std::cout << std::string("## intr_stk=") +
							(dev->get_sticky_interrupt() ? '1' : '0') + "\n";
						break;
					case opts::cmd::get_interrupt_mask:
						std::cout << std::string("## intr_mask: ") +
							flag(dev->get_interrupt_mask()) + "\n";
						break;
					case opts::cmd::set_interrupt_mask:
						dev->set_interrupt_mask(cmd.data, cmd.mask);
						break;
					case opts::cmd::get_status:
						std::cout << std::string("## status: ") +
							flag(dev->get_status()) + "\n";
						break;
					case opts::cmd::get_sticky_status:
						std::cout << std::string("## status_stk: ") +
							flag(dev->get_sticky_status()) + "\n";
						break;
					case opts::cmd::set_sticky_status:
						dev->set_sticky_status(cmd.data, cmd.mask);
						break;
				}
			}
			for (auto map: args.maps) {
				std::cout << "## " << map << "\n";
				if (!dev->set_register_map(map))
					throw std::string(std::strerror(errno));
			}
		}
		catch (const std::string& err) {
			std::cerr << err << "\n";
			return 65;
		}
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << "\n";
		return 113;
	}
	return 0;
}
