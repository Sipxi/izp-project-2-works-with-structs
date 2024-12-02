#!/bin/bash
# IZP 2024/25 proj2 simple tests

RED= GREEN= RESET=
if test -t 1; then
    RED="\033[31m"
    GREEN="\033[32m"
    RESET="\033[0m"
fi

passed() {
    printf "${GREEN}    Passed${RESET}\n"
}

failed() {
    printf "${RED}    Failed${RESET} %s\n" "$*"
}

# $1 test name
# $2 expected output
# $3... cmd line
run_test() {
    testname="$1"
    expected="$2"
    shift 2
    echo "$testname: $*"
    out=$("$@")
    if [ "x$out" = "x$expected" ]; then
        passed
    else
        failed "Expected $expected, got: $out"
    fi
}

## Compile the program
echo "----- Required functionality --------------------"
echo "Compilation: cc -std=c11 -Wall -Wextra figsearch.c -o figsearch"
if cc -std=c11 -Wall -Wextra figsearch.c -o figsearch -lm; then
    passed
else
    failed
    exit 1
fi

# Create the input files
cat > obrazek.txt <<EOL
4 5
0 0 1 1 1
0 0 1 0 1
1 0 1 1 1
1 1 1 1 1
EOL

cat > obrazek-invalid.txt <<EOL
4 4
0 0 1 1
0 4 1 0
1 1 1 1
EOL

# Test 1: Valid file
run_test "Test 1" "Valid" ./figsearch test obrazek.txt

# Test 2: Invalid file
echo "Example 2: ./figsearch test obrazek-invalid.txt"
if ./figsearch test obrazek-invalid.txt 2>&1 | grep -q "Invalid"; then
    passed
else
    failed
fi

# Test 3... hline,vline,square
run_test "Test 3" "3 0 3 4" ./figsearch hline obrazek.txt
run_test "Test 4" "0 2 3 2" ./figsearch vline obrazek.txt
run_test "Test 5" "0 2 2 4" ./figsearch square obrazek.txt
