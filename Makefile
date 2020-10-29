CFLAGS = -Wall -Wextra -pedantic -I ./include/

SRC_OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
TEST_OBJ = $(patsubst %.c,%.o,$(wildcard tests/*.c))
HDR = $(wildcard include/*.h)
TARGET = specs

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -g -c $< -o $@

# Final executable
$(TARGET): $(SRC_OBJ) $(HDR)
	$(CC) $(CFLAGS) $^ -o bin/$@

# Tests

hashtable: src/hashtable.o

spec: src/spec.o

cliquep: tests/clique_print.o src/spec.o include/spec.h
	$(CC) $(CFLAGS) $^ -o bin/$@

# New test? Put the *.c file with your main in tests/
# and make a new rule like the above cliquep with its dependencies

# The all-important clean target
clean:
	$(RM) -r $(SRC_OBJ) $(TEST_OBJ) bin/

# This will create the bin directory after Makefile parsing
$(info $(shell mkdir -p bin/))
