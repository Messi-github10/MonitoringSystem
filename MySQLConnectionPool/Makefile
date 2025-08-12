# 编译器配置
CXX := g++
COM_OP := -Wall -g -O0
LDFLAGS := -lmysqlclient  # 关键：链接 MySQL 库
TARGET := Main.exe

# 自动获取源文件
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPENDS := $(SRCS:.cpp=.d)

.PHONY: all clean rebuild

# 编译规则
%.o: %.cpp
	$(CXX) -c $< -o $@ $(COM_OP)

# 链接规则（必须传递 LDFLAGS）
$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(COM_OP) $(LDFLAGS)  # 确保此处有 $(LDFLAGS)

# 依赖生成
%.d: %.cpp
	$(CXX) -MM $< -o $@

-include $(DEPENDS)

clean:
	$(RM) $(TARGET) $(OBJS) $(DEPENDS)

rebuild: clean all
