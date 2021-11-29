#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// size of integer array
#define SIZE 1000000

/* change number of threads 1, 10, 100, 1000, 100000, and 20000 */
#define N 10
 	
// unsorted array 
int array[SIZE];	

void *worker_thread(void *arg)		
{
	int id = (int)arg;		
	//printf("This is worker_thread #%d\n", id); // this line can be uncommented to see the id of the thread

    int max;

	/* find index_of_interval_start */
    int index_of_interval_start = id * (SIZE / N);


	/* find index_of_interval_finish */
    int index_of_interval_finish = (id + 1) * (SIZE / N);


	/* find the maximal element in the interval */
    for (int i = index_of_interval_start; i < index_of_interval_finish; i++) {
        if (array[i] > max)
            max = array[i];
    }

	/* return with maximal element */
    array[id] = max;
}

int main()
{
	/* define an array with type pthread_t, and size N */
    pthread_t threads[N];

	/* define an array to store all maximal elements obtained from each threads */
    int maxElementsArray[SIZE];

	// this creates an unsorted array with random elements
	// do not change
	int i;
	srand(time(NULL));
	for(i = 0; i < SIZE; i++){
		array[i] = rand() % 10000;
	}

	/* create N threads */
    for( i = 0; i < N; i++ ) {
        printf("main() : creating thread, %d\n", i);
        pthread_create(&threads[i], NULL, worker_thread, (void *) i);
    }

	/* join threads and return value of each thread assigned to maxElementsArray[] with corresponding id of the thread */
    for( i = 0; i < N; i++ ) {
        printf("main() : joining thread, %d\n", i);
        pthread_join(threads[i], NULL);
    }

	/* find the maximum from maxElementsArray[] */
    int max;

    for (i = 0; i < N; i++) {
        if (maxElementsArray[i] > max)
            max = maxElementsArray[i];
    }

	printf("After threads joined, the found max value by parent is %d\n", max);

	return 0;
}
