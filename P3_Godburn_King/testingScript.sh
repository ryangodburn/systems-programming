#!/bin/bash
# -------------------------------------------------
# testingScript.sh
# Build, set up test directory, and run all tests
# for Program 3: Sweeper.
# By Ryan Godburn and Emily King
# -------------------------------------------------

RESULTS="test_results.txt"

# Start fresh.
echo "Building sweeper..."
make clean
make
if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi
echo "Build succeeded."
echo ""

# Remove old test artifacts.
rm -rf testdir
rm -f "$RESULTS"

# -------------------------------------------------
# Create the test directory with varied files.
# -------------------------------------------------
echo "Creating test directory..."
mkdir -p testdir/subdir

echo "Hello" > testdir/file1.txt
echo "This is a longer test file" \
     > testdir/file2.txt
echo "Short" > testdir/file3.txt
dd if=/dev/urandom of=testdir/bigfile.bin \
    bs=1024 count=5 2>/dev/null
echo "Sub file" > testdir/subdir/file4.txt

# Add hard and soft links.
ln testdir/file1.txt testdir/hardlink1.txt
ln -s file2.txt testdir/softlink1.txt

echo "Test directory created."
echo ""

# -------------------------------------------------
# Capture ls -l at the top of the results file.
# -------------------------------------------------
echo "=== ls -la of testdir ===" \
    > "$RESULTS"
ls -la testdir/ >> "$RESULTS"
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 1: Default run (no switches).
# -------------------------------------------------
echo "=== Test 1: Default run ===" \
    >> "$RESULTS"
./sweeper -d testdir >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 2: Verbose mode.
# -------------------------------------------------
echo "=== Test 2: Verbose ===" \
    >> "$RESULTS"
./sweeper -d testdir -v >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 3: Help flag.
# -------------------------------------------------
echo "=== Test 3: Help flag ===" \
    >> "$RESULTS"
./sweeper -h >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 4: Output to a file.
# Note: Params does chdir, so out.txt lands
# inside testdir.
# -------------------------------------------------
echo "=== Test 4: Output file ===" \
    >> "$RESULTS"
./sweeper -d testdir -v -o out.txt \
    >> "$RESULTS" 2>&1
echo "(Sweeper output written to out.txt):" \
    >> "$RESULTS"
cat testdir/out.txt >> "$RESULTS" 2>/dev/null
rm -f testdir/out.txt
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 5: --size filter (only large files).
# -------------------------------------------------
echo "=== Test 5: --size 100 ===" \
    >> "$RESULTS"
./sweeper -d testdir -v --size 100 \
    >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 6: --ignore-case flag.
# -------------------------------------------------
echo "=== Test 6: --ignore-case ===" \
    >> "$RESULTS"
./sweeper -d testdir -v --ignore-case \
    >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 7: Current directory (no -d).
# -------------------------------------------------
echo "=== Test 7: Current directory ===" \
    >> "$RESULTS"
./sweeper >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Done. Show the results.
# -------------------------------------------------
echo "All tests complete."
echo "Results saved to $RESULTS"
echo ""
cat "$RESULTS"
