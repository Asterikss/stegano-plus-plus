#pragma once

#include <string>

namespace execute{
	auto execute() -> void;
	auto ex_parse(int argc, char *arqv[]) -> bool;
	auto encrypt() -> void;
	auto decrypt() -> void;
	auto encrypt_ppm() -> void;
	auto encrypt_bmp() -> void;
	auto read_mess_ppm() -> void;
	auto read_mess_bmp() -> void;
	auto new_file_name(std::string const& name) -> std::string;
}
