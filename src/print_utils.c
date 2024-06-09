#include "print_utils.h"

#include <stdio.h>
#include <stdlib.h> // Thêm thư viện này

#include "constants.h"

// Print graph in a matrix-like form
void PrintGraph(unsigned **graph, unsigned dim)
{
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            if (graph[i][j] == INF)
                printf("inf\t");
            else
                printf("%u\t", graph[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    return;
}

// Print results from Dijkstra function
void PrintResults(unsigned *dist, unsigned *prev, unsigned dim, unsigned node)
{
    // Print results in following form: Node number | Distance to start node | Path from start node
    printf("Node\tDist.\tPath\n");
    for (int i = 0; i < dim; i++) {
        if (dist[i] == INF)
            printf("%u\tinf\t", i);
        else
            printf("%u\t%u\t", i, dist[i]);
        // Generate path from previous node array
        int tmp_node = prev[i];
        if (tmp_node != INF) {
            if (prev[i] != i) {
                printf("%u <- ", i);
                while (tmp_node != node) {
                    printf("%u <- ", tmp_node);
                    tmp_node = prev[tmp_node];
                }
            }
            printf("start");
        }
        printf("\n");
    }
    printf("\n");
    return;
}

// Write results to a file
void WriteResultsToFile(const char *filename, unsigned *dist, unsigned *prev, unsigned dim, unsigned node)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE); // Sử dụng exit(EXIT_FAILURE)
    }

    fprintf(file, "Node\tDist.\tPath\n");
    for (int i = 0; i < dim; i++) {
        if (dist[i] == INF)
            fprintf(file, "%u\tinf\t", i);
        else
            fprintf(file, "%u\t%u\t", i, dist[i]);
        // Generate path from previous node array
        int tmp_node = prev[i];
        if (tmp_node != INF) {
            if (prev[i] != i) {
                fprintf(file, "%u <- ", i);
                while (tmp_node != node) {
                    fprintf(file, "%u <- ", tmp_node);
                    tmp_node = prev[tmp_node];
                }
            }
            fprintf(file, "start");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

