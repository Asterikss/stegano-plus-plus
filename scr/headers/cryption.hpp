#pragma once

#include <vector>
#include <string>

namespace cryption{
	auto conv_bits_to_mess(std::vector<std::bitset<8>> const& bits) -> void;
	auto conv_str_to_bin(std::string const& str, bool const tail) -> std::vector<std::bitset<8>>;
	auto change_last_bits(unsigned char& RGB, std::vector<std::bitset<8>> const& bits, int& index_set, int& index_bit) -> bool;
	auto bad_conv_bits_to_mess(std::vector<int> const& bits) -> std::vector<char>;
	auto bad_conv_str_to_bin(std::string const& str, bool const tail) -> std::vector<int>;
	auto bad_change_last_bits(int& RGB, std::vector<int> const& bin, int& index) -> bool;
}
