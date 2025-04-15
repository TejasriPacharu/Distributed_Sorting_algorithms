#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

// Structure to represent a processor node in the line network
typedef struct Node {
    int value;                  // Value stored in the processor
    int index;                  // Position in the line
    struct Node* left;          // Pointer to left neighbor
    struct Node* right;         // Pointer to right neighbor
    pthread_mutex_t lock;       // Lock for thread safety
} Node;

// Structure to hold arguments for thread function
typedef struct {
    Node* node;                 // Node to process
    int phase;                  // Current phase (odd or even)
} ThreadArgs;

// Initialize the network with random values
Node* initNetwork(int size) {
    // Seed the random number generator
    srand(time(NULL));

    Node* root = NULL;
    Node* prev = NULL;

    // Create linked list of processors
    for (int i = 0; i < size; i++) {
        Node* node = (Node*)malloc(sizeof(Node));

        // Initialize node properties
        node->value = rand() % 1000;  // Random value between 0 and 999
        node->index = i;
        node->left = prev;
        node->right = NULL;
        pthread_mutex_init(&node->lock, NULL);

        if (prev != NULL) {
            prev->right = node;
        } else {
            root = node;  // First node becomes root
        }

        prev = node;
    }

    return root;
}

// Initialize the network with provided values
Node* initNetworkWithValues(int arr[], int size) {
    Node* root = NULL;
    Node* prev = NULL;

    // Create linked list of processors
    for (int i = 0; i < size; i++) {
        Node* node = (Node*)malloc(sizeof(Node));

        // Initialize node properties
        node->value = arr[i];
        node->index = i;
        node->left = prev;
        node->right = NULL;
        pthread_mutex_init(&node->lock, NULL);

        if (prev != NULL) {
            prev->right = node;
        } else {
            root = node;  // First node becomes root
        }

        prev = node;
    }

    return root;
}

// Print the values in the network
void printNetwork(Node* root) {
    Node* temp = root;
    printf("Network values: ");
    while (temp != NULL) {
        printf("%d ", temp->value);
        temp = temp->right;
    }
    printf("\n");
}

// Check if the network is sorted
int isSorted(Node* root) {
    if (root == NULL) return 1;  // Empty list is sorted

    Node* current = root;
    while (current->right != NULL) {
        if (current->value > current->right->value) {
            return 0;  // Not sorted
        }
        current = current->right;
    }

    return 1;  // Sorted
}

// Clean up the network resources
void cleanupNetwork(Node* root) {
    Node* current = root;

    while (current != NULL) {
        Node* next = current->right;
        pthread_mutex_destroy(&current->lock);
        free(current);
        current = next;
    }
}

// Compare and swap values between two adjacent nodes if needed
void compareAndSwapNodes(Node* left, Node* right) {
    if (left == NULL || right == NULL) return;

    // Lock both nodes to prevent race conditions
    pthread_mutex_lock(&left->lock);
    pthread_mutex_lock(&right->lock);

    // Compare and swap if needed
    if (left->value > right->value) {
        int temp = left->value;
        left->value = right->value;
        right->value = temp;
    }

    // Unlock nodes
    pthread_mutex_unlock(&right->lock);
    pthread_mutex_unlock(&left->lock);
}

// Thread function to perform comparison and swapping
void* compareAndSwapThread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Node* node = args->node;
    int phase = args->phase;

    // Odd phase: process nodes with odd indices
    // Even phase: process nodes with even indices
    if ((node->index % 2 == 1 && phase == 1) || (node->index % 2 == 0 && phase == 0)) {
        // If there's a right neighbor, compare and potentially swap
        if (node->right != NULL) {
            compareAndSwapNodes(node, node->right);
        }
    }

    return NULL;
}

// Odd-Even Transposition Sort Algorithm with threads
void oddEvenSort(Node* root, int size) {
    printf("Starting Odd-Even Transposition Sort with Threads...\n");

    int sorted = 0;
    int phase_count = 0;
    pthread_t* threads = (pthread_t*)malloc(size * sizeof(pthread_t));
    ThreadArgs* args = (ThreadArgs*)malloc(size * sizeof(ThreadArgs));

    // Continue until the array is sorted
    while (!sorted) {
        sorted = 1;  // Assume the array is sorted

        // Process both odd and even phases
        for (int phase = 0; phase <= 1; phase++) {
            Node* current = root;
            int thread_idx = 0;

            // Create threads for each node
            while (current != NULL) {
                args[thread_idx].node = current;
                args[thread_idx].phase = phase;

                pthread_create(&threads[thread_idx], NULL, compareAndSwapThread, &args[thread_idx]);

                current = current->right;
                thread_idx++;
            }

            // Wait for all threads to complete
            for (int i = 0; i < thread_idx; i++) {
                pthread_join(threads[i], NULL);
            }

            phase_count++;

            // Check if still sorted after this phase
            if (!isSorted(root)) {
                sorted = 0;
            }

            printf("After phase %d: ", phase_count);
            printNetwork(root);
        }

        // Ensure we don't get stuck in an infinite loop
        if (phase_count >= 2 * size) {
            printf("Maximum number of phases reached.\n");
            break;
        }
    }

    printf("Sorting completed in %d phases.\n", phase_count);

    free(threads);
    free(args);
}

// Main function to test Odd-Even Transposition Sort
int main(int argc, char* argv[]) {
    int size = 10;  // Default size

    // Check if size is provided as command line argument
    if (argc > 1) {
        size = atoi(argv[1]);
    }

    // Create a network
    Node* root = initNetwork(size);

    // Print the initial state
    printf("Initial state (n=%d):\n", size);
    printNetwork(root);

    // Execute Odd-Even Transposition Sort
    clock_t start = clock();
    oddEvenSort(root, size);
    clock_t end = clock();

    // Check if sorted correctly
    if (isSorted(root)) {
        printf("Network is correctly sorted.\n");
    } else {
        printf("ERROR: Network is not sorted correctly!\n");
    }

    // Print execution time
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", cpu_time_used);

    // Clean up
    cleanupNetwork(root);

    return 0;
}
