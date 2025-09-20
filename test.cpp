#include <cassert>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>
#include <algorithm>

// Copy essential structures and functions from solution.cpp here
struct Constraint {
    int Ax, Ay;
    int Bx, By;
    char symbol;
};

struct Futoshiki {
    int size;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<int>> display_grid;
    std::vector<Constraint> constraints;
};

// Only include the functions we need to test
int getRandomInt(int minInt, int maxInt) {
    return (std::rand() % (maxInt - minInt + 1)) + minInt;
}

bool validate_constraint(Futoshiki& puzzle, Constraint temp) {
    if ((temp.Ax < 0) || (temp.Bx >= puzzle.size) || (temp.Ay < 0) || (temp.By >= puzzle.size))
        return false;
    for (const Constraint& other : puzzle.constraints) {
        if ((other.Ax == temp.Ax) && (other.Bx == temp.Bx) && (other.Ay == temp.Ay) && (other.By == temp.By))
            return false;
    }
    return true;
}

bool is_valid(const Futoshiki& puzzle, int row, int col, int num) {
    for (int i = 0; i < puzzle.size; ++i) {
        if (puzzle.grid[row][i] == num || puzzle.grid[i][col] == num)
            return false;
    }
    return true;
}

// Test functions
void test_basic_functionality() {
    Futoshiki puzzle;
    puzzle.size = 3;
    puzzle.grid = std::vector<std::vector<int>>(3, std::vector<int>(3, 0));
    puzzle.constraints.clear();

    // Test constraint validation
    Constraint valid = {0, 0, 0, 1, '>'};
    assert(validate_constraint(puzzle, valid));
    
    // Test is_valid
    puzzle.grid[0][0] = 1;
    assert(!is_valid(puzzle, 0, 1, 1)); // Same row
    assert(!is_valid(puzzle, 1, 0, 1)); // Same column
    assert(is_valid(puzzle, 1, 1, 2));  // Valid placement
    
    std::cout << "✓ Basic functionality test passed" << std::endl;
}

void test_constraint_validation() {
    Futoshiki puzzle;
    puzzle.size = 3;
    puzzle.constraints.clear();

    Constraint valid = {0, 0, 0, 1, '>'};
    assert(validate_constraint(puzzle, valid));

    Constraint invalid = {0, 0, 0, 3, '>'}; // Out of bounds
    assert(!validate_constraint(puzzle, invalid));
    
    std::cout << "✓ Constraint validation test passed" << std::endl;
}

void test_grid_validity() {
    Futoshiki puzzle;
    puzzle.size = 4;
    puzzle.grid = std::vector<std::vector<int>>(4, std::vector<int>(4, 0));
    
    // Place numbers to test validity
    puzzle.grid[0][0] = 1;
    puzzle.grid[0][1] = 2;
    puzzle.grid[1][0] = 3;
    
    assert(!is_valid(puzzle, 0, 2, 1)); // Duplicate in row
    assert(!is_valid(puzzle, 2, 0, 1)); // Duplicate in column
    assert(is_valid(puzzle, 1, 1, 4));  // Valid placement
    
    std::cout << "✓ Grid validity test passed" << std::endl;
}

int main() {
    std::cout << "==================== Futoshiki Puzzle Tests ====================" << std::endl;
    
    test_basic_functionality();
    test_constraint_validation();
    test_grid_validity();
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
}
