CFLAGS = -Wall -Werror -Wextra -std=c99 -O2

# Since this is the first target, it will be executed by default.
.PHONY: all
all: testvector


#PHONY's are just to ensure, that the rule 'clean' is used, rather then the file 'clean'
.PHONY: clean
clean:
	rm -f btree.o btree_test.o testvector
	clear

#Our Phony will ensure, that even when the testvector is up-to-date, it will build
.PHONY:testvector
testvector: btree.o btree_test.o
	gcc $(CFLAGS) btree.o btree_test.o -o $@
	rm -f btree.o btree_test.o

btree.o: btree.c btree.h
	gcc $(CFLAGS) -c btree.c
	
btree_test.o: btree_test.c
	gcc $(CFLAGS) -c btree_test.c

#This is just for the task, because I don't have access to a computer with linux (or bash at all)
#so that even if i can't verify my program has no leaks, I can atleast say, I could have
#used valgrind
.PHONY: valgrind
valgrind:
	valgrind --leak-check=full -v ./testvector
