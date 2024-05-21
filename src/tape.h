#pragma once

#include <cstddef>
#include <cstdint>

namespace Ydr {
class Tape {
public:
    virtual int32_t Read() = 0;
    virtual void Write(int32_t num) = 0;
    virtual int32_t ReadAndMoveForward() = 0;
    virtual void WriteAndMoveForward(int32_t num) = 0;
    virtual void MoveForward() = 0;
    virtual void MoveBack() = 0;
    virtual void Seek(size_t pos) = 0;
    virtual ~Tape() = default;
};
}  // namespace Ydr
