CFLAGS = -Wall -Wextra -pedantic -Iinclude -g3

HDR = $(wildcard include/*.h)

SRC_OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
TARGET = specs

TEST_OBJ = $(patsubst %.c,%.o,$(wildcard tests/*/*.c))
TEST_TARGETS = tests/test_insertion tests/test_spec

TARGETS = $(TARGET) $(TEST_TARGETS)

# Default goal
all: $(TARGETS)

# Object files
%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

# Compilation recipe for executables (common)
$(TARGETS):
	$(CC) $(CFLAGS) -o $@ $^

# Specific dependencies for executables (object files + $(HDR))
$(TARGET): $(SRC_OBJ) $(HDR)
tests/test_insertion: tests/insertion/test.o src/operations.o src/hashtable.o src/spec.o $(HDR)
tests/test_spec: tests/spec/test.o src/spec.o $(HDR)

tests: $(TEST_TARGETS)

check: tests
	@cd tests && \
	for bin in ${TEST_TARGETS}; do \
		run="$$(basename $$bin)" ; \
		echo "[Running $$run]"; \
		"./$$run"; \
	done; \
	cd ..

# The all-important clean target
clean:
	$(RM) $(SRC_OBJ) $(TEST_OBJ) $(TARGETS)

.PHONY: all tests check clean
