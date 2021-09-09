TARGET = simulator

.PHONY: all
all: main.c tape.h 103.h
	$(CC) main.c 103.c -o $(TARGET)

.PHONY: clean
clean:
	rm -f *.o $(TARGET)
