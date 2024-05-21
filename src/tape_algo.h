#pragma once

#include "tape.h"

namespace Ydr {
void Sort(Tape& in, size_t n, Tape& out, Tape& left_buffer, Tape& right_buffer);
}