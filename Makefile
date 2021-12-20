TARGET = exorcists
LIBS = -lm
CC = mpicc
CFLAGS = -g -Wall

.PHONY: run all clean build

all: build run

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

run:
	mpirun -np 4 ./$(TARGET)

build: $(TARGET)

clean:
	-rm -f *.o
	-rm -f $(TARGET)