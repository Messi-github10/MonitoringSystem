# 最简Makefile
CXX := g++
CXXFLAGS := -std=c++17 -I.
TARGET := sql_demo

SRCS := Main.cpp SQLTemplate.cpp
OBJS := $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp json.hpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) json.hpp

.PHONY: clean