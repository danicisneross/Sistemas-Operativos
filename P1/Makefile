p1 :  p1.o historial.o 
	gcc -o p1  p1.o historial.o
	echo "Compilación exitosa"  

historial.o : historial.h historial.c
	gcc -c historial.c

p1.o : p1.h p1.c
	gcc -c p1.c
	
limpar:
	rm p1 historial.o p1.o
