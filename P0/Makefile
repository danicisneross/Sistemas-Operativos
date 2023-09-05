p0 :  p0.o historial.o 
	gcc -o p0 p0.o historial.o
	echo "Compilación exitosa"  

historial.o : historial.h historial.c
	gcc -c  historial.c

p0.o : p0.h p0.c
	gcc -c p0.c
	
limpar:
	rm p0 historial.o p0.o
