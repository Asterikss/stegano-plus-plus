#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace flags{
	auto get_path_to_read() -> std::string;
	auto get_path_to_write() -> std::string;
	auto get_mess() -> std::string;
	auto get_extension(std::string const& path) -> std::string;
	auto help() -> void;
	auto reverse_str(std::string const& str) -> std::string;
	auto print_last_write_time(std::filesystem::file_time_type const& ftime) -> void;
	auto check_extension(std::string const& path) -> bool;
	auto usual_check(std::vector<std::string>::iterator &curr, std::vector<std::string>::iterator const& end, std::vector<std::string> const& args, int const& next_flag_in, int& inner_arg_counter) -> bool;
	auto print_permission(std::filesystem::perms const& p) -> void;
	auto parse(int const argc, char *arqv[]) -> bool;
}
