# 定义变量
CC = gcc
CFLAGS = -Wall -I.

# 定义目标文件
TARGET = libxconn_leak_detect.so

# 定义源文件
SOURCES = $(wildcard *.c)

# 生成可执行文件
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ -shared -fPIC -g -o $@

# 清理编译产物
clean:
	rm -f $(TARGET) $(SOURCES:.c=.o)
