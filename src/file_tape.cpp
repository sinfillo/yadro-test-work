#include "file_tape.h"

namespace Ydr {

FileTape::FileTape(const std::string& filename)
    : io_(filename, std::ios::in | std::ios::out | std::ios::trunc) {
}

int32_t FileTape::Read() {
    int32_t res;
    // TODO: find a better way of reading int.
    io_.read(reinterpret_cast<char*>(&res), sizeof(res));
    MoveBack();
    return res;
}

void FileTape::Write(int32_t num) {
    io_.write(reinterpret_cast<char*>(&num), sizeof(num));
    MoveBack();
}

void FileTape::MoveForward() {
    io_.seekg(sizeof(int32_t), std::ios_base::cur);
}

void FileTape::MoveBack() {
    io_.seekg(-sizeof(int32_t), std::ios_base::cur);
}

void FileTape::Seek(size_t pos) {
    io_.seekg(sizeof(int32_t) * pos);
}

}  // namespace Ydr
