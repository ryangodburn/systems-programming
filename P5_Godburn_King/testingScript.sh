#!/bin/bash
# -------------------------------------------------
# testingScript.sh
# Build, set up test directory, and run all tests
# for Program 5: Eliminating Duplicates.
# Tests duplicate detection via hard links and
# content-identical files on a test directory
# and on the user's home directory.
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
# Create the test directory tree.
# Two levels of subdirectories with files
# designed to exercise duplicate detection:
# hard links, copied files, same-length but
# different content, and unique files.
# -------------------------------------------------
echo "Creating test directory..."

# Level 0 (root of test tree)
mkdir -p testdir

# Level 1: two subdirectories
mkdir -p testdir/alpha
mkdir -p testdir/beta

# Level 2: subdirectories inside level 1
mkdir -p testdir/alpha/deep1
mkdir -p testdir/beta/deep2

# -------------------------------------------------
# Populate with files.
# Use printf to control exact byte counts so
# we can create same-length-but-different files.
# -------------------------------------------------

# Root level files
echo "Hello from root" \
    > testdir/root1.txt
echo "Another root file here" \
    > testdir/root2.txt
dd if=/dev/urandom \
    of=testdir/bigfile.bin \
    bs=1024 count=5 2>/dev/null

# Level 1 files
printf "AAAAAAAAAA" \
    > testdir/alpha/ten_a.txt
printf "BBBBBBBBBB" \
    > testdir/beta/ten_b.txt
echo "Alpha file one" \
    > testdir/alpha/notes.txt
echo "Beta file one" \
    > testdir/beta/memo.txt

# Level 2 files
printf "CCCCCCCCCC" \
    > testdir/alpha/deep1/ten_c.txt
echo "Deep inside alpha" \
    > testdir/alpha/deep1/info.txt
echo "Deep inside beta" \
    > testdir/beta/deep2/data.txt
dd if=/dev/urandom \
    of=testdir/beta/deep2/medium.bin \
    bs=512 count=3 2>/dev/null

# -------------------------------------------------
# Add soft links across subdirectories.
# These should be ignored by the sweeper.
# -------------------------------------------------
ln -s ../beta/memo.txt \
    testdir/alpha/link_to_memo.txt
ln -s ../alpha/notes.txt \
    testdir/beta/link_to_notes.txt
ln -s ../../testdir/alpha/deep1/info.txt \
    testdir/beta/deep2/link_to_info.txt

# -------------------------------------------------
# Add hard links across subdirectories.
# These share an iNode and should be detected
# in Pass 1 of findDups.
# -------------------------------------------------
ln testdir/root1.txt \
    testdir/alpha/hardlink_root1.txt
ln testdir/alpha/notes.txt \
    testdir/beta/deep2/hardlink_notes.txt
ln testdir/beta/memo.txt \
    testdir/alpha/deep1/hardlink_memo.txt

# -------------------------------------------------
# Copy files to create true content duplicates.
# These have different iNodes but identical
# contents and should be detected in Pass 2.
# -------------------------------------------------
cp testdir/root1.txt \
    testdir/beta/deep2/root1_copy.txt
cp testdir/alpha/notes.txt \
    testdir/beta/notes_dup.txt
cp testdir/beta/deep2/data.txt \
    testdir/data_dup.txt

echo "Test directory created."
echo ""

# -------------------------------------------------
# Show the full tree at the top of results.
# -------------------------------------------------
echo "=== Directory tree ===" > "$RESULTS"
if command -v tree &>/dev/null; then
    tree testdir >> "$RESULTS"
else
    find testdir -print | sort >> "$RESULTS"
fi
echo "" >> "$RESULTS"

echo "=== ls -laR of testdir ===" \
    >> "$RESULTS"
ls -laR testdir/ >> "$RESULTS"
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 1: Default run on test directory.
# This is the primary test for findDups.
# It should find hard-link duplicates (same
# iNode) and content-identical duplicates
# (same length, same contents).
# -------------------------------------------------
echo "=== Test 1: Default run ===" \
    >> "$RESULTS"
echo "(findDups on test directory)" \
    >> "$RESULTS"
./sweeper -d testdir >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 2: Verbose mode on test directory.
# Shows all directory entries including links.
# -------------------------------------------------
echo "=== Test 2: Verbose ===" \
    >> "$RESULTS"
./sweeper -d testdir -v \
    >> "$RESULTS" 2>&1
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
# -------------------------------------------------
echo "=== Test 4: Output file ===" \
    >> "$RESULTS"
./sweeper -d testdir -o out.txt \
    >> "$RESULTS" 2>&1
echo "(Sweeper output in out.txt):" \
    >> "$RESULTS"
cat out.txt >> "$RESULTS" 2>/dev/null
rm -f out.txt
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 5: --size filter (skip small files).
# With --size 100, the 10-byte files and most
# small text files should be excluded.
# -------------------------------------------------
echo "=== Test 5: --size 100 ===" \
    >> "$RESULTS"
./sweeper -d testdir --size 100 \
    >> "$RESULTS" 2>&1
echo "" >> "$RESULTS"

# -------------------------------------------------
# Test 6: Run on home directory.
# This is the second test required by the
# assignment: run on a greater portion of
# the home directory.  Output truncated to
# keep results manageable.
# -------------------------------------------------
echo "=== Test 6: Home directory ===" \
    >> "$RESULTS"
echo "(Running on \$HOME, " \
    "truncated to 300 lines)" \
    >> "$RESULTS"
./sweeper -d "$HOME" 2>&1 \
    | head -300 >> "$RESULTS"
echo "... (truncated) ..." >> "$RESULTS"
echo "" >> "$RESULTS"

# -------------------------------------------------
# Done. Show the results.
# -------------------------------------------------
echo "All tests complete."
echo "Results saved to $RESULTS"
echo ""
cat "$RESULTS"
