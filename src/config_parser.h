#pragma once

#include <map>

namespace Ydr {
class ConfigParser {
public:
    void Register(const std::string& param_name, int64_t default_val);
    void Parse(const std::string& config_filename);
    int64_t Get(const std::string& param_name) const;

private:
    static const std::map<char, int64_t> unit_ram_map;
    std::map<std::string, int64_t> config_values_;
};
}  // namespace Ydr
