CFLAGS = -Wall -Wextra -pedantic -Iinclude -g3

HDR = $(wildcard include/*.h)
SRC_OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
TARGET = specs

TEST_OBJ = $(patsubst %.c,%.o,$(wildcard tests/*/*.c))
TEST_TARGETS = tests/test_insertion

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

# Final executable
$(TARGET): $(SRC_OBJ) $(HDR)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_insertion: tests/insertion/test.o src/operations.o src/hashtable.o src/spec.o $(HDR)
	$(CC) $(CFLAGS) $^ -o $@

tests: $(TEST_TARGETS)

check: tests
	@cd tests; \
	for bin in ${TEST_TARGETS}; do \
		run="./$$(basename $$bin)" ; \
		echo "[Running $$run]"; \
		"./$$run"; \
	done; \
	cd ..

# The all-important clean target
clean:
	$(RM) $(SRC_OBJ) $(TARGET) $(TEST_OBJ) $(TEST_TARGETS)

.PHONY: tests check clean
