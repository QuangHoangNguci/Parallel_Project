CC = mpicc
CFLAGS = -std=c99
SRC = src


dijkstra: $(SRC)/main.c create_graph.o dijkstra_mpi.o print_utils.o
	$(CC) $(CFLAGS) -o dijkstra $(SRC)/main.c create_graph.o dijkstra_mpi.o print_utils.o

create_graph.o: $(SRC)/create_graph.c
	$(CC) $(CFLAGS) -c $(SRC)/create_graph.c

dijkstra_mpi.o: $(SRC)/dijkstra_mpi.c
	$(CC) $(CFLAGS) -c $(SRC)/dijkstra_mpi.c

print_utils.o: $(SRC)/print_utils.c
	$(CC) $(CFLAGS) -c $(SRC)/print_utils.c


all: dijkstra

clean:
	rm -f dijkstra create_graph.o dijkstra_mpi.o print_utils.o

new: clean all
