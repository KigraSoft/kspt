/*
 KigraSoft Simple Profile Tool
 
 kspt.c

 Copyright 2026 Stephen R. Kifer
 
 This file is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation; either version 3.0 of the License, or (at
 your option) any later version.

 This file is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see
 <https://www.gnu.org/licenses/>.
*/

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <threads.h>

const long nanosec = 1000000000;
struct process_data {
	int    (*funct_ptr)(void*);
	uint32_t cycles;
	uint32_t a;
	uint32_t b;
	char    *title;
};

int
prime_sieve(void *param)
{
	struct process_data *sdata = param;
	uint32_t test_limit = sdata->a;
	uint32_t n = 2;
	uint32_t count = 0;
	uint32_t i = 0;

	bool *test_range = malloc((test_limit + 1) * sizeof(bool));
	if (test_range == NULL) {
		return 0;
	}

	test_range[0] = false;
	test_range[1] = false;
	for (i = 2; i <= test_limit; i++) {
		test_range[i] = true;
	}

	while (n < test_limit) {
		for (i = n + n; i <= test_limit; i = i + n) {
			test_range[i] = false;
		}
		n++;
	}

	for (i = 2; i <= test_limit; i++) {
		if (test_range[i] == true) {
			count++;
		}
	}

	free(test_range);
	
	return count;
}

int
rand_sort(void *param)
{
	struct process_data *pdata = param;
	int n = pdata->a;
	int seed = 0;
	int tmp = 0;

	int *numbers = malloc(n * sizeof(int));
	if (numbers == NULL) {
		return 0;
	}

	for (int i = 0; i < n; i++) {
		numbers[i] = rand_r(&seed);
	}

	for (int i = n; i > 0; i--) {
		for (int j = 1; j < i; j++) {
			if (numbers[j-1] > numbers[j]) {
				tmp = numbers[j-1];
				numbers[j-1] = numbers[j];
				numbers[j] = tmp;
			}
		}
	}

	free(numbers);

	return 0;
}

long
time_function(int funct_ptr(void*), void *funct_param, int cycles) {
	long start_time, end_time, total_time;
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	start_time = (long)ts.tv_sec * nanosec + ts.tv_nsec;

	for (int a = 0; a < cycles; a++) {
		funct_ptr(funct_param);
	}
	
	timespec_get(&ts, TIME_UTC);
	end_time = (long)ts.tv_sec * nanosec + ts.tv_nsec;
	total_time = end_time - start_time;
	return(total_time);
}

long
time_function_threaded(int funct_ptr(void*), void *funct_param, int cycles) {
	thrd_t threads[cycles];
	long start_time, end_time, total_time;
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	start_time = (long)ts.tv_sec * nanosec + ts.tv_nsec;

	for (int a = 0; a < cycles; a++) {
		thrd_create(&threads[a], funct_ptr, funct_param);
	}

	for (int a = 0; a < cycles; a++) {
		thrd_join(threads[a], NULL);
	}
	
	timespec_get(&ts, TIME_UTC);
	end_time = (long)ts.tv_sec * nanosec + ts.tv_nsec;
	total_time = end_time - start_time;
	return(total_time);
}

int
test_function(void *funct_param) {
	struct process_data *pdata = funct_param;
	uint32_t count = 0;
	long total_time = 0;
	double total_seconds = 0.0;

	printf("Test Function: %s\n", pdata->title);
	total_time = time_function(pdata->funct_ptr, pdata, pdata->cycles);
	total_seconds = (double)total_time / nanosec;

	//printf("  Single - Total Time:     %li\n", total_time);
	printf("  Single - Total Seconds:  %.8g\n", total_seconds);

	total_time = time_function_threaded(pdata->funct_ptr, pdata, pdata->cycles);
	total_seconds = (double)total_time / nanosec;

	//printf("  Mulit  - Total Time:     %li\n", total_time);
	printf("  Multi  - Total Seconds:  %.8g\n", total_seconds);

	return 0;
}

int
main()
{
	int count;
	long total_time;
	double total_seconds = 0.0;
	struct process_data pdata;
	pdata.cycles = 32;
	pdata.b = 1;

	pdata.funct_ptr = &prime_sieve;
	pdata.a = 1000 * 1000 * 10; // for prime_sieve 1000 x 1000 = approx. 1M memory usage
	pdata.title = "Prime Sieve";
	test_function(&pdata);
	
	pdata.funct_ptr = &rand_sort;
	pdata.a = 1000 * 20;
	pdata.title = "Bubble Sort of Random Numbers";
	test_function(&pdata);
		
	return 0;
}
