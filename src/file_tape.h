#pragma once

#include "tape.h"

#include <fstream>
#include <string>

namespace Ydr {
class FileTape : public Tape {
public:
    FileTape(const std::string& filename);

    int32_t Read() override;
    void Write(int32_t num) override;
    void MoveForward() override;
    void MoveBack() override;
    void Seek(size_t pos) override;

private:
    std::fstream io_;
};
}  // namespace Ydr