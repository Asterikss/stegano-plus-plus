#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace params {
std::string path_to_read = "";
std::string path_to_write = "";
std::string mess = "";
bool help_printed = false;
} // namespace params

namespace flags {
auto get_path_to_read() -> std::string { return params::path_to_read; }
auto get_path_to_write() -> std::string { return params::path_to_write; }
auto get_mess() -> std::string { return params::mess; }

auto help() -> bool {
  if (!params::help_printed) {
    params::help_printed = true;

    std::ostringstream help_message;

    help_message << R"(
~~~~~~~~~~
Supported extensions: bmp, ppm (P3 type)
Available options:
  -i/--info     : Checks if a file specified in the <path> can be modified.
                  Usage: -i <path>
  -e/--encrypt  : Opens the <path> and creates a mirror file with the <message> embedded.
                  Usage: -e <path> <message>
                  Note: <message> should be surrounded by double quotes.
  -d/--decrypt  : Reads an encrypted message in a file.
                  Only decrypt files that were encrypted with this program.
                  Usage: -d <path>
  -c/--check    : Checks if <message> can be written to a file specified by <path>.
                  Usage: -c <path> <message>
  -h/--help     : Prints this help message.

Example Usage:
  ./bin/stegopp -i there/here/picture.ppm -e here/there/image.ppm "Behind you"

Notes:
  - <path> cannot exceed 70 characters.
  - Maximum 12 arguments.
  - "-e" produces a new file, so using -e and -d on the same file simultaneously will produce gibberish.
  - Only one encryption or decryption per execute (i.e., cannot use -e twice).
  - To encrypt ppm P6 files, you can use an online converter to turn them into P3s.
~~~~~~~~~~
    )";

    std::cout << help_message.str();
  }

  return false;
}

auto reverse_str(std::string const &str) -> std::string {
  std::string tmp;
  for (int i = str.size() - 1; i >= 0; i--) {
    tmp += str[i];
  }
  return tmp;
}

/// Fetches the extension of the path given.
///
///@returns The extension of the file.
auto get_extension(std::string const &path) -> std::string {
  return reverse_str(path).substr(0, (reverse_str(path).find(".") + 1));
}

auto print_write_time(std::filesystem::file_time_type const &ftime) -> void {
  std::time_t cftime = std::chrono::system_clock::to_time_t(
      std::chrono::file_clock::to_sys(ftime));
  std::cout << "File write time is " << std::asctime(std::localtime(&cftime));
}

/// Displays the informationa about the permissions.
///
///@returns The owner_read permission.
auto print_permission(std::filesystem::perms const &p) -> char {
  namespace fs = std::filesystem;
  std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
            << ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
            << ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
            << ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
            << ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
            << ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
            << ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
            << ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
            << ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-")
            << '\n';
  return ((p & fs::perms::owner_read) != fs::perms::none ? 'r' : '-');
}

/// Checks and displays the informationa about the given path.
///
///@returns False if something went wrong.
auto check_extension(std::string const &path) -> bool {
  // This might actually fail later given e.g. ".ppm"
  if (path.size() > 70 || path.find(".") == std::string::npos) {
    std::cout << "Incorrect path\n";
    return help();
  }

  std::string exten = get_extension(path);
  if (exten != "mpp." && exten != "pmb.") {
    std::cout << "Not supported extention"
              << "\n";
    return help();
  }

  if (!std::filesystem::exists(path)) {
    std::cout << "Could not find the file (" << path << ")\n";
    return help();
  }

  std::cout << "\nFile has been found (" << path << ")\n";
  std::cout << "Format choosen: " << reverse_str(exten) << "\n";
  std::cout << "File size: " << std::filesystem::file_size(path) << "\n";
  // std::chrono_literals::print_last_write_time(std::filesystem::last_write_time(path));
  print_write_time(std::filesystem::last_write_time(path));
  std::cout << "Permissions: ";
  if (print_permission(std::filesystem::status(path).permissions()) == '-') {
    std::cout << "WARNING!!!: no premission to read this file. Encryption "
      "and decryption will probably fail\n";
  }
  std::cout << "\n";

  return true;
}

/// Function used when setting any flag.
///
/// Checks if the number of arguments meets the flag's requirements.
/// Checks if the extension is correct.
/// Moves the iterator to the possition before the next flag.
///@param curr Current state of the iterator.
///@param end Const iterator representing the end of the arguments.
///@param args Arguments parsed earlier.
///@param next_flag_in relative possition of the next flag.
///@param inner_arg_counter Used to check if their is nothing in bweetwen flags
///and if some of them were not used twice.
///@returns False if something went wrong.
auto usual_check(std::vector<std::string>::iterator &curr,
                 std::vector<std::string>::iterator const &end,
                 std::vector<std::string> const &args, int const next_flag_in,
                 int &inner_arg_counter) -> bool {
  inner_arg_counter++;

  if (curr + (next_flag_in - 1) >= end) {
    std::cout << "Missing arguments\n";
    return help();
  }

  curr += next_flag_in - 1;

  return check_extension(*(curr + 1 - (next_flag_in - 1)));
}

/// Parses the arguments.
///
/// Sets the paths and the message.
///@returns False if something went wrong.
auto parse(int const argc, char *arqv[]) -> bool {
  if (argc <= 1 || argc > 12) {
    return help();
  }

  std::vector<std::string> args;
  for (int i = 1; i < argc; i++) {
    args.push_back(arqv[i]);
  }

  int arg_counter = 0, inner_arg_counter = 0;

  for (auto curr = args.begin(), end = args.end(); curr != end;
       ++curr, arg_counter++) {

    if (*curr == "-h" || *curr == "--help") {
      help();
      inner_arg_counter++;
      // return false;
    }

    if (*curr == "-i" || *curr == "--info") {
      if (!usual_check(curr, end, args, 2, inner_arg_counter)) {
        return false;
      }
    }

    if ((*curr == "-e" || *curr == "--encrypt") &&
        params::path_to_write.size() == 0) {
      if (!usual_check(curr, end, args, 3, inner_arg_counter)) {
        return false;
      }
      params::path_to_write = *(curr - 1);
      params::mess = *(curr);
    }

    if ((*curr == "-d" || *curr == "--decrypt") &&
        params::path_to_read.size() == 0) {
      if (!usual_check(curr, end, args, 2, inner_arg_counter)) {
        return false;
      }
      params::path_to_read = *(curr);
    }

    if (*curr == "-c" || *curr == "--check") {
      if (!usual_check(curr, end, args, 3, inner_arg_counter)) {
        return false;
      }
      if (((std::filesystem::file_size(*(curr - 1))) -
           ((get_extension(*(curr - 1)) == "mpp.") ? 4 : 138)) <
          (((*(curr)).size() * 8))) {
        std::cout << "There is no place to write this message\n";
      } else
        std::cout << "Message can be written to that file\n";
    }

    if ((arg_counter + 1) != inner_arg_counter) {
      std::cout << "Wrong arguments\n";
      return help();
    }
  }
  return true;
}

} // namespace flags
