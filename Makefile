CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Wno-deprecated-declarations

allocator: allocator.cpp
	$(CXX) $(CXXFLAGS) -o allocator allocator.cpp

clean:
	rm -f allocator