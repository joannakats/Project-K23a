CFLAGS = -Wall -Wextra -pedantic -Iinclude

# Run make DEBUG=1 for debug build
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	# Force rebuild if previous compilation was 'release'
	ifeq (,$(wildcard .debug))
		MAKEFLAGS += -B
	endif

	CFLAGS += -g3 -DDEBUG
	flag_debug := $(shell touch .debug)
else
	# Force rebuild if previous compilation was 'debug'
	ifneq (,$(wildcard .debug))
		MAKEFLAGS += -B
	endif

	CFLAGS += -O3 -DNDEBUG
	flag_release := $(shell rm -f .debug)
endif

HDR = $(wildcard include/*.h)

SRC_OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
TARGET = specs

TEST_OBJ = $(patsubst %.c,%.o,$(wildcard tests/*/*.c))
TEST_TARGETS = tests/test_spec

TARGETS = $(TARGET) $(TEST_TARGETS)

# Compilation
all: $(TARGETS)
tests: $(TEST_TARGETS)

## Compilation recipe for executables (common)
$(TARGETS):
	$(CC) $(CFLAGS) -o $@ $^

## Specific dependencies for executables (object files + $(HDR))
$(TARGET): $(SRC_OBJ) $(HDR)
tests/test_json_insertion: tests/json_insertion/test.o $(filter-out src/main.o,$(SRC_OBJ)) $(HDR)
tests/test_hstable: tests/hashtable/test.o src/hashtable.o src/spec_hashtable.o src/spec.o src/clique.o $(HDR)
tests/test_spec: tests/spec/test.o src/spec.o src/clique.o src/hashtable.o $(HDR)
tests/test_clique: tests/clique/test.o src/spec.o src/clique.o $(HDR)

## Object files
%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

# Run tests
check: tests
	cd tests && $(foreach bin,$(notdir $(TEST_TARGETS)),./$(bin);)

# The all-important clean target
clean:
	$(RM) $(SRC_OBJ) $(TEST_OBJ) $(TARGETS)

.PHONY: all tests check clean
