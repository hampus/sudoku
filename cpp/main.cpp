#include <cstdint>
#include <iostream>
#include <exception>
#include <array>
#include <bitset>
#include <vector>

int GetBitIndex(const std::bitset<9> bits) {
    for (int i = 0; i < 9; ++i) {
        if (bits.test(i)) return i;
    }
    throw std::runtime_error("No bit was set!");
}

class Sudoku {
public:
    Sudoku() : known_values_(0) {
        for (int i = 0; i < 81; ++i) {
            values_.at(i) = 0;
            potential_.at(i).set();
        }
    }

    Sudoku(const Sudoku& other) = default;

    Sudoku(Sudoku&& other) = default;

    void SetValue(int index, int value) {
        int x = index % 9;
        int y = index / 9;
        SetValue(x, y, value);
    }

    void SetValue(int x, int y, int value) {
        if (value > 0 && !HasFailed()) {
            const int index = 9*y + x;
            if (values_.at(index) == 0) {
                known_values_++;
                values_.at(index) = static_cast<uint8_t>(value);
                potential_.at(index).reset();
                eliminate_peers(x, y, value);
                check_peers(x, y);
            } else if (values_.at(index) == value) {
                //std::cout << "Already deduced " << x << ":" << y << " = " << value << "\n";
            } else {
                //std::cout << "Contradictory value. " << value << " != ";
                //std::cout << static_cast<int>(values_.at(index)) << "\n";
                known_values_ = -1;
            }
        }
    }

    const uint8_t& GetValue(int x, int y) const {
        return values_.at(9*y+x);
    }

    const int GetBestNext() const {
        int best_index = -1;
        int best_count = 10;
        for (int i = 0; i < 81; ++i) {
            int count = potential_.at(i).count();
            if (count == 2) {
                return i; // we can't beat this
            } else if (count > 0 && count < best_count)  {
                best_count = count;
                best_index = i;
            } else {
                //std::cout << "count = " << count << "\n";
            }
        }
        return best_index;
    }

    const std::bitset<9>& GetPotentialValues(int index) const {
        return potential_.at(index);
    }

    const bool IsFinished() const {
        return known_values_ == 81;
    }

    const bool HasFailed() const {
        return known_values_ == -1;
    }
private:
    void eliminate_peers(int x, int y, int value) {
        // row and col
        for (int i = 0; i < 9; ++i) {
            if (i != x) eliminate(i, y, value);
            if (i != y) eliminate(x, i, value);
            if (HasFailed()) return;
        }
        // zone
        const int x0 = x - (x % 3);
        const int y0 = y - (y % 3);
        for (int i = y0; i < y0+3; ++i) {
            for (int j = x0; j < x0+3; ++j) {
                if (i != y && j != x) eliminate(j, i, value);
                if (HasFailed()) return;
            }
        }
    }

    void eliminate(int x, int y, int value) {
        if (HasFailed()) return;
        const int zvalue = value - 1;
        std::bitset<9>& potential_values = potential_.at(y*9 + x);
        if (potential_values.test(zvalue)) {
            potential_values.reset(zvalue);
            if (potential_values.none()) {
                known_values_ = -1; // this sudoku failed
                //std::cout << "Reached impossible state\n";
            }
        }
    }

    void check_peers(int x, int y) {
        if (HasFailed()) return;
        // row and col
        for (int i = 0; i < 9; ++i) {
            if (i != x) check_cell(i, y);
            if (i != y) check_cell(x, i);
        }
        // zone
        const int x0 = x - (x % 3);
        const int y0 = y - (y % 3);
        for (int i = y0; i < y0+3; ++i) {
            for (int j = x0; j < x0+3; ++j) {
                if (i != y && j != x) check_cell(j, i);
            }
        }
    }

    void check_cell(int x, int y) {
        std::bitset<9>& potential_values = potential_.at(y*9 + x);
        if (potential_values.count() == 1) {
            int found_value = GetBitIndex(potential_values) + 1;
            //std::cout << "Found value " << x << ":" << y << " = " << found_value << "\n";
            SetValue(x, y, found_value);
        }
    }

    int known_values_;
    std::array<uint8_t,81> values_;
    std::array<std::bitset<9>,81> potential_;
};

bool isValid(char c) {
    return c == '.' || (c >= '1' && c <= '9');
}

int getNextValue() {
    char c;
    while (std::cin.get(c)) {
        if (c == '.') return 0;
        if (c >= '1' && c <= '9') return c - '0';
    }
    throw std::runtime_error("Reached end of input!");
}

Sudoku parse_stdin() {
    Sudoku s;
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            const int v = getNextValue();
            s.SetValue(x, y, v);
        }
    }
    return s;
}

void print_sudoku(const Sudoku& s) {
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

void solve(const Sudoku& sudoku) {
    std::vector<Sudoku> remaining;
    remaining.push_back(sudoku);
    while (!remaining.empty()) {
        Sudoku s{ remaining.back() };
        remaining.pop_back();

        //std::cout << "\nExamining:\n";
        //print_sudoku(s);

        if (s.IsFinished()) {
            //std::cout << "Success!\n";
            print_sudoku(s);
            return;
        } else if (!s.HasFailed()) {
            int best_pos = s.GetBestNext();
            const std::bitset<9>& potential = s.GetPotentialValues(best_pos);
            //std::cout << "\nCreating " << potential.count() << " branches\n";
            for (int i = 0; i < 9; ++i) {
                if (potential.test(i)) {
                    Sudoku copy(s);
                    copy.SetValue(best_pos, i+1);
                    remaining.push_back(copy);
                }
            }
        } else {
            //std::cout << "Impossible\n";
        }
    }
}

int main() {
    solve(parse_stdin());
    return 0;
}
