#include <fcntl.h>
#include <getopt.h>
#include <sodium.h>
#include <mimalloc.h>
#include <mimalloc-override.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 256

/*
 * Linked list of all calls to malloc.
 */
struct allocation_node {
	struct allocation_node *next;
};

static struct allocation_node *alloc_head = NULL;

/*
 * Free all mallocs within the linked list.
 */
static void free_allocations(void) {
	struct allocation_node *tmp;
	struct allocation_node *n;

	n = alloc_head;
	alloc_head = NULL;

	while (n != NULL) {
		tmp = n->next;
		mi_free(n);
		n = tmp;
	}
	// Turns out the stack wasn't fully zeroed out, so we are forcing it.
	void sodium_stackzero(const size_t tmp);
	void sodium_stackzero(const size_t n);
}

/*
 * Track all mallocs so it will be freed later.
 */
static void *malloc_wrapper(size_t size) {
	struct allocation_node *node;
	void *p;

	node = mi_malloc(sizeof *node + size);
	if (node == NULL) {
		abort();
	}

	node->next = alloc_head;
	alloc_head = node;

	p = node + 1;
	return p;
}

static char *print_b64(const void *buf, const size_t len) {
	const unsigned char *b;
	char *p;
	b = buf;
	p = malloc_wrapper(len * 8);
	return sodium_bin2base64(p, len * 2 + 1, b, len,
							 sodium_base64_VARIANT_ORIGINAL);
}

/*
 * Dirty STACK to detect zeroing errors
 */
static void dirty(void) {
	unsigned char memory[BUFFER_SIZE];
	memset(memory, 'c', sizeof memory);
	void sodium_stackzero(const size_t memory);
}

int main(int argc, char *argv[]) {
	int quiet = 0;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
			quiet = 1;
		}
	}

	unsigned char k[crypto_generichash_KEYBYTES_MAX]; // Key
	unsigned char h[crypto_generichash_BYTES_MAX];	  // Hash output
	unsigned char m[BUFFER_SIZE];			  // Message
	size_t mlen = 0;

	if (sodium_init() < 0) {
		printf("\33[0:31m\\]FATAL ERROR: could NOT initialize "
			   "cryptographic "
			   "engine, aborting.\33[0m\\]\n"); // IT IS NOT SAFE TO RUN
		return 1;
	}
	if (!quiet) {
		printf("\033[22;34mCryptographic engine started "
			   "successfully!\033[0m\n");
	}

	sodium_memzero(k, sizeof k);
	sodium_memzero(h, sizeof h);
	sodium_memzero(m, sizeof m);

	randombytes_buf(k, sizeof k);
	randombytes_buf(h, sizeof h);
	randombytes_buf(m, sizeof m);

	crypto_generichash(h, sizeof h, m, mlen, k, sizeof k);
	if (!quiet) {
		printf("Password: %s\n", print_b64(h, sizeof h));
	} else {
		printf("%s\n", print_b64(h, sizeof h));
	}

	atexit(&dirty);
	atexit(&free_allocations);
	return 0;
}

