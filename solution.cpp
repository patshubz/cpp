#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>
#include <algorithm>

struct Constraint {
    int Ax, Ay; // coordinates cell A
    int Bx, By; // coordinates cell B
    char symbol; // '>', '<', 'v', '^'
};

struct Futoshiki {
    int size;
    std::vector<std::vector<int>> grid;         // full solution grid
    std::vector<std::vector<int>> display_grid; // puzzle grid with empty cells
    std::vector<Constraint> constraints;
};

int getRandomInt(int minInt, int maxInt) {
    return (std::rand() % (maxInt - minInt + 1)) + minInt;
}

bool validate_constraint(Futoshiki& puzzle, Constraint temp) {
    if ((temp.Ax < 0) || (temp.Bx >= puzzle.size) || (temp.Ay < 0) || (temp.By >= puzzle.size))
        return false;
    else if ((temp.Ax != temp.Bx) && (temp.Ay != temp.By))
        return false;
    else if (((temp.Ax == temp.Bx) && (temp.By != temp.Ay + 1)) || ((temp.Ay == temp.By) && (temp.Bx != temp.Ax + 1)))
        return false;
    
    for (const Constraint& other : puzzle.constraints) {
        if ((other.Ax == temp.Ax) && (other.Bx == temp.Bx) && (other.Ay == temp.Ay) && (other.By == temp.By))
            return false;
    }
    return true;
}

void add_constraints(Futoshiki& puzzle) {
    int num_constraints = getRandomInt(puzzle.size, puzzle.size * puzzle.size / 2);
    int row, col, rowB, colB;
    char dir;
    Constraint temp;
    
    while (num_constraints > 0) {
        row = getRandomInt(0, puzzle.size - 1);
        col = getRandomInt(0, puzzle.size - 1);
        dir = (getRandomInt(0, 1) == 0) ? 'V' : 'H';
        
        if (dir == 'V') {
            rowB = row + 1;
            colB = col;
        } else {
            rowB = row;
            colB = col + 1;
        }
        
        temp = {row, col, rowB, colB, 'X'};
        if (!validate_constraint(puzzle, temp))
            continue;
            
        if ((puzzle.grid[row][col] < puzzle.grid[rowB][colB]) && (dir == 'V'))
            temp.symbol = '<';
        else if ((puzzle.grid[row][col] < puzzle.grid[rowB][colB]) && (dir == 'H'))
            temp.symbol = '^';
        else if ((puzzle.grid[row][col] > puzzle.grid[rowB][colB]) && (dir == 'V'))
            temp.symbol = '>';
        else
            temp.symbol = 'v';
            
        puzzle.constraints.push_back(temp);
        num_constraints--;
    }
}

bool is_valid(const Futoshiki& puzzle, int row, int col, int num) {
    // Check row and column constraints
    for (int i = 0; i < puzzle.size; ++i) {
        if (puzzle.grid[row][i] == num || puzzle.grid[i][col] == num)
            return false;
    }
    
    // Check inequality constraints
    for (const auto& constraint : puzzle.constraints) {
        if ((constraint.Ax == row && constraint.Ay == col) || (constraint.Bx == row && constraint.By == col)) {
            int other_row = (constraint.Ax == row && constraint.Ay == col) ? constraint.Bx : constraint.Ax;
            int other_col = (constraint.Ay == col && constraint.Ax == row) ? constraint.By : constraint.Ay;
            
            if (puzzle.grid[other_row][other_col] != 0) {
                if (constraint.Ax == row && constraint.Ay == col) {
                    // This cell is A in constraint A symbol B
                    if ((constraint.symbol == '>' && num <= puzzle.grid[other_row][other_col]) ||
                        (constraint.symbol == '<' && num >= puzzle.grid[other_row][other_col]) ||
                        (constraint.symbol == 'v' && num <= puzzle.grid[other_row][other_col]) ||
                        (constraint.symbol == '^' && num >= puzzle.grid[other_row][other_col]))
                        return false;
                } else {
                    // This cell is B in constraint A symbol B
                    if ((constraint.symbol == '>' && puzzle.grid[other_row][other_col] <= num) ||
                        (constraint.symbol == '<' && puzzle.grid[other_row][other_col] >= num) ||
                        (constraint.symbol == 'v' && puzzle.grid[other_row][other_col] <= num) ||
                        (constraint.symbol == '^' && puzzle.grid[other_row][other_col] >= num))
                        return false;
                }
            }
        }
    }
    return true;
}

bool solve(Futoshiki& puzzle) {
    for (int row = 0; row < puzzle.size; ++row) {
        for (int col = 0; col < puzzle.size; ++col) {
            if (puzzle.grid[row][col] == 0) {
                for (int num = 1; num <= puzzle.size; ++num) {
                    if (is_valid(puzzle, row, col, num)) {
                        puzzle.grid[row][col] = num;
                        if (solve(puzzle))
                            return true;
                        puzzle.grid[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

bool check_for_multiple_solutions(Futoshiki& puzzle, int testCellRow, int testCellCol) {
    Futoshiki testPuzzle = puzzle;
    testPuzzle.grid = testPuzzle.display_grid;
    int solution_count = 0;

    for (int i = 1; i <= puzzle.size; ++i) {
        if (is_valid(testPuzzle, testCellRow, testCellCol, i)) {
            testPuzzle.grid[testCellRow][testCellCol] = i;
            if (solve(testPuzzle)) {
                solution_count++;
                if (solution_count > 1) return true; // Multiple solutions detected
            }
            testPuzzle.grid[testCellRow][testCellCol] = 0;
        }
    }
    return false;
}

bool generate_puzzle(Futoshiki& puzzle) {
    std::srand(std::time(0));
    puzzle.grid.assign(puzzle.size, std::vector<int>(puzzle.size, 0));

    if (!solve(puzzle)) return false;
    add_constraints(puzzle);

    puzzle.display_grid = puzzle.grid;
    int cells_to_remove = puzzle.size * puzzle.size * 3 / 4;
    int removed_cells = 0;
    std::vector<int> cannotRemove;
    int attempts = 0;
    const int MAX_ATTEMPTS = puzzle.size * puzzle.size * 2;

    while (cells_to_remove > 0 && attempts < MAX_ATTEMPTS) {
        attempts++;
        int row = getRandomInt(0, puzzle.size - 1);
        int col = getRandomInt(0, puzzle.size - 1);

        if (puzzle.display_grid[row][col] == 0) continue;

        int cellIndex = row * puzzle.size + col;

        if (std::find(cannotRemove.begin(), cannotRemove.end(), cellIndex) != cannotRemove.end())
            continue;

        int temp = puzzle.display_grid[row][col];
        puzzle.display_grid[row][col] = 0;

        if (check_for_multiple_solutions(puzzle, row, col)) {
            puzzle.display_grid[row][col] = temp;
            cannotRemove.push_back(cellIndex);
        } else {
            cells_to_remove--;
            removed_cells++;
        }
    }
    return true;
}

void bubbleSort(std::vector<Constraint> &vectorObject, std::string sortBy) {
    for (size_t i = 1; i < vectorObject.size(); ++i) {
        for (size_t j = 0; j < vectorObject.size() - i; ++j) {
            if (sortBy == "Ay") {
                if (vectorObject[j].Ay > vectorObject[j+1].Ay) {
                    std::swap(vectorObject[j], vectorObject[j+1]);
                }
            } else if (sortBy == "By") {
                if ((vectorObject[j].By > vectorObject[j+1].By) && (vectorObject[j].Ay == vectorObject[j+1].Ay)) {
                    std::swap(vectorObject[j], vectorObject[j + 1]);
                }
            } else {
                if ((vectorObject[j].Ax > vectorObject[j+1].Ax) && 
                    (vectorObject[j].By == vectorObject[j+1].By) && 
                    (vectorObject[j].Ay == vectorObject[j+1].Ay)) {
                    std::swap(vectorObject[j], vectorObject[j + 1]);
                }
            }
        }
    }
}

void sort_constraints(Futoshiki& puzzle) {
    if (puzzle.constraints.empty()) return;
    bubbleSort(puzzle.constraints, "Ay");
    bubbleSort(puzzle.constraints, "By");
    bubbleSort(puzzle.constraints, "Ax");
}

void print_solution(Futoshiki& puzzle) {
    sort_constraints(puzzle);
    int k = 0;
    
    for (int i = 0; i < puzzle.size; ++i) {
        for (int j = 0; j < puzzle.size; ++j) {
            if (puzzle.grid[i][j] != 0)
                std::cout << "[" << puzzle.grid[i][j] << "]";
            else
                std::cout << "[ ]";

            // Check for horizontal constraint
            bool found_constraint = false;
            for (size_t c = 0; c < puzzle.constraints.size(); ++c) {
                if (puzzle.constraints[c].Ax == j && puzzle.constraints[c].Ay == i && 
                    puzzle.constraints[c].Bx == j + 1 && puzzle.constraints[c].By == i) {
                    std::cout << puzzle.constraints[c].symbol;
                    found_constraint = true;
                    break;
                }
            }
            if (!found_constraint) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
        
        // Print vertical constraints
        for (int j = 0; j < puzzle.size; ++j) {
            std::cout << " ";
            bool found_constraint = false;
            for (size_t c = 0; c < puzzle.constraints.size(); ++c) {
                if (puzzle.constraints[c].Ax == j && puzzle.constraints[c].Ay == i && 
                    puzzle.constraints[c].Bx == j && puzzle.constraints[c].By == i + 1) {
                    std::cout << puzzle.constraints[c].symbol;
                    found_constraint = true;
                    break;
                }
            }
            if (!found_constraint) {
                std::cout << " ";
            }
            std::cout << "  ";
        }
        std::cout << std::endl;
    }
}

void print_puzzle(Futoshiki& puzzle) {
    sort_constraints(puzzle);
    
    for (int i = 0; i < puzzle.size; ++i) {
        for (int j = 0; j < puzzle.size; ++j) {
            if (puzzle.display_grid[i][j] != 0)
                std::cout << "[" << puzzle.display_grid[i][j] << "]";
            else
                std::cout << "[ ]";

            // Check for horizontal constraint
            bool found_constraint = false;
            for (size_t c = 0; c < puzzle.constraints.size(); ++c) {
                if (puzzle.constraints[c].Ax == j && puzzle.constraints[c].Ay == i && 
                    puzzle.constraints[c].Bx == j + 1 && puzzle.constraints[c].By == i) {
                    std::cout << puzzle.constraints[c].symbol;
                    found_constraint = true;
                    break;
                }
            }
            if (!found_constraint) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
        
        // Print vertical constraints
        for (int j = 0; j < puzzle.size; ++j) {
            std::cout << " ";
            bool found_constraint = false;
            for (size_t c = 0; c < puzzle.constraints.size(); ++c) {
                if (puzzle.constraints[c].Ax == j && puzzle.constraints[c].Ay == i && 
                    puzzle.constraints[c].Bx == j && puzzle.constraints[c].By == i + 1) {
                    std::cout << puzzle.constraints[c].symbol;
                    found_constraint = true;
                    break;
                }
            }
            if (!found_constraint) {
                std::cout << " ";
            }
            std::cout << "  ";
        }
        std::cout << std::endl;
    }
}

int main() {
    int size;
    std::cout << "Enter the size of the grid: ";
    std::cin >> size;

    if (size < 3 || size > 9) {
        std::cout << "Size must be between 3 and 9" << std::endl;
        return 1;
    }

    Futoshiki puzzle;
    puzzle.size = size;

    std::cout << "Generating unique solution puzzle..." << std::endl;
    if (generate_puzzle(puzzle)) {
        std::cout << "Puzzle generated successfully!" << std::endl;
        std::cout << "\nPuzzle to solve:" << std::endl;
        print_puzzle(puzzle);
        
        std::cout << "\nSolution:" << std::endl;
        print_solution(puzzle);
    } else {
        std::cout << "Failed to generate puzzle." << std::endl;
    }
    return 0;
}
