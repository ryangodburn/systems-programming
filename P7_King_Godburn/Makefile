# Makefile -------------------------------------------
# Authors: Ryan Godburn and Emily King
# Builds both mom (server) and kid (client) for
# the Musical Chairs socket program.
# ---------------------------------------------------

CXX      = g++
CXXFLAGS = -Wall -g -std=c++17

# Build both executables.
all: mom kid

# ----- Mom (server) -----
mom: mom.o momLogic.o
	$(CXX) $(CXXFLAGS) -o mom mom.o momLogic.o

mom.o: mom.cpp momLogic.hpp protocol.hpp socktypes.h
	$(CXX) $(CXXFLAGS) -c mom.cpp

momLogic.o: momLogic.cpp momLogic.hpp protocol.hpp
	$(CXX) $(CXXFLAGS) -c momLogic.cpp

# ----- Kid (client) -----
kid: kid.o kidLogic.o
	$(CXX) $(CXXFLAGS) -o kid kid.o kidLogic.o

kid.o: kid.cpp kidLogic.hpp protocol.hpp socktypes.h
	$(CXX) $(CXXFLAGS) -c kid.cpp

kidLogic.o: kidLogic.cpp kidLogic.hpp protocol.hpp
	$(CXX) $(CXXFLAGS) -c kidLogic.cpp

# ----- Clean -----
clean:
	rm -f *.o mom kid
