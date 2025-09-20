#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cassert>

struct Constraint {
    int first_row, first_col;
    int second_row, second_col;
    char symbol;
};

struct FutoshikiPuzzle {
    int size;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<int>> display_grid;
    std::vector<Constraint> constraints;
};

int generateRandomInt(int min_val, int max_val) {
    return (std::rand() % (max_val - min_val + 1)) + min_val;
}

bool isValidConstraint(FutoshikiPuzzle& puzzle, Constraint constraint) {
    if (constraint.first_row < 0 || constraint.second_row >= puzzle.size || 
        constraint.first_col < 0 || constraint.second_col >= puzzle.size)
        return false;
        
    if ((constraint.first_row != constraint.second_row) && (constraint.first_col != constraint.second_col))
        return false;
        
    if (((constraint.first_row == constraint.second_row) && (constraint.second_col != constraint.first_col + 1)) || 
        ((constraint.first_col == constraint.second_col) && (constraint.second_row != constraint.first_row + 1)))
        return false;

    for (const Constraint& existing : puzzle.constraints) {
        if ((existing.first_row == constraint.first_row) && (existing.second_row == constraint.second_row) && 
            (existing.first_col == constraint.first_col) && (existing.second_col == constraint.second_col))
            return false;
    }
    
    return true;
}

bool isValidPlacement(const FutoshikiPuzzle& puzzle, int row, int col, int num) {
    for (int i = 0; i < puzzle.size; ++i) {
        if (puzzle.grid[row][i] == num || puzzle.grid[i][col] == num)
            return false;
    }
    
    for (const Constraint& constraint : puzzle.constraints) {
        if (constraint.first_row == row && constraint.first_col == col) {
            int other_value = puzzle.grid[constraint.second_row][constraint.second_col];
            if (other_value != 0) {
                if (constraint.symbol == '>' && num <= other_value) return false;
                if (constraint.symbol == '<' && num >= other_value) return false;
                if (constraint.symbol == 'v' && num <= other_value) return false;
                if (constraint.symbol == '^' && num >= other_value) return false;
            }
        } else if (constraint.second_row == row && constraint.second_col == col) {
            int other_value = puzzle.grid[constraint.first_row][constraint.first_col];
            if (other_value != 0) {
                if (constraint.symbol == '>' && other_value <= num) return false;
                if (constraint.symbol == '<' && other_value >= num) return false;
                if (constraint.symbol == 'v' && other_value <= num) return false;
                if (constraint.symbol == '^' && other_value >= num) return false;
            }
        }
    }
    
    return true;
}

bool solvePuzzle(FutoshikiPuzzle& puzzle) {
    for (int row = 0; row < puzzle.size; ++row) {
        for (int col = 0; col < puzzle.size; ++col) {
            if (puzzle.grid[row][col] == 0) {
                for (int num = 1; num <= puzzle.size; ++num) {
                    if (isValidPlacement(puzzle, row, col, num)) {
                        puzzle.grid[row][col] = num;
                        if (solvePuzzle(puzzle)) {
                            return true;
                        }
                        puzzle.grid[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

bool hasUniqueSolution(FutoshikiPuzzle puzzle) {
    std::vector<std::vector<int>> original_grid = puzzle.grid;
    int solution_count = 0;
    
    if (solvePuzzle(puzzle)) {
        solution_count = 1;
        std::vector<std::vector<int>> first_solution = puzzle.grid;
        
        puzzle.grid = original_grid;
        
        for (int row = 0; row < puzzle.size && solution_count == 1; ++row) {
            for (int col = 0; col < puzzle.size && solution_count == 1; ++col) {
                if (original_grid[row][col] == 0) {
                    for (int num = puzzle.size; num >= 1; --num) {
                        if (isValidPlacement(puzzle, row, col, num)) {
                            puzzle.grid[row][col] = num;
                            if (solvePuzzle(puzzle)) {
                                if (puzzle.grid != first_solution) {
                                    solution_count = 2;
                                    break;
                                }
                            }
                            puzzle.grid = original_grid;
                        }
                    }
                }
            }
        }
    }
    
    return solution_count == 1;
}

FutoshikiPuzzle generatePuzzle(int puzzle_size) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    FutoshikiPuzzle puzzle;
    puzzle.size = puzzle_size;
    puzzle.grid.assign(puzzle_size, std::vector<int>(puzzle_size, 0));
    puzzle.display_grid.assign(puzzle_size, std::vector<int>(puzzle_size, 0));
    
    if (!solvePuzzle(puzzle)) {
        for (int i = 0; i < puzzle_size; ++i) {
            for (int j = 0; j < puzzle_size; ++j) {
                puzzle.grid[i][j] = ((i + j) % puzzle_size) + 1;
            }
        }
    }
    
    int constraint_count = generateRandomInt(puzzle_size, puzzle_size * 2);
    for (int i = 0; i < constraint_count; ++i) {
        Constraint new_constraint;
        new_constraint.first_row = generateRandomInt(0, puzzle_size - 1);
        new_constraint.first_col = generateRandomInt(0, puzzle_size - 1);
        
        if (generateRandomInt(0, 1) == 0 && new_constraint.first_col + 1 < puzzle_size) {
            new_constraint.second_row = new_constraint.first_row;
            new_constraint.second_col = new_constraint.first_col + 1;
        } else if (new_constraint.first_row + 1 < puzzle_size) {
            new_constraint.second_row = new_constraint.first_row + 1;
            new_constraint.second_col = new_constraint.first_col;
        } else {
            continue;
        }
        
        int first_val = puzzle.grid[new_constraint.first_row][new_constraint.first_col];
        int second_val = puzzle.grid[new_constraint.second_row][new_constraint.second_col];
        
        if (new_constraint.first_row == new_constraint.second_row) {
            new_constraint.symbol = (first_val > second_val) ? '>' : '<';
        } else {
            new_constraint.symbol = (first_val > second_val) ? 'v' : '^';
        }
        
        if (isValidConstraint(puzzle, new_constraint)) {
            puzzle.constraints.push_back(new_constraint);
        }
    }
    
    puzzle.display_grid = puzzle.grid;
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < puzzle_size; ++i) {
        for (int j = 0; j < puzzle_size; ++j) {
            positions.push_back({i, j});
        }
    }
    
    std::random_shuffle(positions.begin(), positions.end());
    
    for (const auto& pos : positions) {
        int row = pos.first, col = pos.second;
        int original_value = puzzle.display_grid[row][col];
        
        puzzle.display_grid[row][col] = 0;
        
        FutoshikiPuzzle test_puzzle = puzzle;
        test_puzzle.grid = puzzle.display_grid;
        
        if (!hasUniqueSolution(test_puzzle)) {
            puzzle.display_grid[row][col] = original_value;
        }
    }
    
    return puzzle;
}

void printPuzzle(const FutoshikiPuzzle& puzzle) {
    for (int i = 0; i < puzzle.size; ++i) {
        for (int j = 0; j < puzzle.size; ++j) {
            if (puzzle.display_grid[i][j] == 0) {
                std::cout << "_ ";
            } else {
                std::cout << puzzle.display_grid[i][j] << " ";
            }
            
            for (const Constraint& constraint : puzzle.constraints) {
                if (constraint.first_row == i && constraint.first_col == j && 
                    constraint.second_row == i && constraint.second_col == j + 1) {
                    std::cout << constraint.symbol << " ";
                    break;
                }
            }
        }
        std::cout << std::endl;
        
        for (int j = 0; j < puzzle.size; ++j) {
            for (const Constraint& constraint : puzzle.constraints) {
                if (constraint.first_row == i && constraint.first_col == j && 
                    constraint.second_row == i + 1 && constraint.second_col == j) {
                    std::cout << constraint.symbol << " ";
                    break;
                } else {
                    std::cout << "  ";
                    break;
                }
            }
        }
        if (i < puzzle.size - 1) std::cout << std::endl;
    }
}

int main() {
    FutoshikiPuzzle puzzle = generatePuzzle(4);
    
    std::cout << "Generated Futoshiki Puzzle:" << std::endl;
    printPuzzle(puzzle);
    
    return 0;
}
