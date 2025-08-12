# 定义变量
SRCS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp,%, $(SRCS))

all:$(OBJS)

%:%.cc
	g++ -o $@ $^

clean:
	rm -rf $(OBJS)

# 打印变量的信息
echo:
	@echo $(SRCS)
	@echo $(OBJS)