MAKEFLAGS = -j$(shell nproc)
CFLAGS = -Wall -Wextra -pedantic -Iinclude
LIBS = -lm

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
# TODO: Fix TESTS for fields hashtable
TEST_TARGETS = tests/test_vocabulary tests/test_spec tests/test_clique tests/test_hstable
#tests/test_json_insertion

TARGETS = $(TARGET) $(TEST_TARGETS)

# Compilation
all: $(TARGETS)
tests: $(TEST_TARGETS)

## Compilation recipe for executables (common)
$(TARGETS):
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

## Specific dependencies for executables (object files + $(HDR))
$(TARGET): $(SRC_OBJ) $(HDR)
tests/test_json_insertion: tests/json_insertion/test.o $(filter-out src/main.o,$(SRC_OBJ)) $(HDR)
tests/test_vocabulary: tests/vocabulary/test.o src/hashtable.o src/spec_hashtable.o src/spec.o src/clique.o src/preprocessing.o src/vocabulary.o $(HDR)
tests/test_hstable: tests/hashtable/test.o src/hashtable.o src/spec_hashtable.o src/spec.o src/clique.o src/json.o $(HDR)
tests/test_spec: tests/spec/test.o src/hashtable.o src/spec.o src/clique.o src/json.o $(HDR)
tests/test_clique: tests/clique/test.o src/spec.o src/clique.o src/hashtable.o src/json.o src/spec_hashtable.o $(HDR)

## Object files
%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $< $(LIBS)

# Run tests
check: tests
	cd tests && $(foreach bin,$(notdir $(TEST_TARGETS)),./$(bin);)

# The all-important clean target
clean:
	$(RM) $(SRC_OBJ) $(TEST_OBJ) $(TARGETS)

.PHONY: all tests check clean
