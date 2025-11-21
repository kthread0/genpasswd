# genpasswd

Simple, fast, and cryptographically secure password generator written in C. Originally made by [lilyyllyyllyly](https://github.com/lilyyllyyllyly), turned into a cryptographically secure password generator by [kthread0](https://github.com/kthread0).

## Features

- **Cryptographically secure**: Uses libsodium's `randombytes_buf` and `randombytes_uniform` for secure random generation
- **Multiple character sets**: Base64, alphanumeric, alphanumeric with symbols, and hexadecimal
- **Customizable length**: Generate passwords from 1 to 1024 characters
- **Batch generation**: Generate multiple passwords at once
- **Exclude ambiguous characters**: Option to exclude easily confused characters (0/O, 1/l/I)
- **Secure memory handling**: Uses `sodium_malloc` and `sodium_free` for protected memory allocation
- **Fast and efficient**: Single-threaded, optimized implementation

## Installation

### Dependencies

- libsodium

```bash
git clone https://github.com/kthread0/genpasswd
cd genpasswd
make
./genpasswd
```

## Usage

```bash
# Generate a 32-character base64 password (default)
./genpasswd

# Generate five 16-character passwords
./genpasswd -l 16 -n 5

# Generate alphanumeric password without ambiguous characters
./genpasswd -c alnum -x

# Generate 20-character password with symbols
./genpasswd -c alnumsym -l 20

# Generate hexadecimal password
./genpasswd -c hex -l 32
```

### Options

- `-l, --length=N`: Password length (default: 32, max: 1024)
- `-n, --count=N`: Number of passwords to generate (default: 1)
- `-c, --charset=TYPE`: Character set type:
  - `base64` (default): Base64 encoding
  - `alnum`: Alphanumeric only
  - `alnumsym`: Alphanumeric with symbols
  - `hex`: Hexadecimal
- `-x, --exclude-ambiguous`: Exclude ambiguous characters (0/O, 1/l/I)
- `-h, --help`: Show help message
- `-v, --version`: Show version information

## Security

This password generator uses libsodium's cryptographically secure random number generators:
- `randombytes_buf()` for filling buffers with random data
- `randombytes_uniform()` for unbiased random selection within a range

All sensitive data is allocated using `sodium_malloc()` which provides guard pages and makes the memory region non-swappable, and freed with `sodium_free()` which securely wipes the memory before deallocation.
