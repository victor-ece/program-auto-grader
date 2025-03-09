CC = gcc
CFLAGS = -Wall -g
Sanitize = -fsanitize=address

hw4: hw4.c p4diff
	$(CC) $(CFLAGS) hw4.c -o hw4 $(Sanitize)

p4diff: p4diff.c
	$(CC) $(CFLAGS) p4diff.c -o p4diff 

.PHONY: clean

clean:
	rm hw4 p4diff