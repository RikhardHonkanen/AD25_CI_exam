CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++17 -MMD

TEST_EXE = test
BUILD_DIR = build

OBJECTS = $(wildcard *.cpp)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(OBJECTS:.cpp=.o))

all: $(BUILD_DIR) $(BUILD_DIR)/$(TEST_EXE)

$(BUILD_DIR)/$(TEST_EXE): $(OBJECTS)
	@$(CC) $+ -lgtest -lgtest_main -lgmock -o $@

$(BUILD_DIR)/%.o: %.cpp
	@$(CC) -c $(CFLAGS) $< -o $@

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all check clean

check: $(BUILD_DIR) $(BUILD_DIR)/$(TEST_EXE)
	@./$(BUILD_DIR)/$(TEST_EXE)

clean:
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@