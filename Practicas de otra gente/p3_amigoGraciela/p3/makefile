shell.out: p3.o listah.o listamem.o listaproc.o
	gcc -o shell.out p3.o listah.o listamem.o listaproc.o -g

listah.o: listah.c listah.h
	gcc -c listah.c -g
	
listamem.o: listamem.c listamem.h
	gcc -c listamem.c -g
	
listaproc.o: listaproc.c listaproc.h
	gcc -c listaproc.c -g	

p3.o: p3.c listah.h listamem.h listaproc.h
	gcc -c p3.c -g

clean:
	rm -f listah.o p3.o listamem.o listaproc.o shell.out
