todo: repostero cocineros mozos

mozos: mozos.o restaurante.o
	g++ -g -o mozos mozos.o restaurante.o 
mozos.o: mozos.c 
	g++ -g -c mozos.c 

cocineros: cocineros.o restaurante.o
	g++ -g -o cocineros cocineros.o restaurante.o 
cocineros.o: cocineros.c 
	g++ -g -c cocineros.c 

repostero: repostero.o restaurante.o
	g++ -g -o repostero repostero.o restaurante.o 
repostero.o: repostero.c 
	g++ -g -c repostero.c 


restaurante.o: restaurante.c restaurante.h
	g++ -g -c restaurante.c
clean:
	rm -f *.o
	rm -f cocineros mozos repostero