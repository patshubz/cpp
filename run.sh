#!/bin/bash

# Check if files exist
if [ ! -f "solution.cpp" ]; then
    echo "Error: Required files (solution.cpp) not found"
    exit 1
fi

if [ ! -f "test.cpp" ]; then
    echo "Error: Required files (test.cpp) not found"
    exit 1
fi

echo "Futoshiki Puzzle Generator Tests"

# Compile and run (test.cpp should include solution functions)
g++ -std=c++17 -O2 test.cpp -o test_futoshiki
./test_futoshiki

# Capture return code
return_code=$?

# Cleanup
rm -f test_futoshiki

# Print return code
echo $return_code

# Exit with the return code
exit $return_code
