CFLAGS += -std=gnu99
CFLAGS += -DDEBUG
pos: pos.o art.o bell.o database.o nfc.o key.o tube.o vla.o
	$(CC) $^ -o $@ -lpthread
clean:
	rm -f *.o pos
