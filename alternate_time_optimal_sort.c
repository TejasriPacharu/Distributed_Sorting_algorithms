#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

// Structure to represent a processor in the line network
typedef struct {
    int value;              // Value stored in the processor
    int index;              // Position in the line
    int* sendBuffer;        // Buffer for sending values
    int* recvBuffer;        // Buffer for receiving values
} Processor;

// Structure to represent the network
typedef struct {
    Processor* processors;   // Array of processors
    int size;                // Number of processors
} Network;

// Structure for thread arguments
typedef struct {
    Network* network;       // The network
    int center;             // Center position for comparison
} ThreadArgs;

// Initialize the network with random values
void initNetwork(Network* network, int size) {
    network->size = size;
    network->processors = (Processor*)malloc(size * sizeof(Processor));

    // Seed the random number generator
    srand(time(NULL));

    // Initialize each processor
    for (int i = 0; i < size; i++) {
        network->processors[i].value = rand() % 1000;  // Random value between 0 and 999
        network->processors[i].index = i;
        network->processors[i].sendBuffer = (int*)malloc(sizeof(int));
        network->processors[i].recvBuffer = (int*)malloc(sizeof(int));
    }
}

// Print the values in the network
void printNetwork(Network* network) {
    printf("Network values: ");
    for (int i = 0; i < network->size; i++) {
        printf("%d ", network->processors[i].value);
    }
    printf("\n");
}

// Check if the network is sorted
int isSorted(Network* network) {
    for (int i = 1; i < network->size; i++) {
        if (network->processors[i-1].value > network->processors[i].value) {
            return 0;  // Not sorted
        }
    }
    return 1;  // Sorted
}

// Clean up the network resources
void cleanupNetwork(Network* network) {
    for (int i = 0; i < network->size; i++) {
        free(network->processors[i].sendBuffer);
        free(network->processors[i].recvBuffer);
    }
    free(network->processors);
}

// Utility function to find the minimum of two values
int minimum(int a, int b) {
    return (a < b) ? a : b;
}

// Utility function to find the maximum of two values
int maximum(int a, int b) {
    return (a > b) ? a : b;
}

// Thread function for comparison operations
void* compare(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Network* network = args->network;
    int center = args->center;
    int n = network->size;

    // Edge case: left boundary
    if (center == 0) {
        if (network->processors[center].value > network->processors[center + 1].value) {
            // Swap values
            int temp = network->processors[center].value;
            network->processors[center].value = network->processors[center + 1].value;
            network->processors[center + 1].value = temp;
        }
    }
    // Edge case: right boundary
    else if (center == n - 1) {
        if (network->processors[center].value < network->processors[center - 1].value) {
            // Swap values
            int temp = network->processors[center].value;
            network->processors[center].value = network->processors[center - 1].value;
            network->processors[center - 1].value = temp;
        }
    }
    // Non-edge case: sort 3 elements (center and its neighbors)
    else if (center > 0 && center < n - 1) {
        int leftVal = network->processors[center - 1].value;
        int centerVal = network->processors[center].value;
        int rightVal = network->processors[center + 1].value;

        // Find min, max, and middle values
        int minValue = minimum(centerVal, minimum(leftVal, rightVal));
        int maxValue = maximum(centerVal, maximum(leftVal, rightVal));
        int midValue = leftVal + centerVal + rightVal - minValue - maxValue;

        // Place the values in correct positions
        network->processors[center - 1].value = minValue;
        network->processors[center].value = midValue;
        network->processors[center + 1].value = maxValue;
    }

    return NULL;
}

// Alternative Time-Optimal Sort algorithm using threads
void alternativeTimeOptimalSort(Network* network) {
    int n = network->size;
    printf("Starting Alternative Time-Optimal Sort (Thread-based)...\n");

    int maxThreads = n;
    pthread_t* threads = (pthread_t*)malloc(maxThreads * sizeof(pthread_t));
    ThreadArgs* args = (ThreadArgs*)malloc(maxThreads * sizeof(ThreadArgs));

    // For n-1 rounds
    for (int round = 1; round < n; round++) {
        printf("Round %d: ", round);

        // Determine starting position based on round remainder
        int remainder = (round + 1) % 3;
        int startPos;

        if (remainder == 0) {
            startPos = 2;
        } else if (remainder == 1) {
            startPos = 0;
        } else {
            startPos = 1;
        }

        int threadCount = 0;

        // Process centers at distance 3 apart
        for (int center = startPos; center < n; center += 3) {
            args[threadCount].network = network;
            args[threadCount].center = center;

            // Create thread for this center
            pthread_create(&threads[threadCount], NULL, compare, &args[threadCount]);
            threadCount++;
        }

        // Wait for all threads to complete
        for (int i = 0; i < threadCount; i++) {
            pthread_join(threads[i], NULL);
        }

        // Print network state after this round
        printNetwork(network);
    }

    printf("Sorting completed.\n");

    free(threads);
    free(args);
}

// Main function to test Alternative Time-Optimal Sort
int main(int argc, char* argv[]) {
    int size = 10;  // Default size

    // Check if size is provided as command line argument
    if (argc > 1) {
        size = atoi(argv[1]);
    }

    // Create a network
    Network network;
    initNetwork(&network, size);

    // Print the initial state
    printf("Initial state (n=%d):\n", size);
    printNetwork(&network);

    // Execute Alternative Time-Optimal Sort
    clock_t start = clock();
    alternativeTimeOptimalSort(&network);
    clock_t end = clock();

    // Check if sorted correctly
    if (isSorted(&network)) {
        printf("Network is correctly sorted.\n");
    } else {
        printf("ERROR: Network is not sorted correctly!\n");
    }

    // Print execution time
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", cpu_time_used);

    // Clean up
    cleanupNetwork(&network);

    return 0;
}
