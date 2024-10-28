#include <iostream>
#include <vector>
#include <string>
#include <bitset>

namespace cryption{

	///Changes the LSB and the "second" LSB.
	///
	///Used in binary files.
	///@param RGB Value in which bits should be changed.
	///@param bits Bits gotten from converting the message.
	///@param index_set Index of the 8 bit bitsets closed int a vector.
	///@param index_bit Index of the bits closed in the 8 bit bitset.
	///@returns True if there are more bits that need to be changed.
	auto change_last_bits(unsigned char& RGB, std::vector<std::bitset<8>> const& bits, int& index_set, int& index_bit) -> bool{
		auto to_bit = std::bitset<8>(RGB);

		if(to_bit[1] != bits[index_set][index_bit]){
			to_bit[1] = ((to_bit[1] & 0) | bits[index_set][index_bit]);
		}
		index_bit++;

		if(to_bit[0] != bits[index_set][index_bit]){
			to_bit[0] = ((to_bit[0] & 0) | bits[index_set][index_bit]);
		}
		index_bit++;

		if (index_bit >= 8){
			index_bit = 0;
			index_set++;
		}		

		RGB = to_bit.to_ullong();

		return index_set < bits.size();
	}

	///Converts the string given to its bit by bit representation closed into the vector if bitsets.
	///
	///@param str
	///@param tail If set to true, function adds the "mark" at the end that signifies the end.
	///@returns True if there are more bits that need to be changed.
	auto conv_str_to_bin(std::string const& str, bool const tail) -> std::vector<std::bitset<8>>{
		auto bits = std::vector<std::bitset<8>>(str.begin(), str.end());
		if (tail){
			bits.push_back(std::bitset<8>("01111111"));
		}
		return bits;

	}

	///Displays message converted from the vector of bitsets.
	///
	///@param bits
	///@return Void
	auto conv_bits_to_mess(std::vector<std::bitset<8>> const& bits) -> void{
		for (int i = 0; i < bits.size() ; i++){
			std::cout << (char)bits[i].to_ullong();
		}
		std::cout << "\n";

	}

	///Clumsy way of converting bits represented in a vector to a message represeneted in a vector.
	///
	///@param bits
	///@returns The vector of chars representing a message.
	auto bad_conv_bits_to_mess(std::vector<int> const& bits) -> std::vector<char>{
		auto mess = std::vector<char>();
		unsigned int to_int;

		for (int i = 0; i < bits.size()/8 ; i++){
			to_int = bits[i*8];
			int counter_start = i*8 + 1;
			int counter_finish = counter_start+7;

			for (int j = counter_start; j < counter_finish ; j++){
				to_int *= 2;
				to_int += bits[j];
			}
			mess.push_back((char)to_int);
		}
		return mess;
	}

	///Clumsy way of changing the LSB and the "second" LSB.
	///
	///Used in txt files.
	///@param RGB Value in which bits should be changed.
	///@param bin Binary representation of a message gotten earlier.
	///@param index Index of the bin (vector).
	///@returns True if there are more bits that need to be changed.
	auto bad_change_last_bits(int& RGB, std::vector<int> const& bin, int& index) -> bool{
		int LSB = RGB%2;
		int SLSB = (RGB/2)%2;

		if(SLSB != bin[index++]){
			if (SLSB == 1) RGB -= 2;
			else	RGB += 2;
		}

		if(LSB != bin[index++]){
			if (LSB == 1) RGB -= 1;
			else	RGB += 1;
		}
		return index < bin.size();

	}

	///Clumsy way of converting string to its binary representation in a vector.
	///
	///@param str
	///@param tail If set to true, function adds the "mark" at the end that signifies the end.
	///@returns The vector if ints.
	auto bad_conv_str_to_bin(std::string const& str, bool const tail) -> std::vector<int>{
		std::vector<int> bin;
		for (int i = 0; i < str.size(); i++){
			int letter_ascii = int(str[i]);
			std::vector<int> tmp;
			while (letter_ascii > 0){
				(letter_ascii%2)? tmp.push_back(1) : tmp.push_back(0);
				letter_ascii /= 2;
			}
			while (tmp.size() < 8){
				tmp.push_back(0);
			}
      reverse(tmp.begin(), tmp.end());
			bin.insert(bin.end(),tmp.begin(),tmp.end());
		}

		if (tail){
			bin.push_back(0);
			for (int i = 0; i < 7 ; i++){
				bin.push_back(1);
			}
		}
		return bin;
	}

}
