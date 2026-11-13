#!/bin/bash
# -------------------------------------------------
# testingScript.sh
# Build and run all tests for Program 6:
# Musical Chairs with Threads.
# By Ryan Godburn and Emily King
# -------------------------------------------------

RESULTS="test_results.txt"

# Start fresh.
echo "Building musicalChairs..."
make clean
make
if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi
echo "Build succeeded."
echo ""

rm -f "$RESULTS"

# -------------------------------------------------
# Helper: run one test with a timeout so a
# deadlock does not hang the script forever.
# Parameters:
#   $1 - test label
#   $2 - timeout in seconds
#   $3 ... - command and arguments
# -------------------------------------------------
run_test() {
    local label="$1"
    local limit="$2"
    shift 2

    echo "=== $label ===" >> "$RESULTS"
    echo "Command: $*"    >> "$RESULTS"
    echo ""               >> "$RESULTS"

    timeout "$limit" "$@" >> "$RESULTS" 2>&1
    local status=$?

    if [ $status -eq 124 ]; then
        echo "*** TIMEOUT after ${limit}s" \
            " (possible deadlock) ***" \
            >> "$RESULTS"
    elif [ $status -ne 0 ]; then
        echo "*** Exit status: $status ***" \
            >> "$RESULTS"
    fi

    echo "" >> "$RESULTS"
    echo "-------------------------------" \
        >> "$RESULTS"
    echo "" >> "$RESULTS"
}

# -------------------------------------------------
# Test 1: No arguments (should print usage).
# -------------------------------------------------
run_test "Test 1: No arguments (usage)" \
    5 ./musicalChairs

# -------------------------------------------------
# Test 2: Invalid argument (0 kids).
# -------------------------------------------------
run_test "Test 2: 0 kids (error)" \
    5 ./musicalChairs 0

# -------------------------------------------------
# Test 3: Invalid argument (1 kid).
# -------------------------------------------------
run_test "Test 3: 1 kid (error)" \
    5 ./musicalChairs 1

# -------------------------------------------------
# Test 4: Smallest valid game (2 kids).
# One round, one winner.
# -------------------------------------------------
run_test "Test 4: 2 kids (minimum game)" \
    15 ./musicalChairs 2

# -------------------------------------------------
# Test 5: Three kids (two rounds).
# -------------------------------------------------
run_test "Test 5: 3 kids" \
    20 ./musicalChairs 3

# -------------------------------------------------
# Test 6: Five kids (medium game).
# -------------------------------------------------
run_test "Test 6: 5 kids" \
    30 ./musicalChairs 5

# -------------------------------------------------
# Test 7: Seven kids (larger game).
# Verifies many rounds of thread coordination.
# -------------------------------------------------
run_test "Test 7: 7 kids" \
    45 ./musicalChairs 7

# -------------------------------------------------
# Test 8: Ten kids (stress test).
# More threads competing for chairs.
# -------------------------------------------------
run_test "Test 8: 10 kids (stress)" \
    60 ./musicalChairs 10

# -------------------------------------------------
# Post-test verification.
# Scan the results for key markers that prove
# the program ran correctly.
# -------------------------------------------------
echo "" >> "$RESULTS"
echo "===============================" \
    >> "$RESULTS"
echo "  POST-TEST VERIFICATION" \
    >> "$RESULTS"
echo "===============================" \
    >> "$RESULTS"
echo "" >> "$RESULTS"

# Check that each valid test announced a winner.
for nkids in 2 3 5 7 10; do
    wins=$(grep -c "WINNER" "$RESULTS")
    elims=$(grep -c "is OUT" "$RESULTS")
    echo "Total winners announced: $wins" \
        >> "$RESULTS"
    echo "Total eliminations: $elims" \
        >> "$RESULTS"
    break
done

# Check no deadlock timeouts occurred.
timeouts=$(grep -c "TIMEOUT" "$RESULTS")
echo "Deadlock timeouts: $timeouts" \
    >> "$RESULTS"

# Check that threads were joined.
joins=$(grep -c "joined Kid" "$RESULTS")
echo "Total thread joins: $joins" \
    >> "$RESULTS"

# Check that the model was destroyed each time.
destroys=$(grep -c "Model: destroyed" "$RESULTS")
echo "Model destructions: $destroys" \
    >> "$RESULTS"

echo "" >> "$RESULTS"
if [ "$timeouts" -eq 0 ]; then
    echo "All tests passed without deadlock." \
        >> "$RESULTS"
else
    echo "WARNING: $timeouts test(s) timed out." \
        >> "$RESULTS"
fi

# -------------------------------------------------
# Done. Show the results.
# -------------------------------------------------
echo ""
echo "All tests complete."
echo "Results saved to $RESULTS"
echo ""
cat "$RESULTS"
