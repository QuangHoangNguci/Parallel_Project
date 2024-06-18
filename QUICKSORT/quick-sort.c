#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Function to generate input to input.txt
void genInput()
{
    FILE *file;
    int num_elements = 10000;
    int i;
    int random_number;

    // Open the file in write mode, which will clear the file if it exists
    file = fopen("input.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Generate random numbers and write to file
    for (i = 0; i < num_elements; i++)
    {
        random_number = rand();
        fprintf(file, "%d ", random_number);
    }

    // Close the file
    fclose(file);

    printf("Input file generated successfully.\n");
}

// Function to append the sorted result to result.csv
void appendResultToCsv(int* arr, int size, double time_taken)
{
    FILE *file;

    // Open the CSV file in append mode
    file = fopen("result.csv", "a");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    // Get the current timestamp
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    // Write the current date and time
    fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d, ", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

    // Write the time taken for sorting
    fprintf(file, "%f, ", time_taken);

    // Write the sorted array to the CSV file
    for (int i = 0; i < size; i++)
    {
        fprintf(file, "%d", arr[i]);
        if (i < size - 1)
        {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "\n");

    // Close the file
    fclose(file);

    printf("Result appended to result.csv successfully.\n");
}

// Function to swap two numbers
void swap(int* arr, int i, int j)
{
    int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}

// Function that performs the Quick Sort
// for an array arr[] starting from the
// index start and ending at index end
void quicksort(int* arr, int start, int end)
{
    int pivot, index;

    // Base Case
    if (end <= 1)
        return;

    // Pick pivot and swap with first
    // element Pivot is middle element
    pivot = arr[start + end / 2];
    swap(arr, start, start + end / 2);

    // Partitioning Steps
    index = start;

    // Iterate over the range [start, end]
    for (int i = start + 1; i < start + end; i++) {

        // Swap if the element is less
        // than the pivot element
        if (arr[i] < pivot) {
            index++;
            swap(arr, i, index);
        }
    }

    // Swap the pivot into place
    swap(arr, start, index);

    // Recursive Call for sorting
    // of quick sort function
    quicksort(arr, start, index - start);
    quicksort(arr, index + 1, start + end - index - 1);
}

// Function that merges the two arrays
int* merge(int* arr1, int n1, int* arr2, int n2)
{
    int* result = (int*)malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;

    for (k = 0; k < n1 + n2; k++) {
        if (i >= n1) {
            result[k] = arr2[j];
            j++;
        }
        else if (j >= n2) {
            result[k] = arr1[i];
            i++;
        }

        // Indices in bounds as i < n1
        // && j < n2
        else if (arr1[i] < arr2[j]) {
            result[k] = arr1[i];
            i++;
        }

        // v2[j] <= v1[i]
        else {
            result[k] = arr2[j];
            j++;
        }
    }
    return result;
}

// Driver Code
int main(int argc, char* argv[])
{
    int number_of_elements;
    int* data = NULL;
    int chunk_size, own_chunk_size;
    int* chunk;
    FILE* file = NULL;
    double total_time, comm_time, sort_time;
    MPI_Status status;

    if (argc != 3) {
        printf("Desired number of arguments are not their "
               "in argv....\n");
        printf("2 files required first one input and "
               "second one output....\n");
        exit(-1);
    }

    genInput();
    
    int number_of_process, rank_of_process;
    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS) {
        printf("Error in creating MPI "
               "program.\n "
               "Terminating......\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);

    total_time = MPI_Wtime();

    if (rank_of_process == 0) {
        // Opening the file
        file = fopen(argv[1], "r");

        // Printing Error message if any
        if (file == NULL) {
            printf("Error in opening file\n");
            exit(-1);
        }

        // Reading number of Elements in file ...
        // First Value in file is number of Elements
        printf(
            "Reading number of Elements From file ....\n");
        fscanf(file, "%d", &number_of_elements);
        printf("Number of Elements in the file is %d \n",
               number_of_elements);

        // Computing chunk size
        chunk_size
            = (number_of_elements % number_of_process == 0)
                  ? (number_of_elements / number_of_process)
                  : (number_of_elements / number_of_process
                     - 1);

        data = (int*)malloc(number_of_process * chunk_size
                            * sizeof(int));

        // Reading the rest elements in which
        // operation is being performed
        printf("Reading the array from the file.......\n");
        for (int i = 0; i < number_of_elements; i++) {
            fscanf(file, "%d", &data[i]);
        }

        // Padding data with zero
        for (int i = number_of_elements;
             i < number_of_process * chunk_size; i++) {
            data[i] = 0;
        }

        // Printing the array read from file
        printf("Elements in the array is : \n");
        for (int i = 0; i < number_of_elements; i++) {
            printf("%d ", data[i]);
        }

        printf("\n");

        fclose(file);
        file = NULL;
    }

    // Blocks all process until reach this point
    MPI_Barrier(MPI_COMM_WORLD);

    comm_time = MPI_Wtime();

    // BroadCast the Size to all the
    // process from root process
    MPI_Bcast(&number_of_elements, 1, MPI_INT, 0,
              MPI_COMM_WORLD);

    // Computing chunk size
    chunk_size
        = (number_of_elements % number_of_process == 0)
              ? (number_of_elements / number_of_process)
              : number_of_elements
                    / (number_of_process - 1);

    // Calculating total size of chunk
    // according to bits
    chunk = (int*)malloc(chunk_size * sizeof(int));

    // Scatter the chuck size data to all process
    MPI_Scatter(data, chunk_size, MPI_INT, chunk,
                chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    free(data);
    data = NULL;

    comm_time = MPI_Wtime() - comm_time;

    sort_time = MPI_Wtime();

    // Compute size of own chunk and
    // then sort them
    // using quick sort

    own_chunk_size = (number_of_elements
                      >= chunk_size * (rank_of_process + 1))
                         ? chunk_size
                         : (number_of_elements
                            - chunk_size * rank_of_process);

    // Sorting array with quick sort for every
    // chunk as called by process
    quicksort(chunk, 0, own_chunk_size);

    for (int step = 1; step < number_of_process;
         step = 2 * step) {
        if (rank_of_process % (2 * step) != 0) {
            MPI_Send(chunk, own_chunk_size, MPI_INT,
                     rank_of_process - step, 0,
                     MPI_COMM_WORLD);
            break;
        }

        if (rank_of_process + step < number_of_process) {
            int received_chunk_size
                = (number_of_elements
                   >= chunk_size
                          * (rank_of_process + 2 * step))
                      ? (chunk_size * step)
                      : (number_of_elements
                         - chunk_size
                               * (rank_of_process + step));
            int* chunk_received;
            chunk_received = (int*)malloc(
                received_chunk_size * sizeof(int));
            MPI_Recv(chunk_received, received_chunk_size,
                     MPI_INT, rank_of_process + step, 0,
                     MPI_COMM_WORLD, &status);

            data = merge(chunk, own_chunk_size,
                         chunk_received,
                         received_chunk_size);

            free(chunk);
            free(chunk_received);
            chunk = data;
            own_chunk_size
                = own_chunk_size + received_chunk_size;
        }
    }

    sort_time = MPI_Wtime() - sort_time;

    total_time = MPI_Wtime() - total_time;

    // Opening the other file as taken form input
    // and writing it to the file and giving it
    // as the output
    if (rank_of_process == 0) {
        // Opening the file
        file = fopen(argv[2], "w");

        if (file == NULL) {
            printf("Error in opening file... \n");
            exit(-1);
        }

        // Printing total number of elements
        // in the file
        fprintf(
            file,
            "Total number of Elements in the array : %d\n",
            own_chunk_size);

        // Printing the value of array in the file
        for (int i = 0; i < own_chunk_size; i++) {
            fprintf(file, "%d ", chunk[i]);
        }

        // Closing the file
        fclose(file);

        printf("\n\n\n\nResult printed in output.txt file "
               "and shown below: \n");

        // For Printing in the terminal
        printf("Total number of Elements given as input : "
               "%d\n",
               number_of_elements);
        printf("Sorted array is: \n");

        for (int i = 0; i < number_of_elements; i++) {
            printf("%d ", chunk[i]);
        }

        printf(
            "\n\nQuicksort %d ints on %d procs: %f secs\n",
            number_of_elements, number_of_process,
            total_time);

        printf("Time spent in communication: %f secs\n", comm_time);
        printf("Time spent in sorting: %f secs\n", sort_time);

        // Append the sorted result to CSV
        appendResultToCsv(chunk, own_chunk_size, total_time);
    }

    MPI_Finalize();
    return 0;
}
