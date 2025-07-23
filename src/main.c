#include <pthread.h>
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256

typedef struct _thread_data_t {
		int tid;
		pthread_t stuff;
} thread_data_t;

double shared;
pthread_mutex_t lock;

static char *print_b64(const void *buf, const size_t len) {
	const unsigned char *b;
	char *p;
	b = buf;
	p = malloc(len * 4);
	return sodium_bin2base64(p, len * 2, b, len,
							 sodium_base64_VARIANT_ORIGINAL);
}

int generate(void) {
	unsigned char k[crypto_generichash_KEYBYTES_MAX]; // Key
	unsigned char h[crypto_generichash_BYTES_MAX];	  // Hash output
	unsigned char m[BUFFER_SIZE];					  // Message
	randombytes_buf(k, sizeof k);
	randombytes_buf(h, sizeof h);
	randombytes_buf(m, sizeof m);
	size_t mlen = 0;
	crypto_generichash(h, sizeof h, m, mlen, k, sizeof k);
	printf("%s\n", print_b64(h, sizeof h));
	return 0;
}

void *threading(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;
	if (sodium_init() < 0) {
		perror(
			"CRITICAL: libsodium couldn't be initialized, not safe to use!\n");
		exit(EXIT_FAILURE);
	} else {
		pthread_mutex_lock(&lock);
		shared += data->stuff;
		generate();
		pthread_mutex_unlock(&lock);
		pthread_exit(NULL);
		exit(EXIT_SUCCESS);
	}
}

int main(void) {
	long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
	if (num_cores == -1) {
		fprintf(stderr, "Could not detect number of cores\n");
		return 1;
	}
	pthread_t *threads = malloc(num_cores * sizeof(pthread_t));
	thread_data_t thr_data[num_cores];
	int i, rc;
	shared = 0;
	pthread_mutex_init(&lock, NULL);
	/* create threads */
	for (i = 0; i < num_cores; ++i) {
		thr_data[i].tid = i;
		thr_data[i].stuff = *threads;
		if ((rc = pthread_create(&threads[i], NULL, threading, &thr_data[i]))) {
			fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
			return EXIT_FAILURE;
		}
	}

	for (i = 0; i < num_cores; ++i) {
		pthread_join(threads[i], NULL);
	}

	return 0;
}
