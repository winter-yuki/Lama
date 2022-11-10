all: byterun.o lib.o
	$(CC) -m32 -g -o -byterun byterun.o lib.o ../runtime/runtime.a

byterun.o: runtime.o byterun.c lib.c lib.h
	$(CC) -g -fstack-protector-all -m32 -c byterun.c lib.c

runtime.o:
	$(MAKE) -C ../runtime

clean:
	$(RM) *.a *.o *~
