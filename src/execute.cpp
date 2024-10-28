#include <bitset>
#include <fstream>
#include <iostream>
#include <string>

#include "cryption.hpp"
#include "flags.hpp"

namespace execute {

auto ex_parse(int argc, char *arqv[]) -> bool {
  return flags::parse(argc, arqv);
}

auto new_file_name(std::string const &fname) -> std::string {
  std::filesystem::path path(fname);
  return path.parent_path().string() + "/" + path.stem().string() + "_encrypted" + path.extension().string();
}

/// Encrypts the message in a bmp file.
///
/// Checks the type of the bmp file.
/// Displays the dimensions of the image.
/// Creates a new file with the appropriate ammount of bits changed.
///@returns Void.
auto encrypt_bmp() -> void {
  std::ifstream original_image;
  original_image.open(flags::get_path_to_write(), std::ios::binary);

  if (!original_image.is_open()) {
    std::cout << "File to be encrypted could not be opened\n";
    return;
  }
  std::cout << "File to be encrypted has been opened\n";

  const int file_header_size = 26;
  unsigned char file_header[file_header_size];
  original_image.read(reinterpret_cast<char *>(file_header), file_header_size);

  if (file_header[0] != 'B' || file_header[1] != 'M') {
    std::cout << "Incorrenct format (not a bit map (\"BM\"))\n";
  }

  const int file_size = file_header[2] + (file_header[3] << 8) +
                        (file_header[4] << 16) + (file_header[5] << 24);
  const int possition_of_pixels = file_header[10] + (file_header[11] << 8) +
                                  (file_header[12] << 16) +
                                  (file_header[13] << 24);
  const int width = file_header[18] + (file_header[19] << 8) +
                    (file_header[20] << 16) + (file_header[21] << 24);
  const int height = file_header[22] + (file_header[23] << 8) +
                     (file_header[24] << 16) + (file_header[25] << 24);

  std::cout << "Image dimensions: width: " << width << ", height: " << height
            << "\n";

  const int padding = ((4 - (width * 3) % 4) % 4);

  const int add_info_size = possition_of_pixels - file_header_size;
  unsigned char add_info[add_info_size];
  original_image.read(reinterpret_cast<char *>(add_info), add_info_size);

  const int pixels_size = file_size - possition_of_pixels;
  unsigned char pixels_bgr[pixels_size];
  original_image.read(reinterpret_cast<char *>(pixels_bgr), pixels_size);

  const std::vector<std::bitset<8>> bin_mess =
      cryption::conv_str_to_bin(flags::get_mess(), true);
  int index_pix = 0, index_bit = 0, index_set = 0;

  bool has_next = true;
  const bool has_padding = (padding > 0);
  const int start_of_padding = width;

  while (has_next) {
    if (has_padding && ((index_pix) / 3) == start_of_padding) {
      index_pix += (padding);
    } else {
      if (!cryption::change_last_bits(pixels_bgr[index_pix++], bin_mess,
                                      index_set, index_bit)) {
        has_next = false;
      }
    }
  }

  std::ofstream new_image;
  new_image.open(new_file_name(flags::get_path_to_write()), std::ios::binary);
  if (!new_image.is_open()) {
    std::cout << "New file could not be opened\n";
    return;
  }
  std::cout << "New file opened\n";

  new_image.write(reinterpret_cast<char *>(file_header), file_header_size);
  new_image.write(reinterpret_cast<char *>(add_info), add_info_size);
  new_image.write(reinterpret_cast<char *>(pixels_bgr), pixels_size);

  new_image.close();

  std::cout << "Encryption completed\n\n";
}

/// Tries to retrive a message from the bmp file specified.
///
/// Displays the dimensions of the image.
///@returns Void.
auto read_mess_bmp() -> void {
  std::ifstream image;
  image.open(flags::get_path_to_read(), std::ios::binary);
  // TODO file size changes for some reason for ppms

  if (!image.is_open()) {
    std::cout << "File to decrypt could not be opened\n";
    return;
  }
  std::cout << "File to decrypt has been opened\n";

  const int file_header_size = 26;
  unsigned char file_header[file_header_size];
  image.read(reinterpret_cast<char *>(file_header), file_header_size);

  const int possition_of_pixels = file_header[10] + (file_header[11] << 8) +
                                  (file_header[12] << 16) +
                                  (file_header[13] << 24);
  const int width = file_header[18] + (file_header[19] << 8) +
                    (file_header[20] << 16) + (file_header[21] << 24);
  const int height = file_header[22] + (file_header[23] << 8) +
                     (file_header[24] << 16) + (file_header[25] << 24);
  const int padding = ((4 - (width * 3) % 4) % 4);

  const int skip_size = possition_of_pixels - file_header_size;
  unsigned char skip[skip_size];
  image.read(reinterpret_cast<char *>(skip), skip_size);

  auto bits = std::vector<std::bitset<8>>();
  const auto tail = std::bitset<8>("01111111");
  bool not_end = true;

  const int start_of_padding = width;
  const bool has_padding = (padding > 0);
  int counter_bit = 0, counter_sets = 0;

  auto transition_set = std::bitset<8>();

  unsigned char pixels[1];

  while (not_end && image.read(reinterpret_cast<char *>(pixels), 1)) {

    if ((((pixels[0]) >> 1) & 1) == 1) {
      transition_set[counter_bit] = 1;
    } else {
      transition_set[counter_bit] = 0;
    }

    counter_bit++;

    if (((pixels[0]) & 1) == 1) {
      transition_set[counter_bit] = 1;
    } else {
      transition_set[counter_bit] = 0;
    }
    counter_bit++;

    if (has_padding && (((counter_bit / 2) / 3) == start_of_padding)) {
      image.ignore(padding);
    }

    if (counter_bit == 8) {
      if (transition_set == tail) {
        not_end = false;
        std::cout << "\nEnd reached" << "\n";
      } else {
        bits.push_back(transition_set);
      }
    }

    if (counter_bit >= 8) {
      counter_bit = 0;
      counter_sets++;
    }
  }
  image.close();

  std::cout << "Extracted:\n";
  cryption::conv_bits_to_mess(
      bits); // z deklaracja mozna//dlaczgo to vector charow
}

/// Encrypts the message in a ppm file.
///
/// Checks the type of the ppm file.
/// Displays the dimensions of the image.
/// Creates a new file with the appropriate ammount of bits changed.
///@returns Void.
auto encrypt_ppm() -> void {
  std::ifstream original_image;
  std::ofstream new_image;
  original_image.open(flags::get_path_to_write());
  new_image.open(new_file_name(flags::get_path_to_write()));

  if (!(original_image.is_open() && new_image.is_open())) {
    std::cout << "File to encrypt could not be opened" << "\n";
    return;
  }
  std::cout << "File to encrypt has been opened" << "\n";
  std::string type, width, height, RGB;
  original_image >> type;
  if (type != "P3") {
    std::cout << "Warning: only ppm files of type \"P3\" are supported.\n"
                 "Type found: "
              << type << "\n";
    flags::help();
    return;
  }
  original_image >> width;
  original_image >> height;
  original_image >> RGB;
  std::cout << "Image dinesions: width: " << width << " height: " << height;
  new_image << type << "\n" << width << " " << height << "\n" << RGB << "\n";

  std::vector<int> bin = cryption::bad_conv_str_to_bin(flags::get_mess(), true);

  std::string red, green, blue;
  int r, g, b;

  int index = 0;
  while (original_image >> red) {
    original_image >> green;
    original_image >> blue;

    std::stringstream redstream(red);
    std::stringstream greenstream(green);
    std::stringstream bluestream(blue);

    redstream >> r;
    greenstream >> g;
    bluestream >> b;

    if (index < bin.size()) {
      if (cryption::bad_change_last_bits(r, bin, index)) {
        if (cryption::bad_change_last_bits(g, bin, index)) {
          cryption::bad_change_last_bits(b, bin, index);
        }
      }
    }

    new_image << r << " " << g << " " << b << "\n";
  }

  original_image.close();
  new_image.close();

  std::cout << "\nEncryption completed\n\n";
}

/// Tries to retrive a message from the ppm file specified.
///
///@returns Void.
auto read_mess_ppm() -> void {
  std::ifstream image;
  image.open(flags::get_path_to_read());

  if (!image.is_open()) {
    std::cout << "File to decrypt could not be opened" << "\n";
    return;
  }
  std::cout << "File to decrypt has been opened" << "\n";

  auto mess = std::vector<char>();
  auto vec_bits = std::vector<int>();
  bool end = false;
  const auto vec_tail = std::vector<int>{0, 1, 1, 1, 1, 1, 1, 1};
  auto counter = 0;
  auto vec_tmp = std::vector<int>();

  std::string RGB = "";
  int RGB_int = 0;
  for (int i = 0; i < 4; i++) {
    image >> RGB;
  }
  while (image >> RGB && end == false) {
    std::stringstream RGB_stream(RGB);

    RGB_stream >> RGB_int;

    vec_bits.push_back((RGB_int >> 1) % 2);
    vec_bits.push_back(RGB_int % 2);

    counter++;

    if (counter >= 4 && counter % 4 == 0) {
      vec_tmp = std::vector<int>(vec_bits.end() - 8, vec_bits.end());
      if (vec_tmp == vec_tail) {
        end = true;
        std::cout << "End reached\n";
      }
    }
  }
  mess = cryption::bad_conv_bits_to_mess(
      vec_bits); // z deklaracja mozna//dlaczgo to vector charow

  image.close();

  std::cout << "Extracted: \n";
  for (auto elem : mess) {
    std::cout << elem;
  }
  std::cout << "\n";
}

/// When decrypting a file, points to the right method.
///
///@returns Void.
auto decrypt() -> void {
  if (flags::get_path_to_read().size() == 0) {
    return;
  }

  switch (flags::get_file_type(flags::get_path_to_read())) {
    case FileType::PPM:
      read_mess_ppm();
      break;
    case FileType::BMP:
      read_mess_bmp();
      break;
    default:
      throw std::runtime_error("Unsupported file extension");
  }
}

/// When encrypting a file, points to the right method.
///
///@returns Void.
auto encrypt() -> void {
  if (flags::get_path_to_write().size() == 0) {
    return;
  }

  switch (flags::get_file_type(flags::get_path_to_write())) {
    case FileType::PPM:
      encrypt_ppm();
      break;
    case FileType::BMP:
      encrypt_bmp();
      break;
    default:
      throw std::runtime_error("Unsupported file extension");
  }
}

auto execute() -> void {
  encrypt();
  decrypt();
}

} // namespace execute
