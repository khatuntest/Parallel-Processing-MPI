/*
 * Parallel Prime Number Counter using MPI
 *
 * This program counts prime numbers between a given range [x, y] using MPI for parallel processing.
 * The work is distributed among multiple processes to improve performance.
 *
 * Key Features:
 * - Master-worker model (rank 0 is master, others are workers)
 * - Dynamic workload distribution
 * - Prime checking with optimizations
 * - Timing measurement
 */
#include <mpi.h>       // MPI library for parallel processing
#include <stdbool.h>   // For boolean type
#include <math.h>      // For sqrt() function
#include <stdio.h>     // For I/O operations
/*
 * Function: isPrime
 * -----------------
 * Checks if a number is prime using trial division with optimizations.
 *
 * Parameters:
 *   num - The number to check for primality
 *
 * Returns:
 *   bool - true if the number is prime, false otherwise
 */
bool isPrime(int num) {
    if (num <= 1)              // Numbers <= 1 are not prime
        return false;
    if (num == 2)              // 2 is the only even prime
        return true;
    if (num % 2 == 0)          // Eliminate even numbers > 2
        return false;

    int root = (int)sqrt(num); // Only check up to square root
    for (int i = 3; i <= root; i += 2) {
        if (num % i == 0)      // If divisible, not prime
            return false;
    }
    return true;
}

/*
 * Function: CountPrimes
 * --------------------
 * Counts prime numbers in a given range [start, end].
 *
 * Parameters:
 *   start - Lower bound of range (inclusive)
 *   end   - Upper bound of range (inclusive)
 *
 * Returns:
 *   int - Count of prime numbers in the range
 */
int CountPrimes(int start, int end) {
    int count = 0;
    for (int i = start; i <= end; ++i) {
        if (isPrime(i)) {
            count++;          // Increment count for each prime found
        }
    }
    return count;
}

/*
 * Function: main
 * --------------
 * Main program entry point using MPI for parallel processing.
 *
 * Workflow:
 * 1. MPI initialization
 * 2. Master process (rank 0):
 *    - Gets input range from user
 *    - Divides work among workers
 *    - Collects and aggregates results
 *    - Measures and reports execution time
 * 3. Worker processes:
 *    - Receive work range from master
 *    - Count primes in assigned range
 *    - Send results back to master
 * 4. MPI cleanup
 */
int main(int argc, char **argv) {
    int size, rank, x, y;
    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    // Get total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Get current process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int rem, chunk;
    // Master process (rank 0)
    if (rank == 0) {
        // Get input range from user
        printf("Enter lower bound x:\n");
        scanf("%d", &x);
        printf("Enter upper bound y:\n");
        scanf("%d", &y);
        int range = y - x + 1;
        // Calculate base chunk size for each worker
        chunk = range / (size - 1);
        // Calculate remainder for uneven distribution
        rem = range % (size - 1);
        // Start timing
        double start_time = MPI_Wtime();
        int total_count = 0;
        int cur = x;  // Current starting position
        // Distribute work to workers
        for (int i = 1; i < size; ++i) {
            // Give extra number to first 'rem' workers if range doesn't divide evenly
            int cur_chunk = chunk + (i <= rem ? 1 : 0);
            int start = cur;
            int end = cur + cur_chunk - 1;
            // Send work range to worker i
            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            cur = end + 1;  // Update current position
        }
        // Collect results from workers
        for (int i = 1; i < size; ++i) {
            int cur_count;
            MPI_Recv(&cur_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += cur_count;
        }
        // Stop timing and print results
        double end_time = MPI_Wtime();
        printf("Total primes between %d and %d = %d\n", x, y, total_count);
        printf("Execution time: %f seconds\n", end_time - start_time);
    }
        // Worker processes (rank > 0)
    else {
        int start, end;
        // Receive work range from master
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Count primes in assigned range
        int Primes = CountPrimes(start, end);
        // Send result back to master
        MPI_Send(&Primes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    // Clean up MPI environment
    MPI_Finalize();
    return 0;
}