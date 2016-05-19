#include <cstdint>
#include <iostream>
#include <exception>
#include <array>
#include <bitset>
#include <vector>
#include <chrono>

int GetBitIndex(const std::bitset<9>& bits) {
    for (int i = 0; i < 9; ++i) {
        if (bits.test(i)) return i;
    }
    throw std::runtime_error("No bit was set!");
}

class Sudoku {
public:
    Sudoku() : known_values_(0) {
        for (int i = 0; i < 81; ++i) {
            values_[i] = 0;
            potential_[i].set();
            counts_[i] = 9;
        }
    }

    Sudoku(const Sudoku& other) = default;

    Sudoku(Sudoku&& other) = default;

    constexpr Sudoku& operator=(const Sudoku& rhs) = default;

    void SetValue(int index, int value) {
        const int x = index % 9;
        const int y = index / 9;
        SetValue(x, y, value);
    }

    void SetValue(int x, int y, int value) {
        if (value > 0 && !HasFailed()) {
            const int index = 9*y + x;
            if (values_[index] == 0) {
                known_values_++;
                values_[index] = static_cast<uint8_t>(value);
                potential_[index].reset();
                counts_[index] = 0;
                EliminatePeers(x, y, value);
                CheckPeers(x, y);
            } else if (values_[index] != value) {
                // Contradictory value to what we deduced already
                SetFailed();
            }
        }
    }

    const uint8_t& GetValue(int x, int y) const {
        return values_[9*y+x];
    }

    const int GetBestNext() const {
        int best_index = -1;
        int best_count = 10;
        for (int i = 0; i < 81; ++i) {
            const int count = counts_[i];
            if (count == 2) {
                return i; // we can't beat this
            } else if (count > 0 && count < best_count)  {
                best_count = count;
                best_index = i;
            }
        }
        return best_index;
    }

    const int GetCount(const int index) const {
        return counts_[index];
    }

    const std::bitset<9>& GetPotentialValues(int index) const {
        return potential_[index];
    }

    const bool IsFinished() const {
        return known_values_ == 81;
    }

    const bool HasFailed() const {
        return known_values_ == -1;
    }
private:
    void SetFailed() {
        known_values_ = -1;
    }

    void EliminatePeers(int x, int y, int value) {
        // row and col
        for (int i = 0; i < 9; ++i) {
            if (i != x) Eliminate(i, y, value);
            if (i != y) Eliminate(x, i, value);
            if (HasFailed()) return;
        }
        // zone
        const int x0 = x - (x % 3);
        const int y0 = y - (y % 3);
        for (int i = y0; i < y0+3; ++i) {
            for (int j = x0; j < x0+3; ++j) {
                if (i != y && j != x) Eliminate(j, i, value);
                if (HasFailed()) return;
            }
        }
    }

    void Eliminate(int x, int y, int value) {
        const int zvalue = value - 1;
        const int index = 9*y + x;
        auto& potential_values = potential_[index];
        if (potential_values.test(zvalue)) {
            potential_values.reset(zvalue);
            counts_[index]--;
            if (counts_[index] == 0) {
                SetFailed();
            }
        }
    }

    void CheckPeers(int x, int y) {
        if (HasFailed()) return;
        // row and col
        for (int i = 0; i < 9; ++i) {
            if (i != x) CheckCell(i, y);
            if (i != y) CheckCell(x, i);
        }
        // zone
        const int x0 = x - (x % 3);
        const int y0 = y - (y % 3);
        for (int i = y0; i < y0+3; ++i) {
            for (int j = x0; j < x0+3; ++j) {
                if (i != y && j != x) CheckCell(j, i);
            }
        }
    }

    void CheckCell(int x, int y) {
        const int index = 9*y + x;
        if (counts_[index] == 1) {
            int found_value = GetBitIndex(potential_[index]) + 1;
            SetValue(x, y, found_value);
        }
    }

    int known_values_;
    std::array<uint8_t,81> values_;
    std::array<std::bitset<9>,81> potential_;
    std::array<uint8_t,81> counts_;
};

bool IsValid(char c) {
    return c == '.' || (c >= '1' && c <= '9');
}

int GetNextValue() {
    char c;
    while (std::cin.get(c)) {
        if (c == '.') return 0;
        if (c >= '1' && c <= '9') return c - '0';
    }
    throw std::runtime_error("Reached end of input!");
}

Sudoku ParseStdin() {
    Sudoku s;
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            const int v = GetNextValue();
            s.SetValue(x, y, v);
        }
    }
    return s;
}

void PrintSudoku(const Sudoku& s) {
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            const int v = s.GetValue(x, y);
            if (v == 0) {
                std::cout << ". ";
            } else {
                std::cout << (char)('0' + s.GetValue(x, y)) << " ";
            }
        }
        std::cout << "\n";
    }
}

void BranchLast(std::vector<Sudoku>& remaining) {
    const auto index = remaining.size() - 1;
    const int best_pos = remaining[index].GetBestNext();
    const int count = remaining[index].GetCount(best_pos);
    const auto potential = remaining[index].GetPotentialValues(best_pos);

    int num = 0;
    for (int i = 0; i < 9; ++i) {
        if (potential.test(i)) {
            num++;
            if (num == count) {
                remaining[index].SetValue(best_pos, i+1);
            } else {
                remaining.emplace_back(remaining[index]);
                remaining.back().SetValue(best_pos, i+1);
                if (remaining.back().HasFailed()) {
                    remaining.pop_back();
                }
            }
        }
    }
}

Sudoku Solve(const Sudoku& sudoku) {
    std::vector<Sudoku> remaining;
    remaining.push_back(sudoku);

    while (!remaining.empty()) {
        const Sudoku& s{ remaining.back() };
        if (s.IsFinished()) {
            return s;
        } else if (!s.HasFailed()) {
            BranchLast(remaining);
        } else {
            remaining.pop_back();
        }
    }

    throw std::runtime_error("No solution!");
}

int main() {
    const auto s{ ParseStdin() };

    const auto start{ std::chrono::high_resolution_clock::now() };
    const auto solution{ Solve(s) };
    const auto end{ std::chrono::high_resolution_clock::now() };
    std::chrono::nanoseconds nanosecs{ end - start };

    PrintSudoku(solution);
    std::cout << "milliseconds: " << nanosecs.count() / 1000000.0 << "\n";
    double persec = 1000000000.0 / nanosecs.count();
    std::cout << "per second: " << persec << "\n";
    return 0;
}
