.PHONY: clean

CC := gcc
CFLAGS := -I./include -Wall -Wextra -Wpedantic -g

bin/pingame: build/pingame.o build/trie.o src/main.c
	$(CC) $(CFLAGS) -o bin/pingame build/trie.o build/pingame.o src/main.c

build/trie.o: src/trie.c include/trie.h
	$(CC) $(CFLAGS) -c src/trie.c -o build/trie.o

build/pingame.o: src/pingame.c include/pingame.h
	$(CC) $(CFLAGS) -c src/pingame.c -o build/pingame.o

clean:
	rm -f bin/pingame
	rm -f build/trie.o
	rm -f build/pingame.o
