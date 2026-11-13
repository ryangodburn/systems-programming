#!/bin/bash
# -------------------------------------------------
# testingScript.sh
# Build and run all tests for Program 7:
# Musical Chairs with Sockets.
# By Ryan Godburn and Emily King
# -------------------------------------------------

RESULTS="test_results.txt"
HOST="localhost"

# Start fresh.
echo "Building mom and kid..."
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
# Helper: run one game with the given kid names.
# Starts mom, then starts each kid, waits for
# everything to finish, and collects output.
# Parameters:
#   $1 - test label
#   $2 - timeout in seconds
#   $3 - number of kids
#   remaining args - kid names
# -------------------------------------------------
run_test() {
    local label="$1"
    local limit="$2"
    local nKids="$3"
    shift 3

    local names=("$@")
    local momLog="/tmp/p7_mom_$$.log"
    local kidLogs=()

    echo "=== $label ===" >> "$RESULTS"
    echo "Kids: ${names[*]}" >> "$RESULTS"
    echo "" >> "$RESULTS"

    # Start mom in background.
    timeout "$limit" ./mom "$nKids" \
        > "$momLog" 2>&1 &
    local momPid=$!

    # Give mom a moment to start listening.
    sleep 0.5

    # Start each kid in background.
    for name in "${names[@]}"; do
        local klog="/tmp/p7_${name}_$$.log"
        kidLogs+=("$klog")
        timeout "$limit" ./kid "$HOST" "$name" \
            > "$klog" 2>&1 &
    done

    # Wait for mom to finish (kids exit on their
    # own when they receive QUIT or PRIZE).
    wait $momPid
    local momStatus=$?

    # Wait briefly for any remaining kids.
    sleep 1
    wait 2>/dev/null

    # ---- Collect output into results ----
    echo "--- Mom's Output ---" >> "$RESULTS"
    if [ $momStatus -eq 124 ]; then
        echo "*** TIMEOUT after ${limit}s ***" \
            >> "$RESULTS"
    fi
    cat "$momLog" >> "$RESULTS"
    echo "" >> "$RESULTS"

    for idx in "${!names[@]}"; do
        echo "--- ${names[$idx]}'s Output ---" \
            >> "$RESULTS"
        cat "${kidLogs[$idx]}" >> "$RESULTS"
        echo "" >> "$RESULTS"
    done

    echo "-------------------------------" \
        >> "$RESULTS"
    echo "" >> "$RESULTS"

    # Clean up temp files.
    rm -f "$momLog"
    for klog in "${kidLogs[@]}"; do
        rm -f "$klog"
    done
}

# -------------------------------------------------
# Helper: run a many-kid game and capture only
# Mom's output (as the assignment requires).
# Parameters:
#   $1 - test label
#   $2 - timeout in seconds
#   $3 - number of kids to use from kids.txt
# -------------------------------------------------
run_many_test() {
    local label="$1"
    local limit="$2"
    local nKids="$3"

    # Read names from kids.txt.
    local names=()
    while IFS= read -r name && \
          [ ${#names[@]} -lt "$nKids" ]; do
        names+=("$name")
    done < kids.txt

    local momLog="/tmp/p7_mom_many_$$.log"

    echo "=== $label ===" >> "$RESULTS"
    echo "Kids: ${names[*]}" >> "$RESULTS"
    echo "" >> "$RESULTS"

    # Start mom.
    timeout "$limit" ./mom "$nKids" \
        > "$momLog" 2>&1 &
    local momPid=$!

    sleep 0.5

    # Start kids.
    for name in "${names[@]}"; do
        timeout "$limit" ./kid "$HOST" "$name" \
            > /dev/null 2>&1 &
    done

    wait $momPid
    local momStatus=$?

    sleep 1
    wait 2>/dev/null

    echo "--- Mom's Output ---" >> "$RESULTS"
    if [ $momStatus -eq 124 ]; then
        echo "*** TIMEOUT after ${limit}s ***" \
            >> "$RESULTS"
    fi
    cat "$momLog" >> "$RESULTS"
    echo "" >> "$RESULTS"

    echo "-------------------------------" \
        >> "$RESULTS"
    echo "" >> "$RESULTS"

    rm -f "$momLog"
}

# -------------------------------------------------
# Test 1: No arguments to mom (should print usage).
# -------------------------------------------------
echo "=== Test 1: No arguments (usage) ===" \
    >> "$RESULTS"
echo "Command: ./mom" >> "$RESULTS"
echo "" >> "$RESULTS"
timeout 5 ./mom >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"
echo "-------------------------------" \
    >> "$RESULTS"
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 2: Invalid argument (0 kids).
# -------------------------------------------------
echo "=== Test 2: 0 kids (error) ===" \
    >> "$RESULTS"
echo "Command: ./mom 0" >> "$RESULTS"
echo "" >> "$RESULTS"
timeout 5 ./mom 0 >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"
echo "-------------------------------" \
    >> "$RESULTS"
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 3: Invalid argument (1 kid).
# -------------------------------------------------
echo "=== Test 3: 1 kid (error) ===" \
    >> "$RESULTS"
echo "Command: ./mom 1" >> "$RESULTS"
echo "" >> "$RESULTS"
timeout 5 ./mom 1 >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"
echo "-------------------------------" \
    >> "$RESULTS"
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 4: Three kids - full output from all.
# Assignment requires Mom + each Kid combined
# in one file with comments.
# -------------------------------------------------
run_test "Test 4: 3 kids (full output)" \
    30 3 alice bob charlie

# -------------------------------------------------
# Test 5: Many kids - Mom's output only.
# Assignment requires a second run with many
# children; turn in Mom's screen output.
# -------------------------------------------------
run_many_test "Test 5: 10 kids (Mom only)" \
    60 10

# -------------------------------------------------
# Post-test verification.
# -------------------------------------------------
echo "" >> "$RESULTS"
echo "===============================" \
    >> "$RESULTS"
echo "  POST-TEST VERIFICATION" \
    >> "$RESULTS"
echo "===============================" \
    >> "$RESULTS"
echo "" >> "$RESULTS"

# Count protocol messages in output.
hellos=$(grep -c "says HELLO" "$RESULTS")
getups=$(grep -c "GETUP"      "$RESULTS")
sits=$(grep -c   "SIT$"       "$RESULTS")
wants=$(grep -c  "WANT"       "$RESULTS")
acks=$(grep -c   ": ACK"      "$RESULTS")
nacks=$(grep -c  "NACK"       "$RESULTS")
quits=$(grep -c  ": QUIT"     "$RESULTS")
prizes=$(grep -c "PRIZE"      "$RESULTS")

echo "HELLO messages:  $hellos" >> "$RESULTS"
echo "GETUP messages:  $getups" >> "$RESULTS"
echo "SIT messages:    $sits"   >> "$RESULTS"
echo "WANT messages:   $wants"  >> "$RESULTS"
echo "ACK messages:    $acks"   >> "$RESULTS"
echo "NACK messages:   $nacks"  >> "$RESULTS"
echo "QUIT messages:   $quits"  >> "$RESULTS"
echo "PRIZE messages:  $prizes" >> "$RESULTS"
echo "" >> "$RESULTS"

# Check for timeouts (possible deadlock).
timeouts=$(grep -c "TIMEOUT" "$RESULTS")
echo "Deadlock timeouts: $timeouts" >> "$RESULTS"

# Check winners announced.
winners=$(grep -c "I WON" "$RESULTS")
echo "Winners announced: $winners" >> "$RESULTS"

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
