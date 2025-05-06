# Compiler configuration
CC = gcc
CFLAGS = -fsanitize=address -Wall -Wextra -g
UNITY_DIR = ./src/unity

# Directory structure
BUILD_DIR = build
SRC_DIR = src

# File discovery
ALL_SRCS = $(wildcard $(SRC_DIR)/*.c)
MAIN_SRC = $(SRC_DIR)/main.c
LIB_SRCS = $(filter-out $(MAIN_SRC) $(SRC_DIR)/test_%.c, $(ALL_SRCS))
TEST_SRCS = $(wildcard $(SRC_DIR)/test_*.c)

# Build targets
TARGET = $(BUILD_DIR)/crn
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SRCS))
TEST_EXECUTABLES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.out,$(TEST_SRCS))

# Ensure build directory exists
$(shell mkdir -p $(BUILD_DIR))

# Main targets
.PHONY: all test clean

all: $(TARGET)

test: $(TEST_EXECUTABLES)
	@echo "Running all tests..."
	@for test in $^; do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "All tests passed!"

clean:
	rm -rf $(BUILD_DIR)

# Main application build rule
$(TARGET): $(LIB_OBJS) $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(MAIN_SRC))
	$(CC) $(CFLAGS) -o $@ $^

# Library object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test executables
$(BUILD_DIR)/test_%.out: $(SRC_DIR)/test_%.c
	$(CC) $(CFLAGS) -I$(UNITY_DIR) $(LIB_SRCS) $< $(UNITY_DIR)/unity.c -o $@