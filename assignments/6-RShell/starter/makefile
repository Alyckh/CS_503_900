# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable name
TARGET = dsh

# Find all source and header files
SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)

# Default target
all: $(TARGET) copy_to_bats  # Ensure dsh is copied to starter/bats/

# Compile source to executable
$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Ensure bats/ exists and copy dsh there only if needed
copy_to_bats: $(TARGET)
	@if [ "$(TARGET)" != "bats/$(TARGET)" ]; then cp $(TARGET) bats/$(TARGET); fi

# Clean up build files
clean:
	rm -f $(TARGET) bats/$(TARGET)

test: all  # Ensure the latest dsh is available before testing
	bats $(wildcard bats/*.sh)

valgrind:
	echo "pwd\nexit" | valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET) 
	echo "pwd\nexit" | valgrind --tool=helgrind --error-exitcode=1 ./$(TARGET) 

# Phony targets
.PHONY: all clean test copy_to_bats
