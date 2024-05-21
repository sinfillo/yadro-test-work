#include "tape_algo.h"

#include <algorithm>
#include <vector>
#include <queue>

namespace Ydr {

namespace {

void ScanFromTape(Tape& tape, int* destination, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        destination[i] = tape.ReadAndMoveForward();
    }
}

void WriteOnTape(Tape& tape, int* from, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        tape.WriteAndMoveForward(from[i]);
    }
}

struct ElementFromArrays {
    int32_t value;
    size_t array_ind;

    bool operator>(const ElementFromArrays& oth) const {
        return value > oth.value;
    }
};

/**
 * Invariants:
 * - Before call `in` and `out` are pointing to `start` position
 * - After call `in` and `out` are pointing to `(start + n)` position
 */
void MergeSort(Tape& in, size_t start, size_t n, Tape& out, Tape& buffer, size_t branches,
               size_t block_size) {
    if (n <= branches * block_size) {
        // Small size, dummy sort
        std::vector<int32_t> block(n);
        block.shrink_to_fit();
        ScanFromTape(in, block.data(), n);
        std::sort(block.begin(), block.end());
        WriteOnTape(out, block.data(), n);
        return;
    }
    auto part_size = n / branches;
    for (size_t i = 0; i + 1 < branches; ++i) {
        MergeSort(in, start + i * part_size, part_size, out, buffer, branches, block_size);
    }
    MergeSort(in, start + (branches - 1) * part_size, n - (branches - 1) * part_size, out, buffer,
              branches, block_size);
    std::priority_queue<ElementFromArrays, std::vector<ElementFromArrays>, std::greater<>> heap;
    std::vector<std::vector<int32_t>> blocks(branches);
    std::vector<size_t> ind_in_block(branches);
    std::vector<size_t> block_cnt_from_part(branches);
    blocks.shrink_to_fit();
    for (size_t i = 0; i < branches; ++i) {
        auto cur_size = std::min(block_size, n - i * part_size);
        blocks[i].assign(cur_size, 0);
        blocks[i].shrink_to_fit();
        out.Seek(start + i * part_size);
        ScanFromTape(out, blocks[i].data(), cur_size);
        heap.push(ElementFromArrays{.value = blocks[i][0], .array_ind = i});
    }
    buffer.Seek(0);
    while (!heap.empty()) {
        auto [value, part_ind] = heap.top();
        heap.pop();
        buffer.WriteAndMoveForward(value);
        ++ind_in_block[part_ind];
        if (ind_in_block[part_ind] == blocks[part_ind].size()) {
            size_t cur_part_size = 0;
            if (part_ind + 1 != branches) {
                cur_part_size = part_size;
            } else {
                cur_part_size = n - (branches - 1) * part_size;
            }
            if ((block_cnt_from_part[part_ind] + 1) * block_size >= cur_part_size) {
                continue;
            }
            ++block_cnt_from_part[part_ind];
            auto read_cnt =
                std::min(cur_part_size - block_cnt_from_part[part_ind] * block_size, block_size);
            blocks[part_ind].assign(read_cnt, 0);
            out.Seek(start + part_ind * part_size + block_cnt_from_part[part_ind] * block_size);
            ScanFromTape(out, blocks[part_ind].data(), read_cnt);
            ind_in_block[part_ind] = 0;
        }
        heap.push(ElementFromArrays{.value = blocks[part_ind][ind_in_block[part_ind]],
                                    .array_ind = part_ind});
    }
    out.Seek(start);
    buffer.Seek(0);
    for (size_t i = 0; i < n; ++i) {
        out.WriteAndMoveForward(buffer.ReadAndMoveForward());
    }
}
}  // namespace

void Sort(Tape& in, size_t n, Tape& out, Tape& buffer, size_t ram_ints_cnt,
          size_t scan_block_size) {
    in.Seek(0);
    out.Seek(0);
    auto branches = ram_ints_cnt / scan_block_size;
    if (branches < 2) {
        throw std::runtime_error("Sort failed, RAM is too small");
    }
    MergeSort(in, 0, n, out, buffer, branches, scan_block_size);
    in.Seek(0);
    out.Seek(0);
}
}  // namespace Ydr
