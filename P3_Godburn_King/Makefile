# -------------------------------------------------
# Makefile for Program 3: Sweeper
# By Ryan Godburn and Emily King
# -------------------------------------------------

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

OBJS = main.o Params.o Sweeper.o FileID.o \
       Stats.o Direntry.o tools.o

TARGET = sweeper

# Link all object files into the executable.
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile individual source files.
main.o: main.cpp tools.hpp Params.hpp Sweeper.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Params.o: Params.cpp Params.hpp
	$(CXX) $(CXXFLAGS) -c Params.cpp

Sweeper.o: Sweeper.cpp Sweeper.hpp Params.hpp \
           Direntry.hpp Stats.hpp FileID.hpp \
           tools.hpp
	$(CXX) $(CXXFLAGS) -c Sweeper.cpp

FileID.o: FileID.cpp FileID.hpp
	$(CXX) $(CXXFLAGS) -c FileID.cpp

Stats.o: Stats.cpp Stats.hpp
	$(CXX) $(CXXFLAGS) -c Stats.cpp

Direntry.o: Direntry.cpp Direntry.hpp
	$(CXX) $(CXXFLAGS) -c Direntry.cpp

tools.o: tools.cpp tools.hpp
	$(CXX) $(CXXFLAGS) -c tools.cpp

clean:
	rm -f $(OBJS) $(TARGET)
