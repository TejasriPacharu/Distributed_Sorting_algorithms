# Distributed Sorting Algorithms on a Line Network

This project implements three distributed sorting algorithms on a line network using a discrete event simulator:

1. Odd-Even Transposition Algorithm (`odd_even_transposition_sort.c`)
2. Sasaki's Time-Optimal Algorithm (`sasaki_time_optimal_sort.c`)
3. Alternative Time-Optimal Algorithm (`alternate_time_optimal_sort.c`)

## Compilation Instructions

To compile all three programs, you can use the following commands:

```bash
gcc -o odd_even_transposition_sort odd_even_transposition_sort.c -lm
gcc -o sasaki_time_optimal_sort sasaki_time_optimal_sort.c -lm
gcc -o alternate_time_optimal_sort alternate_time_optimal_sort.c -lm
```

Note: The `-lm` flag is needed to link the math library (for log2 function in the alternative algorithm).

## Running the Programs

Each program can be run with an optional command-line argument to specify the number of processors - 10, 20, 30, 50 (default is 10):

```bash
./odd_even_transposition_sort [number_of_processors]
./sasaki_time_optimal_sort  [number_of_processors]
./alternate_time_optimal_sort [number_of_processors]
```

Example:
```bash
./odd_even_sort 20  # Run odd-even sort with 20 processors
```

## Understanding the Output

Each program will output:
1. The initial state of the network
2. Step-by-step values after each phase/iteration
3. Whether the network is correctly sorted at the end
4. The execution time in seconds

## Testing with Different Sizes

As required, you can test with various sizes (n = 10, 20, 30, 50):

```bash
./odd_even_transposition_sort 10
./odd_even_transposition_sort 20
./odd_even_transposition_sort 30
./odd_even_transposition_sort 50

./sasaki_time_optimal_sort 10
./sasaki_time_optimal_sort 20
./sasaki_time_optimal_sort 30
./sasaki_time_optimal_sort 50

./alternate_time_optimal_sort 10
./alternate_time_optimal_sort 20
./alternate_time_optimal_sort 30
./alternate_time_optimal_sort 50
```

## Results

Here's a comparison table showing the execution times and number of phases for different algorithms with varying numbers of processors:

| Algorithm                | n=10      | n=20      | n=30      | n=50      |
|--------------------------|---------- |---------- |---------- |---------- |
| Odd-Even Transposition   | 0.007977s | 0.029694s | 0.062100s | 0.089038s |
| Sasaki's Algorithm       | 0.000143s | 0.000291s | 0.000537s | 0.001142s |
| Alternative Algorithm    | 0.001476s | 0.012928s | 0.022273s | 0.052802s |

## Time and Space Complexities

| Algorithm                | Data Structure              | Time Complexity | Space Complexity  |
|--------------------------|-----------------------------|-----------------|-------------------|
| Odd-Even Transposition   | Linked List with Threads    | O(n²)           | O(n)              |
| Sasaki's Algorithm       | Linked List with Elements   | O(n)            | O(n)              |
| Alternative Algorithm    | Array with Threads          | O(n log n)      | O(n)              |
