/*
 * KigraSoft Simple Profile Tool
 *
 * kspt.c
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <threads.h>

const long nanosec = 1000000000;

struct sieve_data {
	int test_limit;
};

struct factorial_data {
	int n;
};

struct fibonacci_data {
	int n;
};

int
prime_sieve(void *param)
{
	struct sieve_data *sdata = param;
	int test_limit = sdata->test_limit;
	int n = 2;
	int count = 0;

	bool *test_range = malloc(test_limit + 1);
	if (test_range == NULL) {
		return 0;
	}

	test_range[0] = false;
	test_range[1] = false;
	for (int i = 2; i <= test_limit; i++) {
		test_range[i] = true;
	}

	while (n < test_limit) {
		for (int i = n + n; i <= test_limit; i = i + n) {
			test_range[i] = false;
		}
		n++;
	}

	for (int i = 2; i <= test_limit; i++) {
		if (test_range[i] == true) {
			count++;
		}
	}

	free(test_range);
	
	return count;
}

int
factorial(void *param) {
	struct factorial_data *fdata = param;
	int n = fdata->n;
	int a = 1;
	uint64_t r = 1;

	while (a <= n) {
		r = r * a;
		a++;
	}

	return r;
}

int
fibonacci(void *param)
{
	struct fibonacci_data *fdata = param;
	int n = fdata->n;
	int a = 2;
	uint64_t r0 = 0;
	uint64_t r1 = 1;
	uint64_t r  = 0;

	while (a <= n) {
		r = r0 + r1;
		r0 = r1;
		r1 = r;
		a++;
	}

	return r;
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
main()
{
	int count;
	long total_time;
	double total_seconds = 0.0;
	struct sieve_data sdata;
	sdata.test_limit = 1000 * 1000 * 10;
	int cycles = 16;

	// for prime_sieve 1000 x 1000 = approx. 1M memory usage
	total_time = time_function(&prime_sieve, &sdata, cycles);
	total_seconds = (double)total_time / nanosec;

	printf("Prime Sieve\n");
	printf("Single - Total time:     %li\n", total_time);
	printf("Single - Total seconds:  %.8g\n", total_seconds);

	total_time = time_function_threaded(&prime_sieve, &sdata, cycles);
	total_seconds = (double)total_time / nanosec;

	printf("Mulit  - Total time:     %li\n", total_time);
	printf("Multi  - Total seconds:  %.8g\n", total_seconds);

	struct factorial_data fdata;
	fdata.n = 20;
	cycles = 50;

	total_time = time_function(&factorial, &fdata, cycles);
	total_seconds = (double)total_time / nanosec;

	printf("Factorial\n");
	printf("Single - Total time:     %li\n", total_time);
	printf("Single - Total seconds:  %.8g\n", total_seconds);

	total_time = time_function_threaded(&factorial, &fdata, cycles);
	total_seconds = (double)total_time / nanosec;
	
	printf("Mulit  - Total time:     %li\n", total_time);
	printf("Multi  - Total seconds:  %.8g\n", total_seconds);

	struct fibonacci_data fibdata;
	fibdata.n = 90;
	cycles = 16;

	total_time = time_function(&fibonacci, &fibdata, cycles);
	total_seconds = (double)total_time / nanosec;

	printf("Fibonacci\n");
	printf("Single - Total time:     %li\n", total_time);
	printf("Single - Total seconds:  %.8g\n", total_seconds);

	total_time = time_function_threaded(&fibonacci, &fibdata, cycles);
	total_seconds = (double)total_time / nanosec;
	
	printf("Mulit  - Total time:     %li\n", total_time);
	printf("Multi  - Total seconds:  %.8g\n", total_seconds);

	return 0;
}
