# CSCI 4547: Systems Programming

Coursework from **CSCI 4547: Systems Programming** at the **University of New Haven**, Spring 2026 (fourth year, spring semester).

- **Authors:** Ryan Godburn and Emily King (pair projects)
- **Language:** C++ on Linux (POSIX APIs)

Programs 2–5 build one tool step by step, a file **sweeper** that walks a directory tree and finds duplicate files. Programs 6 and 7 implement the same game of musical chairs twice, first with threads and then with sockets. P3–P7 include a `testingScript.sh` that builds the program and runs its test cases, and P2–P7 include a `test_results.txt` with the recorded output.

## Programs

| Folder | Program | What it does |
| --- | --- | --- |
| `P1_King_Godburn` | Hello Shell | Sorts each command-line argument into the command, switches (starting with `-`) and plain arguments, printing to the console and appending to a log file. |
| `P2_King_Godburn` | Params | Parses the sweeper's command line with `getopt_long()`, mixing short and long options: `-v`/`--verbose`, `-h`/`--help`, `-o`/`--output <file>`, `-d <dir>` for the starting directory, and the long-only `--size <n>` and `--ignore-case`. |
| `P3_Godburn_King` | Sweeper | Reads one directory with `opendir()` / `readdir()`, calls `lstat()` on each regular file, and builds a list of files (name, inode, size) with summary statistics. |
| `P4_Godburn_King` | Recursive Sweeper | Walks the whole directory tree from an absolute starting path. |
| `P5_Godburn_King` | Eliminating Duplicates | Sorts the files by size, then reports duplicates: hard links (same inode) and separate files with identical contents. |
| `P6_Godburn_King` | Musical Chairs with Threads | "Mom" runs the game and each "kid" is a POSIX thread. They coordinate through a shared model protected by a mutex and condition variables, and mom signals the kids with `SIGUSR1` / `SIGUSR2` to march and sit. Each round removes a chair and the kid left standing until one winner remains. |
| `P7_King_Godburn` | Musical Chairs with Sockets | The same game over TCP: `mom` is a server that accepts N kid connections and multiplexes them with `poll()`; each `kid` is a client. They talk through a small message protocol (`GETUP`, `SIT`, `WANT`, `ACK`, `NACK`, `QUIT`). |

## Build and run

P3–P7 have Makefiles:

```bash
cd P5_Godburn_King
make
./sweeper --help

cd ../P7_King_Godburn
make
./mom 3                  # server on port 1234: wait for 3 kids
./kid localhost alice    # run each kid in its own terminal
```

P1 and P2 compile directly:

```bash
g++ -std=c++23 -Wall -o P1_King_Godburn P1_King_Godburn/main.cpp
g++ -std=c++17 -o params P2_King_Godburn/*.cpp
```

In P3–P7, `bash testingScript.sh` rebuilds that program, reruns its tests and regenerates `test_results.txt`.

---

*Archived coursework, shared as part of my portfolio. If you're taking this course now, please follow your instructor's academic integrity policy.*
