#include "dijkstra_mpi.h"

#include <mpi.h>
#include <stdlib.h>

#include "constants.h"


// Send portions of the entire graph from the root process to others
void ScatterGraph(unsigned **graph, unsigned dim, unsigned nodes, int rank, int size)
{
    if (rank == 0) {
        // Let the root process send portions of the entire graph
        for (int proc = 0; proc < size; proc++) {
            unsigned msg_size = nodes;
            if (proc == size - 1)
                msg_size += dim % size;
            for (int i = 0; i < dim; i++)
                MPI_Send(&(graph[i][proc * nodes]), msg_size, MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    } else {
        // Recieve portioned graph on non-root processes
        for (int i = 0; i < dim; i++)
            MPI_Recv(&(graph[i][0]), nodes, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
    }
    return;
}


// Collect results from Dijkstra function on the root process
void GatherResults(unsigned *res, unsigned dim, unsigned nodes, int rank, int size)
{
    if (rank > 0) {
        // Send local results from non-root processes
        MPI_Send(&(res[0]), nodes, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        // Recieve results on root process
        for (int proc = 1; proc < size; proc++) {
            unsigned msg_size = nodes;
            if (proc == size - 1)
                msg_size += dim % size;
            MPI_Recv(&(res[proc * nodes]), msg_size, MPI_INT, proc, 0, MPI_COMM_WORLD, NULL);
        }
    }
    return;
}


// Dijkstra algorithm to find the shortest path to all nodes from a start node
void Dijkstra(unsigned **graph, unsigned *dist, unsigned *prev, unsigned dim, unsigned nodes,
              unsigned node, int rank, int size)
{
    // Calculcate real start node number
    unsigned tmp_nodes = nodes;
    if (rank == size - 1)
        tmp_nodes = nodes - dim % size;
    unsigned start_node = rank * tmp_nodes;

    // Array to keep track which nodes have been used
    unsigned *used = malloc(dim * sizeof(unsigned));
    // Initialize used nodes array (set every node to unused at first)
    for (int i = 0; i < dim; i++)
        used[i] = 0;

    // Initialize local distance and local previous node array
    for (int i = 0; i < nodes; i++) {
        dist[i] = INF;
        prev[i] = INF;
    }
    // If the start node is inside portion, set distance to zero and previous node to itself
    if (node >= start_node && node < start_node + nodes) {
        dist[node - start_node] = 0;
        prev[node - start_node] = node;
    }

    for (int i = 0; i < dim; i++) {
        // Find next node with minimum distance to start node in local graph
        unsigned loc_min_node, loc_min_dist = INF;
        for (int j = 0; j < nodes; j++) {
            // New node has to be unused and have local minimum distance
            if (!used[start_node + j] && dist[j] < loc_min_dist) {
                loc_min_dist = dist[j];
                loc_min_node = start_node + j;
            }
        }

        // Struct to save global minimum distance with corresponding node
        struct {
            unsigned dist;
            unsigned node;
        }
        min = {min.dist = loc_min_dist, min.node = loc_min_node};
        // Find global minimum distance
        MPI_Allreduce(MPI_IN_PLACE, &min, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);

        // Set min.node to used
        used[min.node] = 1;

        // Update local distance array if node is unused and new path is shorter
        for (int j = 0; j < nodes; j++) {
            // Check for connection between min.node and j and if min.node is reachable at all
            // (tmp_dist < INF) imitates (min.dist != INF && graph[min.node][j] != INF)
            unsigned tmp_dist = min.dist + graph[min.node][j];
            if (!used[start_node + j] && tmp_dist < dist[j] && tmp_dist < INF) {
                dist[j] = tmp_dist;
                prev[j] = min.node;
            }
        }
    }

    // Collect results on root process
    GatherResults(dist, dim, nodes, rank, size);
    GatherResults(prev, dim, nodes, rank, size);

    // Free allocated memory
    free(used);
    return;
}

