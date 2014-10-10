#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
	int *data;
    int count;
} workpart;

void *shellSort(void *args);
void *shellSort_single(void *args);
void MergeSort(int *arrayA, int sizeA, int* arrayB, int sizeB, int* arrayC, int sizeC);
void write_to_inputfile(char *filename, int size);

int main(int argc, const char* argv[]) {
	if (argc != 4) {
		printf("program format: program input_file output_file thread_count\n");
		return -1;
	}
	printf("Input file: %s\n", argv[1]);
	printf("Output file: %s\n", argv[2]);
	FILE *f= fopen (argv[1], "rt");
	if (f == NULL) {
		printf ("can not open file %s\n", "infile");
		return -1;
	}
	int i, j, thread_count, global_size;
	char *p;
	errno = 0;
	long conv = strtol(argv[3], &p, 10);
	
	if (errno != 0 || *p != '\0' || conv > INT_MAX) {
		printf("enter valid threads count\n");
	} 
	else {
		thread_count = conv;
		if (thread_count < 1) {
			printf("enter valid threads count\n");
			return -1;
		}
	}
	printf("Thread count: %d\n", thread_count);
	write_to_inputfile(argv[1], 4000);
	
	fscanf(f, "%d ", &global_size);
	printf("Global size = %d\n", global_size);
	int *in= malloc (global_size * sizeof(int));
	if (!in) {
		printf ("can not allocate memory\n");
		return -1;
	}
	
	int array[4000];
	for (i= 0; i < global_size; i++) 
	{
		int k;
		fscanf (f, "%d ", in+i);
		k = *(in+i);
		array[i] = k;
	}
	printf("\n\n");
	fclose (f);
	
	clock_t begin, end;
	double time_spent;
	
	workpart single_thread_wp;
	single_thread_wp.data = array;
	single_thread_wp.count = global_size;
	
	begin = clock();
	shellSort_single(&single_thread_wp);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	for (i = 0; i < global_size; i++) {
	}
	
	printf("\nTIME for single thread Shellsort %f", time_spent);
	
	begin = clock();
	workpart wp[thread_count];
	pthread_t thr[thread_count];
	for (i=0; i < thread_count; i++) {
		wp[i].data= in + (global_size/thread_count)*i;
		wp[i].count = global_size/thread_count;
		if (pthread_create(&thr[i], NULL, shellSort,( void *)&wp[i])) {
			printf ("thread %d creation failed", i);
			return -1;
		}
	}
	for (i=0; i < thread_count; i++) if (pthread_join (thr[i], NULL)) {
		printf ("thread %d join failed", i);
		return -1;
	}
	
	for (i=0; i < thread_count; i++) {
		for (j=0; j < wp[i].count; j++) {
		}
	}
	printf("\n\n");
	
	free (in);
	in = NULL;
	
	int *merge_part_1 = malloc(sizeof(int)*(wp[0].count+wp[1].count)); 
	MergeSort(wp[0].data, wp[0].count, wp[1].data, wp[1].count, merge_part_1, (wp[0].count+wp[1].count));
	
	int *merge_part_2 = malloc(sizeof(int)*(wp[2].count+wp[3].count)); 
	MergeSort(wp[2].data, wp[2].count, wp[3].data, wp[3].count, merge_part_2, (wp[2].count+wp[3].count));
	
	int *merge_final = malloc(sizeof(int)*global_size);
	MergeSort(merge_part_1, (wp[0].count+wp[1].count), merge_part_2, (wp[2].count+wp[3].count), merge_final, global_size);
	
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("\nTIME for Multi-thread shellsort + mergesort %f\n", time_spent);
	
	f= fopen (argv[2], "w");
	if (!f) {
		printf ("can not create file %s", "outfile");
		return -1;
	}
	fprintf (f, "%d\n", global_size);
	
	for (i=0; i < global_size; i++) {
		//printf("%d ", merge_final[i]);
		fprintf (f, "%d\n", merge_final[i]);
	}
	printf("\n");
	
	fclose (f);
	return 1;
}  

void *shellSort(void *curworkpart)
{
    int n = ((workpart *)curworkpart)->count;
    int *a = ((workpart *)curworkpart)->data;
	
    int step = n / 2;
    int i, j;
	
    while (step > 0)
    {
        for (i = 0; i < n - step; i++)
        {
            j = i;
            while ((j >= 0) && (a[j]) > a[j + step])
            {
                int buf = a[j];
                a[j] = a[j + step];
                a[j + step] = buf;
                j--;
            }
        }
        step /= 2;
    }
    pthread_exit("Thread exit");
}

void *shellSort_single(void *curworkpart)
{
    int n = ((workpart *)curworkpart)->count;
    int *a = ((workpart *)curworkpart)->data;
	
    int step = n / 2;
    int i, j;
	
    while (step > 0)
    {
        for (i = 0; i < n - step; i++)
        {
            j = i;
            while ((j >= 0) && (a[j]) > a[j + step])
            {
                int buf = a[j];
                a[j] = a[j + step];
                a[j + step] = buf;
                j--;
            }
        }
        step /= 2;
    }
}

void MergeSort(int *arrayA, int sizeA, int *arrayB, int sizeB, int *arrayC, int sizeC)
{
	int indexA = 0;
	int indexB = 0;
	int indexC = 0;
	
	if (sizeC < sizeA+sizeB) {
		printf("Wrong array size!\n");
		return;
	}
	
	while((indexA < sizeA) && (indexB < sizeB)) {
		if (arrayA[indexA] < arrayB[indexB]) {
			arrayC[indexC] = arrayA[indexA];
			indexA++;
		}
		else {
			arrayC[indexC] = arrayB[indexB];
			indexB++;
		}
		indexC++;
	}
	while (indexA < sizeA) {
		arrayC[indexC] = arrayA[indexA];
		indexA++;
		indexC++;

	}
	while (indexB < sizeB) {
		arrayC[indexC] = arrayB[indexB];
		indexB++;
		indexC++;
	}
	return;
}

void write_to_inputfile(char *filename, int size)
{
    FILE *fo;
    fo = fopen(filename,"w");
	
    if(fo == 0)
    {
        perror(filename);
        exit(EXIT_FAILURE);
    }
	fprintf(fo, "%d ", size);
    while(size--)
        fprintf(fo,"%d ",1 + rand() % 99);
    fclose(fo);
}