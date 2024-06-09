#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

void PrintGraph(unsigned **graph, unsigned dim);
void PrintResults(unsigned *dist, unsigned *prev, unsigned dim, unsigned node);
void WriteResultsToFile(const char *filename, unsigned *dist, unsigned *prev, unsigned dim, unsigned start_node);

#endif

