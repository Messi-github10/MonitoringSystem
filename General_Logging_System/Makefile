# 编译器
CXX := g++

# 编译选项
CXXFLAGS := -std=c++17 -Wall -Wextra -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG

# 链接选项
LDFLAGS := -lfmt -lspdlog -pthread

# 目标可执行文件
TARGET := Logger_Demo

# 源文件和对象文件
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# 生成对象文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean