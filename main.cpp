#include <algorithm>
#include <iostream>
#include <optional>
#include <fstream>
#include <filesystem>

#include "src/config_parser.h"
#include "src/file_tape.h"
#include "src/tape_algo.h"

bool file_exists(const std::string& file_name) {
    if (!std::filesystem::exists(std::filesystem::path(file_name))) {
        std::cerr << file_name << " doesn't exist. Enter the correct file name.\n";
        return false;
    }
    return true;
}

template <typename T>
std::optional<T> cast_to_valid_number(const std::string& number_str) {
    try {
        T number = 0;
        if (std::is_same_v<T, int64_t>) {
            number = std::stoll(number_str);
        } else {
            number = std::stoi(number_str);
        }
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

    std::optional<size_t> tape_length_opt = cast_to_valid_number<size_t>(argv[3]);
    if (!tape_length_opt.has_value()) {
        return -1;
    }
    size_t tape_length = tape_length_opt.value();
    try {
        Ydr::ConfigParser config;
        config.Register("rw_latency", 40ll);        // nanoseconds
        config.Register("shift_latency", 30ll);     // nanoseconds
        config.Register("seek_latency", 10'000ll);  // nanoseconds
        config.Register("ram_size", 100'000ll);     // bytes
        config.Parse("config.txt");

        if (config.Get("rw_latency") <= 0 || config.Get("shift_latency") <= 0 ||
            config.Get("seek_latency") <= 0) {
            std::cerr << "Incorrect (negative) latency.\n";
            return -1;
        }
        constexpr int64_t MINIMUM_RAM = 10 * 4096;
        constexpr int64_t MINIMUM_BLOCKS_IN_RAM = 8;
        constexpr double SEEK_TO_SCAN_COEF = 2;  // k in inequality k * T(Seek) <= T(Scan)
        if (config.Get("ram_size") <= MINIMUM_RAM) {
            std::cerr << "Too small RAM.\n";
            return -1;
        }

        auto max_int_cnt_in_ram = config.Get("ram_size") / sizeof(int32_t);
        auto tape_block_size =
            std::clamp<int64_t>((config.Get("seek_latency") * SEEK_TO_SCAN_COEF) /
                                    (config.Get("rw_latency") + config.Get("shift_latency")),
                                1, max_int_cnt_in_ram / MINIMUM_BLOCKS_IN_RAM);

        if (!std::filesystem::exists(std::filesystem::path("tmp"))) {
            std::filesystem::create_directory("tmp");
        }
        Ydr::FileTape input_tape("tmp/bin_input");
        std::ifstream ifs(input_file);
        int32_t num;
        size_t read_nums = 0;
        while (ifs >> num) {
            input_tape.WriteAndMoveForward(num);
            ++read_nums;
        }
        if (read_nums != tape_length) {
            std::cerr << "Number count in program arguments isn't equal amount of numbers have "
                         "been read.";
            return -1;
        }
        Ydr::FileTape buf_tape("tmp/bin_buf");
        Ydr::FileTape output_tape("tmp/bin_output");
        Ydr::Sort(input_tape, tape_length, output_tape, buf_tape, max_int_cnt_in_ram / 2,
                  tape_block_size);

        std::ofstream ofs(output_file);
        for (size_t i = 0; i < tape_length; ++i) {
            ofs << output_tape.ReadAndMoveForward() << " ";
        }
    } catch (std::exception& e) {
        std::cerr << e.what();
        return -1;
    } catch (...) {
        std::cerr << "Something went wrong.\n";
        return -1;
    }
}