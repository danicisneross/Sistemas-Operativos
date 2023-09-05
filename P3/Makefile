p3 :  p3.o historial.o lmem.o lproc.o lenv.o
	gcc -o p3  p3.o historial.o lmem.o lproc.o lenv.o
	echo "Compilación exitosa"  

historial.o : historial.h historial.c
	gcc -c historial.c

lmem.o : lmem.h lmem.c
	gcc -c lmem.c

lproc.o : lproc.h lproc.c
	gcc -c lproc.c

lenv.o : lenv.h lenv.c
	gcc -c lenv.c

p3.o : p3.h p3.c
	gcc -c p3.c
	
limpar:
	rm p3 historial.o p3.o lmem.o lproc.o lenv.o
