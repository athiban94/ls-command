# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
CFLAGS  = -g -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init -Wlogical-op -Wpedantic -Wshadow

# the build target executable:
TARGET = ls
PRINT = print
CMP = cmp

all: $(TARGET)

$(TARGET): $(TARGET).c 
		$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(PRINT).c $(CMP).c -lm

clean: 
		rm $(TARGET)

debug:
		$(CC) -g -o $(TARGET) $(TARGET).c $(PRINT).c $(CMP).c -lm
flags:
		$(CC) -o $(TARGET) $(TARGET).c $(PRINT).c $(CMP).c -lm