# Makefile
# Řešení IJC-DU1, příklad a)
# Autor: Adam Běhoun, FIT
# Datum: 19.3.2024
# login: xbehoua00
# Přeloženo: gcc (GCC) 10.5.0


LC_ALL=cs_CZ.utf8
CC = gcc
CFLAGS = -O2 -g -std=c11 -pedantic -Wall -Wextra
LDFLAGS = -lm
LOGIN = xbehoua00

#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address
#CFLAGS += -m32
#LDFLAGS += -m32

EXECUTABLE = primes primes-i no-comment

all: $(EXECUTABLE)

run: all
	ulimit -s 82000 && ./primes
	ulimit -s 82000 && ./primes-i

no-comment: no-comment.o error.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

primes: primes.o eratosthenes.o error.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

primes-i: primes-i.o eratosthenes-i.o error.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# create the dependencies
eratosthenes.o: eratosthenes.c eratosthenes.h bitset.h error.h
error.o: error.c error.h
no-comment.o: no-comment.c error.h
primes.o: primes.c eratosthenes.h bitset.h error.h

# compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# compile with inline functions separately
eratosthenes-i.o: eratosthenes.c eratosthenes.h bitset.h error.h
	$(CC) $(CFLAGS) -c -DUSE_INLINE $< -o $@

primes-i.o: primes.c eratosthenes.h bitset.h error.h
	$(CC) $(CFLAGS) -c -DUSE_INLINE $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)

zip: clean
	zip $(LOGIN).zip *.c *.h Makefile