# Parallel Processing with MPI - Two Tasks
This repository contains a parallel C program that solves two computational problems using MPI:

### Task 1: Counting Prime Numbers
This task counts the prime numbers within a specified range `[x, y]`. The task is parallelized using MPI, where the master process divides the range among worker processes. The results are aggregated to get the total prime count.

#### Key Concepts:
- **Master process:** Divides the range and aggregates the results from worker processes.
- **Worker processes:** Calculate prime numbers in their assigned subrange and send the result back to the master.

### Task 2: Matrix Summation
This task computes the sum of two matrices, `A` and `B`, and stores the result in matrix `C`. The task is parallelized using MPI to divide the matrix rows among multiple processes for computation, and the results are gathered by the master process.

#### Key Concepts:
- **Master process:** Distributes matrix rows to worker processes and collects the results.
- **Worker processes:** Perform element-wise addition of the assigned rows and send the result back to the master.
