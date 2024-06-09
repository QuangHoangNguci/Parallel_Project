#ifndef DIJKSTRA_MPI_H_
#define DIJKSTRA_MPI_H_

void ScatterGraph(unsigned **graph, unsigned dim, unsigned nodes, int rank, int size);
void GatherResults(unsigned *res, unsigned dim, unsigned nodes, int rank, int size);
void Dijkstra(unsigned **graph, unsigned *dist, unsigned *prev, unsigned dim, unsigned nodes,
              unsigned node, int rank, int size);

#endif

