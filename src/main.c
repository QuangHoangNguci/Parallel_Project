#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "create_graph.h"
#include "dijkstra_mpi.h"
#include "print_utils.h"

void AppendResultsToCSV(const char* filename, unsigned dim, double T_w_com, double T_wo_com) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%u,%.6lf,%.6lf\n", dim, T_w_com, T_wo_com);
    fclose(file);
}

int main(int argc, char **argv) {
    double start_time = MPI_Wtime();
    // Initialize MPI
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Default dimension is 5 (see TestGraph), default start node is the zero-node
    unsigned dim = 5, node = 0;
    // First command line parameter is the start node
    if (argc > 1)
        node = atoi(argv[1]);
    // Second command line parameter is the random graphs node amount
    if (argc == 3)
        dim = atoi(argv[2]);

    // Separate nodes between processes, the last one gets the leftovers
    unsigned nodes = dim / size;
    if (rank == size - 1)
        nodes += dim % size;

    unsigned **graph;
    unsigned *dist, *prev;
    graph = malloc(dim * sizeof(unsigned*));

    if (rank == 0) {
        // Only let the root process have a copy of the entire graph
        unsigned *tmp_arr = malloc(dim * dim * sizeof(unsigned));
        for (int i = 0; i < dim; i++)
            graph[i] = &(tmp_arr[i * dim]);

        // Reserve space for all results
        dist = malloc(dim * sizeof(unsigned));
        prev = malloc(dim * sizeof(unsigned));

        // Create graph
        printf("Create graph...\n");
        if (argc == 3)
            RandomGraph(graph, dim);
        else
            TestGraph(graph, dim);

        // Write graph to file
        WriteGraphToFile(graph, dim, "graph_output.txt");
    } else {
        // Non-root processes only gets a portion of the entire graph
        unsigned *tmp_arr = malloc(dim * nodes * sizeof(unsigned));
        for (int i = 0; i < dim; i++)
            graph[i] = &(tmp_arr[i * nodes]);

        // Reserve space for local results
        dist = malloc(nodes * sizeof(unsigned));
        prev = malloc(nodes * sizeof(unsigned));
    }

    double init_time = MPI_Wtime() - start_time;

    // Time before communication
    double start_comm_time = MPI_Wtime();
    // Send portions of the entire graph to each process
    ScatterGraph(graph, dim, nodes, rank, size);
    double end_comm_time = MPI_Wtime();
    double communication_time = end_comm_time - start_comm_time;

    // Time after communication, before Dijkstra
    double start_dijkstra_time = MPI_Wtime();
    // Perform dijkstra algorithm for every (portioned) graph
    if (rank == 0)
        printf("Find path...\n\n");
    Dijkstra(graph, dist, prev, dim, nodes, node, rank, size);
    double end_dijkstra_time = MPI_Wtime();
    double dijkstra_time = end_dijkstra_time - start_dijkstra_time;

    // Print graph and results, set maximum dimension to print graphs to 15
    const int max_print_dim = 15;
    if (rank == 0 && dim <= max_print_dim) {
        PrintGraph(graph, dim);
        PrintResults(dist, prev, dim, node);
    }
    // Print infos and timers
    if (rank == 0) {
        double total_time = MPI_Wtime() - start_time;
        double T_w_com = total_time; // Thời gian chạy thuật toán tính cả thời gian truyền thông
        double T_wo_com = total_time - communication_time; // Thời gian chạy thuật toán không tính thời gian truyền thông

        printf("Dimension:  %u\n", dim);
        printf("Start node: %u\n", node);
        printf("Processes:  %d\n\n", size);
        printf("Init.:      %lfs\n", init_time);
        printf("Dijkstra:   %lfs\n", dijkstra_time);
        printf("Communication: %lfs\n", communication_time);
        printf("Total (with communication): %lfs\n", T_w_com);
        printf("Total (without communication): %lfs\n", T_wo_com);

        // Write results to file
        WriteResultsToFile("results_output.txt", dist, prev, dim, node);
        // Append results to CSV file
        AppendResultsToCSV("results.csv", dim, T_w_com, T_wo_com);
    }

    // Finish program
    free(graph[0]);
    free(graph);
    free(dist);
    free(prev);
    MPI_Finalize();
}

