#include <iostream>
#include <optional>
#include <fstream>
#include <filesystem>

#include "src/file_tape.h"
#include "src/tape_algo.h"

bool file_exists(const std::string& file_name) {
    if (!std::filesystem::exists(std::filesystem::path(file_name))) {
        std::cerr << file_name << " doesn't exist. Enter the correct file name.\n";
        return false;
    }
    return true;
}

std::optional<size_t> cast_to_valid_number(const std::string& number_str) {
    try {
        size_t number = std::stoll(number_str);
        if (number <= 0) {
            std::cerr << "Enter the natural number.\n";
            return std::nullopt;
        }
        return number;
    } catch (...) {
        std::cerr << number_str << " is not number. Enter the correct number.\n";
        return std::nullopt;
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Invalid numbers of arguments. Enter the input file, the "
                     "output file and the number of numbers in the input file.\n";
        return -1;
    }

    std::string input_file = argv[1];
    if (!file_exists(input_file)) {
        return -1;
    }
    std::string output_file = argv[2];
    if (input_file == output_file) {
        std::cerr << "Identical files on input and output. Enter different files.\n";
        return -1;
    }

    std::optional<size_t> tape_length_opt = cast_to_valid_number(argv[3]);
    if (!tape_length_opt.has_value()) {
        return -1;
    }
    size_t tape_lenght = tape_length_opt.value();

    if (!std::filesystem::exists(std::filesystem::path("tmp"))) {
        std::filesystem::create_directory("tmp");
    }
    Ydr::FileTape input_tape("tmp//bin_input");
    std::ifstream ifs(input_file);
    int32_t num;
    while (ifs >> num) {
        input_tape.Write(num);
        input_tape.MoveForward();
    }
    Ydr::FileTape left_buf_tape("tmp//bin_left_buf");
    Ydr::FileTape right_buf_tape("tmp//bin_right_buf");
    Ydr::FileTape output_tape("tmp//bin_output");
    Ydr::Sort(input_tape, tape_lenght, output_tape, left_buf_tape, right_buf_tape);

    std::ofstream ofs(output_file);
    for (size_t i = 0; i < tape_lenght; ++i) {
        ofs << output_tape.Read() << " ";
        output_tape.MoveForward();
    }
}