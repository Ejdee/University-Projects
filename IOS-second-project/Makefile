# Makefile
# Autor: Adam Běhoun, FIT
# Datum: 19.3.2024
# login: xbehoua00


LC_ALL=cs_CZ.utf8
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -g
LDFLAGS = -lrt -lpthread
LOGIN = xbehoua00

#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address
#CFLAGS += -m32
#LDFLAGS += -m32

EXECUTABLE = proj2

all: $(EXECUTABLE)

proj2: proj2.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)

zip: clean
	zip $(LOGIN).zip *.c *.h Makefile