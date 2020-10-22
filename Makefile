CFLAGS = -Wall -Wextra -pedantic -I ./include/

OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
HDR = $(wildcard include/*.h)
TARGET = specs

all: $(TARGET)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -g -c $< -o $@

# Individual targets for testing
main: src/main.o
	$(CC) $(CFLAGS) $^ -o $@

hashtable: src/hashtable.o

spec: src/spec.o

# Final executable
$(TARGET): $(OBJ) $(HDR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(OBJ) $(TARGET) main
