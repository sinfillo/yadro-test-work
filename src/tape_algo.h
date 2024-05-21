#pragma once

#include "tape.h"

namespace Ydr {
void Sort(Tape& in, size_t n, Tape& out, Tape& buffer, size_t ram_ints_cnt, size_t scan_block_size);
}