BUILD_DIR = build
TARGET = ${BUILD_DIR}/exorcists
LIBS = -lm -pthread
CC = mpicc
CFLAGS = -g -O0 -Wall

.PHONY: run all clean build

all: build run

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

run:
	mpirun -np 4 ./$(TARGET)

debug:
	${MAKE} build
	mpirun -np 2 valgrind --vgdb=yes --vgdb-error=0 ./$(TARGET)

install-valgrind:
	wget https://sourceware.org/pub/valgrind/valgrind-3.18.1.tar.bz2
	tar xvf valgrind-3.18.1.tar.bz2
	cd valgrind-3.18.1 && ./configure && make && sudo make install
	rm -rf valgrind-3.18.1*

build: $(TARGET)

clean:
	-rm -f *.o
	-rm -f $(TARGET)