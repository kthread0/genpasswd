#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VERSION "2.0.0"
#define DEFAULT_LENGTH 32
#define DEFAULT_COUNT 1
#define MAX_PASSWORD_LENGTH 1024

typedef enum {
	CHARSET_BASE64,
	CHARSET_ALPHANUMERIC,
	CHARSET_ALPHANUMERIC_SYMBOLS,
	CHARSET_HEX
} charset_type_t;

typedef struct {
	int length;
	int count;
	charset_type_t charset;
	int exclude_ambiguous;
} options_t;

static const char *ALPHANUMERIC = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static const char *ALPHANUMERIC_NO_AMBIGUOUS = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789";
static const char *ALPHANUMERIC_SYMBOLS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
static const char *ALPHANUMERIC_SYMBOLS_NO_AMBIGUOUS = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789!@#$%^&*()_+-=[]{}|;:,.<>?";

static void print_usage(const char *progname) {
	printf("Usage: %s [OPTIONS]\n\n", progname);
	printf("Generate cryptographically secure random passwords.\n\n");
	printf("Options:\n");
	printf("  -l, --length=N          Password length (default: %d, max: %d)\n", DEFAULT_LENGTH, MAX_PASSWORD_LENGTH);
	printf("  -n, --count=N           Number of passwords to generate (default: %d)\n", DEFAULT_COUNT);
	printf("  -c, --charset=TYPE      Character set type:\n");
	printf("                            base64 (default): Base64 encoding\n");
	printf("                            alnum: Alphanumeric only\n");
	printf("                            alnumsym: Alphanumeric with symbols\n");
	printf("                            hex: Hexadecimal\n");
	printf("  -x, --exclude-ambiguous Exclude ambiguous characters (0/O, 1/l/I)\n");
	printf("  -h, --help              Show this help message\n");
	printf("  -v, --version           Show version information\n");
	printf("\nExamples:\n");
	printf("  %s                      Generate one 32-character base64 password\n", progname);
	printf("  %s -l 16 -n 5           Generate five 16-character passwords\n", progname);
	printf("  %s -c alnum -x          Generate alphanumeric password without ambiguous chars\n", progname);
	printf("  %s -c alnumsym -l 20    Generate 20-character password with symbols\n", progname);
}

static void print_version(void) {
	printf("genpasswd version %s\n", VERSION);
	printf("Cryptographically secure password generator using libsodium\n");
}

static int parse_int(const char *str, int *result, const char *param_name) {
	char *endptr;
	errno = 0;
	long val = strtol(str, &endptr, 10);
	
	if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
		fprintf(stderr, "Error: %s value out of range\n", param_name);
		return -1;
	}
	
	if (endptr == str || *endptr != '\0') {
		fprintf(stderr, "Error: %s must be a valid integer\n", param_name);
		return -1;
	}
	
	*result = (int)val;
	return 0;
}

static int generate_base64(int length) {
	size_t bin_len = (length * 3) / 4 + 1;
	unsigned char *bin_buf = sodium_malloc(bin_len);
	if (!bin_buf) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return 1;
	}
	
	size_t b64_buf_len = sodium_base64_ENCODED_LEN(bin_len, sodium_base64_VARIANT_ORIGINAL);
	char *b64_buf = sodium_malloc(b64_buf_len);
	if (!b64_buf) {
		sodium_free(bin_buf);
		fprintf(stderr, "Error: Memory allocation failed\n");
		return 1;
	}
	
	randombytes_buf(bin_buf, bin_len);
	sodium_bin2base64(b64_buf, b64_buf_len, bin_buf, bin_len, sodium_base64_VARIANT_ORIGINAL);
	
	b64_buf[length] = '\0';
	printf("%s\n", b64_buf);
	
	sodium_free(bin_buf);
	sodium_free(b64_buf);
	return 0;
}

static int generate_charset(int length, const char *charset, size_t charset_len) {
	char *password = sodium_malloc(length + 1);
	if (!password) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return 1;
	}
	
	for (int i = 0; i < length; i++) {
		uint32_t random_val = randombytes_uniform((uint32_t)charset_len);
		password[i] = charset[random_val];
	}
	password[length] = '\0';
	
	printf("%s\n", password);
	
	sodium_free(password);
	return 0;
}

static int generate_hex(int length) {
	size_t bin_len = (length + 1) / 2;
	unsigned char *bin_buf = sodium_malloc(bin_len);
	if (!bin_buf) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return 1;
	}
	
	size_t hex_buf_len = bin_len * 2 + 1;
	char *hex_buf = sodium_malloc(hex_buf_len);
	if (!hex_buf) {
		sodium_free(bin_buf);
		fprintf(stderr, "Error: Memory allocation failed\n");
		return 1;
	}
	
	randombytes_buf(bin_buf, bin_len);
	sodium_bin2hex(hex_buf, hex_buf_len, bin_buf, bin_len);
	
	hex_buf[length] = '\0';
	printf("%s\n", hex_buf);
	
	sodium_free(bin_buf);
	sodium_free(hex_buf);
	return 0;
}

static int generate_password(const options_t *opts) {
	switch (opts->charset) {
		case CHARSET_BASE64:
			return generate_base64(opts->length);
		
		case CHARSET_ALPHANUMERIC:
			if (opts->exclude_ambiguous) {
				return generate_charset(opts->length, ALPHANUMERIC_NO_AMBIGUOUS, 
					strlen(ALPHANUMERIC_NO_AMBIGUOUS));
			} else {
				return generate_charset(opts->length, ALPHANUMERIC, 
					strlen(ALPHANUMERIC));
			}
		
		case CHARSET_ALPHANUMERIC_SYMBOLS:
			if (opts->exclude_ambiguous) {
				return generate_charset(opts->length, ALPHANUMERIC_SYMBOLS_NO_AMBIGUOUS,
					strlen(ALPHANUMERIC_SYMBOLS_NO_AMBIGUOUS));
			} else {
				return generate_charset(opts->length, ALPHANUMERIC_SYMBOLS,
					strlen(ALPHANUMERIC_SYMBOLS));
			}
		
		case CHARSET_HEX:
			return generate_hex(opts->length);
		
		default:
			fprintf(stderr, "Error: Unknown charset type\n");
			return 1;
	}
}

int main(int argc, char *argv[]) {
	options_t opts = {
		.length = DEFAULT_LENGTH,
		.count = DEFAULT_COUNT,
		.charset = CHARSET_BASE64,
		.exclude_ambiguous = 0
	};
	
	static struct option long_options[] = {
		{"length", required_argument, 0, 'l'},
		{"count", required_argument, 0, 'n'},
		{"charset", required_argument, 0, 'c'},
		{"exclude-ambiguous", no_argument, 0, 'x'},
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};
	
	int opt;
	int option_index = 0;
	
	while ((opt = getopt_long(argc, argv, "l:n:c:xhv", long_options, &option_index)) != -1) {
		switch (opt) {
			case 'l':
				if (parse_int(optarg, &opts.length, "Length") != 0) {
					return EXIT_FAILURE;
				}
				if (opts.length <= 0 || opts.length > MAX_PASSWORD_LENGTH) {
					fprintf(stderr, "Error: Length must be between 1 and %d\n", MAX_PASSWORD_LENGTH);
					return EXIT_FAILURE;
				}
				break;
			
			case 'n':
				if (parse_int(optarg, &opts.count, "Count") != 0) {
					return EXIT_FAILURE;
				}
				if (opts.count <= 0) {
					fprintf(stderr, "Error: Count must be positive\n");
					return EXIT_FAILURE;
				}
				break;
			
			case 'c':
				if (strcmp(optarg, "base64") == 0) {
					opts.charset = CHARSET_BASE64;
				} else if (strcmp(optarg, "alnum") == 0) {
					opts.charset = CHARSET_ALPHANUMERIC;
				} else if (strcmp(optarg, "alnumsym") == 0) {
					opts.charset = CHARSET_ALPHANUMERIC_SYMBOLS;
				} else if (strcmp(optarg, "hex") == 0) {
					opts.charset = CHARSET_HEX;
				} else {
					fprintf(stderr, "Error: Unknown charset '%s'\n", optarg);
					fprintf(stderr, "Valid options: base64, alnum, alnumsym, hex\n");
					return EXIT_FAILURE;
				}
				break;
			
			case 'x':
				opts.exclude_ambiguous = 1;
				break;
			
			case 'h':
				print_usage(argv[0]);
				return EXIT_SUCCESS;
			
			case 'v':
				print_version();
				return EXIT_SUCCESS;
			
			default:
				print_usage(argv[0]);
				return EXIT_FAILURE;
		}
	}
	
	if (sodium_init() < 0) {
		fprintf(stderr, "CRITICAL: libsodium couldn't be initialized, not safe to use!\n");
		return EXIT_FAILURE;
	}
	
	for (int i = 0; i < opts.count; i++) {
		if (generate_password(&opts) != 0) {
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}
