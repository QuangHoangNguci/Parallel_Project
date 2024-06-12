#include "create_graph.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "constants.h"

// Create a test graph
void TestGraph(unsigned **graph, unsigned dim) {
    unsigned tmp_graph[][5] = {{  0,   4,   2, INF, INF},
                               {INF,   0,   3,   2,   3},
                               {INF,   1,   0,   4,   5},
                               {INF, INF, INF,   0, INF},
                               {INF, INF, INF,   1,   0}};
    for (int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
            graph[i][j] = tmp_graph[i][j];
}

// Create a random undirected graph (not parallelized)
void RandomGraph(unsigned **graph, unsigned dim) {
    srand(time(NULL));
    // Maximum length between nodes
    const unsigned max_length = 100;
    // Amount of INFs to generate (unconnected nodes)
    const double inf_amount = 0.5;
    for (int i = 0; i < dim; i++) {
        // Diagonal elements will always be zero (no distance to itself)
        graph[i][i] = 0;
        for (int j = i + 1; j < dim; j++) {
            unsigned tmp_rand = rand() % (int)(max_length / inf_amount) + 1;
            if (tmp_rand < max_length)
                graph[i][j] = tmp_rand;
            else
                graph[i][j] = INF;
            graph[j][i] = graph[i][j];
        }
    }
}

// Function to write the graph to a file
void WriteGraphToFile(unsigned **graph, unsigned dim, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    for (unsigned i = 0; i < dim; i++) {
        for (unsigned j = 0; j < dim; j++) {
            if (graph[i][j] == INF)
                fprintf(file, "INF\t");
            else
                fprintf(file, "%u\t", graph[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

