CXXFLAGS := -Wall -Wextra -pedantic -std=c++17
CXXFLAGS += -Os -flto -s

apt-encode: apt-encode.cpp Makefile