
/* 
Monte Carlo algorithm to estimate the value of π.
A circle with radius 1 is inscribed in a square of side 2.

Random points (x, y) are generated in the range [0, 1],
and checked if they lie inside the circle using: x² + y² ≤ 1.

The ratio of points inside the circle to total points
approximates the area ratio: π/4.

Thus, π ≈ 4 * (points_in_circle / total_points).
*/

#include <unistd.h>
#include <stdio.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <time.h>

#define N_THREADS 32
#define TOTAL_POINTS 1<<21

// Struct for armazening number of samples and seed
typedef struct {
	int samples;
	unsigned short seed[3];
} Samples_Seed;

// Thread function
void *calculate_hits(void *arg) {
	Samples_Seed *thread_args = (Samples_Seed *)arg;
	
	// Recived parameters
    int samples = thread_args->samples;
    unsigned short *seed = thread_args->seed;
    
	// Point coordinates
	double x, y;
	
	// Total number of hits (points inside circle)
	long hits = 0;
	
	// Iteration for established number of samples for each thread
	for (long i=0; i < samples; i++) {
		// Generate random numbers between 0 and 1 using the erand48() function
			x = erand48(seed);
			y = erand48(seed);

		// Check if the point's coordinates are inside the circle
		if(x*x+y*y <= 1.0)
			hits++;
	}	
	
	// Allocating result dinamicaly (do not expire when thread ends)
    long *result = malloc(sizeof(long));
    *result = hits;
	
	// Thread return result
	pthread_exit(result);
}

int
main(int argc, char **argv) 
{ 
	int samples;
	int num_threads;
	int status;

	if(argc > 1)
		samples = atoi(argv[1]);
	else
		samples = TOTAL_POINTS;

	// Passing number of threads as parameter of main function
	if(argc > 2) {
		num_threads = atoi(argv[2]);
		if(num_threads <= 0) {
			printf("Invalid number of threads...\n");
			return(EXIT_FAILURE);
		}
	}
	else
		num_threads = N_THREADS;

	// Vector of pthread_t for saving the thread identifiers
	pthread_t *threads; 
	
	// Allocates memory for thread vector IDs in execution time
	threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));	

	// Creating different seeds for each thread, garants different sequence of random numbers
	unsigned int base_seed = (unsigned int)time(NULL);
	unsigned short seeds[num_threads][3];
	
    for (int i = 0; i < num_threads; i++) {
		
		// Data to pass through each thread: number of samples and seed
		Samples_Seed *thread_args = malloc(sizeof(Samples_Seed));
		thread_args->samples = samples;
		
        // Derivie different seeds based on base_seed
        thread_args->seed[0] = (base_seed + i) & 0xFFFF;
		thread_args->seed[1] = ((base_seed + i) >> 8) & 0xFFFF;
		thread_args->seed[2] = ((base_seed + i) >> 16) & 0xFFFF;
        
		// Calling threads with unique seed
        status = pthread_create(&threads[i], NULL, calculate_hits, thread_args);
        
        if (status) {
			printf("Failed in creation of thread %d (%d)\n", i, status);
			return (EXIT_FAILURE);
		}
    }
    
	long long total_hits = 0;
	
	// Loop waiting for all threads to finish execution
	for (int i=0; i < num_threads; i++) {
		
		void *ret_val;
		
		// Join reciving partial sum of each thread
		status = pthread_join(threads[i], &ret_val);
		
		if (status) {
			printf("Error in pthread_join (%d)\n",status);
			break;
		}
		
		// Casting the generic pointer to a pointer of type long
		long *partial_hits = (long *)ret_val;
		
		if (partial_hits != NULL) {
			total_hits += *partial_hits;	// Sum hits of thread
			free(partial_hits);				// Free allocated memory in thread
		}            	
		else 
        printf("Thread %d returned NULL\n", i);
	
	}
	
	free(threads);

	// Showing total number of hits
	printf("hits: %lld\n",total_hits);
	
	// Calculating PI
 	double pi = 4.0 * ((double)total_hits / (samples*num_threads));
	
	// Showing PI 
	printf("Pi: %.16f\n",pi);

	return(0);
} 
