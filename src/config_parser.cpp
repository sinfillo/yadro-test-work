#include "config_parser.h"

#include <string>
#include <fstream>

namespace Ydr {
const std::map<char, int64_t> ConfigParser::unit_ram_map = {
    {'K', 1000}, {'M', 1'000'000}, {'G', 1'000'000'000}};

void ConfigParser::Register(const std::string& param_name, int64_t default_val) {
    config_values_[param_name] = default_val;
}

void ConfigParser::Parse(const std::string& config_filename) {
    std::string param_name;
    std::string param_value;
    std::ifstream ifs_config(config_filename);
    while (ifs_config >> param_name >> param_value) {
        std::optional<char> unit_ram;
        if (unit_ram_map.find(param_value.back()) != unit_ram_map.end()) {
            unit_ram = param_value.back();
            param_value.pop_back();
        }
        if (!config_values_.contains(param_name)) {
            throw std::runtime_error("Unknown parameter in config");
        }
        config_values_[param_name] = std::stoll(param_value);
        if (unit_ram) {
            config_values_[param_name] *= unit_ram_map.at(*unit_ram);
        }
    }
}

int64_t ConfigParser::Get(const std::string& param_name) const {
    return config_values_.at(param_name);
}

}  // namespace Ydr
