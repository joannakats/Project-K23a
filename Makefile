CFLAGS = -Wall -Wextra -pedantic -I ./include/

HDR = $(wildcard include/*.h)
SRC_OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
TARGET = specs

TEST_OBJ = $(patsubst %.c,%.o,$(wildcard tests/*/*.c))
TEST_TARGETS = tests/test_insertion

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -g -c $< -o $@

# Final executable
$(TARGET): $(SRC_OBJ) $(HDR)
	$(CC) $(CFLAGS) $^ -o $@

tests: $(TEST_TARGETS)

tests/test_insertion: tests/insertion/main.o src/operations.o src/hashtable.o src/spec.o $(HDR)
	$(CC) $(CFLAGS) $^ -o $@

# The all-important clean target
.PHONY: clean
clean:
	$(RM) -r $(SRC_OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGETS)
