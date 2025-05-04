#include <mpi.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

// Function to check if a number is prime
bool isPrime(int num) {
    if (num <= 1)              // Numbers <= 1 are not prime
        return false;
    if (num == 2)              // 2 is the only even prime
        return true;
    if (num % 2 == 0)          // Eliminate even numbers greater than 2
        return false;

    int root = (int)sqrt(num); // Limit checking up to square root of num
    for (int i = 3; i <= root; i += 2) {
        if (num % i == 0)      // If divisible by any odd number up to root, not prime
            return false;
    }
    return true;
}

// Counts prime numbers in a given range [start, end]
int CountPrimes(int start, int end) {
    int count = 0;
    for (int i = start; i <= end; ++i) {
        if (isPrime(i)) {
            count++;          // Increment count if the number is prime
        }
    }
    return count;
}

int main(int argc, char **argv) {
    int size, rank, x, y;

    MPI_Init(&argc, &argv);                    // Initialize the MPI environment
    MPI_Comm_size(MPI_COMM_WORLD, &size);      // Get total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // Get the current process's rank

    // Only the root process (rank 0) takes input from the user
    if (rank == 0) {
        printf("Enter lower bound x:\n");
        scanf("%d", &x);
        printf("Enter upper bound y:\n");
        scanf("%d", &y);
    }

    // Broadcast x and y to all processes from rank 0
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate the total number of integers to process
    int range = y - x + 1;
    int chunk = range / size;         // Basic chunk size each rank should process
    int rem = range % size;           // Remaining values to distribute across first 'rem' ranks

    /*
     * Calculate the starting index for each rank:
     * - Each rank handles 'chunk' numbers
     * - The first 'rem' ranks get 1 extra number to balance leftover work
     * - Add 'rank' if it's one of the first 'rem' ranks (to reflect extra numbers already handed out)
     * - Add 'rem' if rank >= rem to skip past all the extras already handed out
     */
    // if rank < rem => add rank , that which add for previous ranks ,
    // if rank > rem add rem , that which already added
    int start = x + rank * chunk + (rank < rem ? rank : rem);

    // Calculate end of this rank's range
    int end = start + chunk - 1;
    if (rank < rem)                   // Give an extra element to the first 'rem' ranks
        end++;

    // Record start time for timing execution
    double start_time = MPI_Wtime();

    // Each process counts primes in its sub-range
    int local_count = CountPrimes(start, end);

    int total_count = 0;

    // Reduce all local counts into total count on the root process (rank 0)
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Record end time
    double end_time = MPI_Wtime();

    // Display the result and execution time on root process
    if (rank == 0) {
        printf("Total primes between %d and %d = %d\n", x, y, total_count);
        printf("Execution time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();     // Clean up the MPI environment
    return 0;
}
