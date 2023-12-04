todo: repostero cocineros mozos

mozos: mozos.o restaurante.o
	g++ -Wall -g -o mozos mozos.o restaurante.o 
mozos.o: mozos.c 
	g++ -Wall -g -c mozos.c 

cocineros: cocineros.o restaurante.o
	g++ -Wall -g -o cocineros cocineros.o restaurante.o 
cocineros.o: cocineros.c 
	g++ -Wall -g -c cocineros.c 

repostero: repostero.o restaurante.o
	g++ -Wall -g -o repostero repostero.o restaurante.o 
repostero.o: repostero.c 
	g++ -Wall -g -c repostero.c 


restaurante.o: restaurante.c restaurante.h
	g++ -Wall -g -c restaurante.c
clean:
	rm -f *.o
	rm -f cocineros mozos repostero