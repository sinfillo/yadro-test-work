#include "tape_algo.h"

namespace Ydr {

namespace {
/**
 * Invariants:
 * - Before call `in` and `out` are pointing to `start` position
 * - After call `in` and `out` are pointing to `(start + n)` position
 */
void MergeSort(Tape& in, size_t start, size_t n, Tape& out, Tape& left_buffer, Tape& right_buffer) {
    if (n == 1) {
        out.Write(in.Read());
        in.MoveForward();
        out.MoveForward();
        return;
    }
    size_t left_size = n / 2;
    size_t right_size = n - left_size;
    MergeSort(in, start, left_size, out, left_buffer, right_buffer);
    MergeSort(in, start + left_size, right_size, out, left_buffer, right_buffer);
    out.Seek(start);
    left_buffer.Seek(0);
    right_buffer.Seek(0);
    for (size_t i = 0; i < left_size; ++i) {
        left_buffer.Write(out.Read());
        out.MoveForward();
        left_buffer.MoveForward();
    }
    for (size_t i = 0; i < right_size; ++i) {
        right_buffer.Write(out.Read());
        out.MoveForward();
        right_buffer.MoveForward();
    }
    out.Seek(start);
    left_buffer.Seek(0);
    right_buffer.Seek(0);
    size_t left_pos = 0;
    size_t right_pos = 0;
    while (left_pos < left_size && right_pos < right_size) {
        auto left_num = left_buffer.Read();
        auto right_num = right_buffer.Read();
        if (left_num <= right_num) {
            out.Write(left_num);
            ++left_pos;
            left_buffer.MoveForward();
        } else {
            out.Write(right_num);
            ++right_pos;
            right_buffer.MoveForward();
        }
        out.MoveForward();
    }
    while (left_pos < left_size) {
        out.Write(left_buffer.Read());
        out.MoveForward();
        left_buffer.MoveForward();
        ++left_pos;
    }
    while (right_pos < right_size) {
        out.Write(right_buffer.Read());
        out.MoveForward();
        right_buffer.MoveForward();
        ++right_pos;
    }
}
}  // namespace

void Sort(Tape& in, size_t n, Tape& out, Tape& left_buffer, Tape& right_buffer) {
    in.Seek(0);
    out.Seek(0);
    MergeSort(in, 0, n, out, left_buffer, right_buffer);
    in.Seek(0);
    out.Seek(0);
}
}  // namespace Ydr
