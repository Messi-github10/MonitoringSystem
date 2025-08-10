# 编译器设置
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
LDFLAGS := -lconfig++ -lpthread

# 目标文件和源文件
TARGET := Config_Demo
SRC_DIR := .
SRCS := $(SRC_DIR)/Main.cpp
OBJS := $(SRCS:.cpp=.o)
HEADERS := Configuration.hpp Noncopyable.hpp

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# 编译.cpp为.o (自动检测头文件依赖)
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 运行程序 (需确保server.conf存在)
run: $(TARGET)
	./$(TARGET)

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean