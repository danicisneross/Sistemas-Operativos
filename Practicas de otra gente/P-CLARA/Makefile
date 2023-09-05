CC=gcc
CFLAGS=-I.
DEPS = list.h listMem.h listJobs.h
OBJ = p3.o
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

p3: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)