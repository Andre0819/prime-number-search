# Prime Number Finder
This program finds prime numbers up to a specified SEARCH_LIMIT using multiple threads. The program's configuration can be customized through a config.txt file. It supports two different task division strategies for parallelizing the search process, and the output format can be adjusted based on your needs.

# Requirements
- C++20 or later
- A C++20-compatible compiler (e.g., GCC 10 or later, Clang 11 or later, MSVC 2019 or later)

# Configuration File (config.txt)
The program uses a configuration file named config.txt to read parameters for the prime number search. Below are the valid configuration keys and values you can include in this file.

# Configuration Keys
- PRINT_VARIATION:
	Specifies the output format for the prime numbers.
	- I: Print the prime number along with the thread ID and the timestamp when found.
	- II: Print all primes found after the search completes, showing the prime number and the thread ID.
- TASK_DIVISION:
	Defines how the workload is distributed across threads.
	- I: Each thread is assigned a specific range of numbers to check.
	- II: Each thread checks one number at a time, with multiple threads checking different numbers in parallel.
- NUM_THREADS:
	The number of threads to use for the prime number search. Must be a positive integer greater than zero and less than or equal to the SEARCH_LIMIT.

- SEARCH_LIMIT:
	The maximum number up to which prime numbers will be found. Must be a positive integer and less than or equal to 2147483647 (the maximum value for a 32-bit signed integer).

Example config.txt:
```
PRINT_VARIATION=I
TASK_DIVISION=I
NUM_THREADS=4
SEARCH_LIMIT=100
```
### Notes on Input Validation:
- The program will validate the PRINT_VARIATION and TASK_DIVISION values, ensuring that they are either I or II.
- The SEARCH_LIMIT must be a positive integer, and it must be less than or equal to 2147483647.
- The NUM_THREADS must be a positive integer, and it must be less than or equal to the SEARCH_LIMIT.
- If any of these conditions are violated, the program will print an error message and exit.