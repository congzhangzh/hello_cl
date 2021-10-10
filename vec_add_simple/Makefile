# 指令编译器和选项,-g一定需要加上，否则无法调试
CC=g++
CFLAGS=-Wall -std=gnu99 -g
 
# 目标文件
TARGET=vec-add-simple
# 源文件
SRCS=vec-add-simple.c
 
OBJS = $(SRCS:.c=.o)
 
$(TARGET):$(OBJS)
#	@echo TARGET:$@
#	@echo OBJECTS:$^
	$(CC) -o $@ $^ -lOpenCL
 
clean:
	rm -rf $(TARGET) $(OBJS)
 
%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<
