CC = gcc
CFLAGS = -g -std=c11 -pedantic -Wall -Wextra -fPIC -O2
LDFLAGS =
EXECUTABLE = tail wordcount wordcount-dynamic
HTAB_OBJECTS = htab_init.o htab_size.o htab_bucket_size.o htab_find.o htab_lookup_add.o htab_hash_function.o htab_erase.o htab_free.o htab_clear.o htab_statistics.o htab_for_each.o

#LDFLAGS += -fsanitize=address
#CFLAGS += -fsanitize=address
#CFLAGS += -DSTATISTICS


all: $(EXECUTABLE) libhtab.a libhtab.so

run: all
	./wordcount < io.h
	LD_LIBRARY_PATH=. ./wordcount-dynamic < io.h
	./tail -n 5 wordcount.c

libhtab.a: $(HTAB_OBJECTS)
	ar crs $@ $^

libhtab.so: $(HTAB_OBJECTS)
	$(CC) -shared -fPIC $^ -o $@

wordcount: wordcount.o libhtab.a io.o
	$(CC) $(CFLAGS) -o $@ -static wordcount.o io.o -L. -lhtab $(LDFLAGS)

wordcount-dynamic: wordcount.o libhtab.so io.o
	$(CC) $(CFLAGS) -o $@ wordcount.o io.o -L. -lhtab $(LDFLAGS)

tail: tail.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(EXECUTABLE) *.a *.so

zip:
	zip xbehoua00.zip *.c *.cc *.h Makefile deps


-include deps
