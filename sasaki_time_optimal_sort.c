#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

// Structure to represent an element with its value and marking
typedef struct {
    int value;          // Value stored in the element
    bool isMarked;      // Whether the element is marked (true for original elements)
} Element;

// Structure to represent a processor in the line network
typedef struct Node {
    Element* lValue;    // Left value (sending buffer)
    Element* rValue;    // Right value (receiving buffer)
    int area;           // Area value (used for routing)
    struct Node* left;  // Pointer to left neighbor
    struct Node* right; // Pointer to right neighbor
} Node;

// Initialize the network with random values
Node* initNetwork(int size) {
    // Seed the random number generator
    srand(time(NULL));

    Node* root = NULL;
    Node* prev = NULL;

    // Create linked list of processors
    for (int i = 0; i < size; i++) {
        Node* node = (Node*)malloc(sizeof(Node));

        if (i == 0) {
            // First node - set left value to INT_MIN (sentinel)
            Element* lElement = (Element*)malloc(sizeof(Element));
            lElement->value = INT_MIN;
            lElement->isMarked = false;

            Element* rElement = (Element*)malloc(sizeof(Element));
            rElement->value = rand() % 1000;  // Random value between 0 and 999
            rElement->isMarked = true;

            node->lValue = lElement;
            node->rValue = rElement;
            node->area = -1;
            root = node;
        }
        else if (i == size - 1) {
            // Last node - set right value to INT_MAX (sentinel)
            Element* lElement = (Element*)malloc(sizeof(Element));
            lElement->value = rand() % 1000;  // Random value between 0 and 999
            lElement->isMarked = true;

            Element* rElement = (Element*)malloc(sizeof(Element));
            rElement->value = INT_MAX;
            rElement->isMarked = false;

            node->lValue = lElement;
            node->rValue = rElement;
            node->area = 0;
        }
        else {
            // Middle nodes
            Element* lElement = (Element*)malloc(sizeof(Element));
            lElement->value = rand() % 1000;  // Random value between 0 and 999
            lElement->isMarked = true;

            Element* rElement = (Element*)malloc(sizeof(Element));
            rElement->value = rand() % 1000;  // Random value between 0 and 999
            rElement->isMarked = true;

            node->lValue = lElement;
            node->rValue = rElement;
            node->area = 0;
        }

        // Set up links
        node->left = prev;
        node->right = NULL;

        if (prev != NULL) {
            prev->right = node;
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

        if (i == 0) {
            // First node - set left value to INT_MIN (sentinel)
            Element* lElement = (Element*)malloc(sizeof(Element));
            lElement->value = INT_MIN;
            lElement->isMarked = false;

            Element* rElement = (Element*)malloc(sizeof(Element));
            rElement->value = arr[i];
            rElement->isMarked = true;

            node->lValue = lElement;
            node->rValue = rElement;
            node->area = -1;
            root = node;
        }
        else if (i == size - 1) {
            // Last node - set right value to INT_MAX (sentinel)
            Element* lElement = (Element*)malloc(sizeof(Element));
            lElement->value = arr[i-1];
            lElement->isMarked = true;

            Element* rElement = (Element*)malloc(sizeof(Element));
            rElement->value = INT_MAX;
            rElement->isMarked = false;

            node->lValue = lElement;
            node->rValue = rElement;
            node->area = 0;
        }
        else {
            // Middle nodes
            Element* lElement = (Element*)malloc(sizeof(Element));
            lElement->value = arr[i-1];
            lElement->isMarked = true;

            Element* rElement = (Element*)malloc(sizeof(Element));
            rElement->value = arr[i];
            rElement->isMarked = true;

            node->lValue = lElement;
            node->rValue = rElement;
            node->area = 0;
        }

        // Set up links
        node->left = prev;
        node->right = NULL;

        if (prev != NULL) {
            prev->right = node;
        }

        prev = node;
    }

    return root;
}

// Print the network (for debugging)
void printNetworkDebug(Node* root) {
    Node* temp = root;
    printf("Network debug: \n");
    while (temp != NULL) {
        printf("L:%d(%d) - R:%d(%d) | Area: %d\n",
            temp->lValue->value, temp->lValue->isMarked,
            temp->rValue->value, temp->rValue->isMarked,
            temp->area);
        temp = temp->right;
    }
    printf("\n");
}

// Print the values in the network (in order according to area rule)
void printNetwork(Node* root) {
    Node* temp = root;
    printf("Network values: ");
    while (temp != NULL) {
        if (temp->area == -1) {
            printf("%d ", temp->rValue->value);
        } else {
            printf("%d ", temp->lValue->value);
        }
        temp = temp->right;
    }
    printf("\n");
}

// Check if the network is sorted
int isSorted(Node* root) {
    int prev_value = INT_MIN;
    Node* temp = root;

    while (temp != NULL) {
        int current_value;
        if (temp->area == -1) {
            current_value = temp->rValue->value;
        } else {
            current_value = temp->lValue->value;
        }

        if (current_value != INT_MAX && current_value != INT_MIN) {
            if (current_value < prev_value) {
                return 0;  // Not sorted
            }
            prev_value = current_value;
        }

        temp = temp->right;
    }

    return 1;  // Sorted
}

// Compare and potentially swap elements in a node
void compareAndSwap(Node* node) {
    // First, check left neighbor connection if exists
    if (node->left != NULL) {
        if (node->left->rValue->value > node->lValue->value) {
            // Update areas if marked elements are moving
            if (node->left->rValue->isMarked) {
                node->area--;
            }
            if (node->lValue->isMarked) {
                node->area++;
            }

            // Swap elements
            Element* temp = node->left->rValue;
            node->left->rValue = node->lValue;
            node->lValue = temp;
        }
    }

    // Then, check right neighbor connection if exists
    if (node->right != NULL) {
        if (node->right->lValue->value < node->rValue->value) {
            // Update areas if marked elements are moving
            if (node->right->lValue->isMarked) {
                node->right->area++;
            }
            if (node->rValue->isMarked) {
                node->right->area--;
            }

            // Swap elements
            Element* temp = node->right->lValue;
            node->right->lValue = node->rValue;
            node->rValue = temp;
        }
    }

    // Finally, compare left and right values within the node
    if (node->lValue->value > node->rValue->value) {
        // Swap values
        Element* temp = node->lValue;
        node->lValue = node->rValue;
        node->rValue = temp;
    }
}

// Clean up the network resources
void cleanupNetwork(Node* root) {
    Node* current = root;

    while (current != NULL) {
        Node* next = current->right;

        // Free elements
        free(current->lValue);
        free(current->rValue);

        // Free node
        free(current);

        current = next;
    }
}

// Sasaki's Time-Optimal Sorting Algorithm
void sasakiSort(Node* root, int size) {
    printf("Starting Sasaki's Time-Optimal Sorting Algorithm...\n");

    // Print initial network state
    printf("Initial state: ");
    printNetwork(root);

    // Execute n-1 rounds of the algorithm
    for (int round = 1; round < size; round++) {
        printf("Round %d: ", round);

        // Process each node
        Node* current = root;
        while (current != NULL) {
            compareAndSwap(current);
            current = current->right;
        }

        // Print network after this round
        printNetwork(root);
    }

    printf("Sorting completed.\n");
}

// Main function to test Sasaki's Time-Optimal Sort
int main(int argc, char* argv[]) {
    int size = 10;  // Default size

    // Check if size is provided as command line argument
    if (argc > 1) {
        size = atoi(argv[1]);
    }

    // Create a network
    Node* root = initNetwork(size);

    // Execute Sasaki's algorithm
    clock_t start = clock();
    sasakiSort(root, size);
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
