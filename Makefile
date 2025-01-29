CC = clang
AR = llvm-ar
NM = llvm-nm
RANLIB = llvm-ranlib

FILES := $(wildcard src/*.c)

FLAGS = -march=x86-64-v3 -O3 -flto=thin -pipe -fno-plt -fexceptions \
        -Wall -Wextra -Wpedantic -Werror \
        -fstack-clash-protection -fcf-protection -fuse-ld=lld \
	-Wl,-O1 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now \
        -Wl,-z,pack-relative-relocs

SANITIZE = -fsanitize=cfi -fvisibility=hidden

LIBS = -pthreads -lsodium -lmimalloc

build: $(FILES)
	$(CC) $(FILES) $(FLAGS) $(SANITIZE) $(LIBS) -o genpasswd

clean:
	rm -vf genpasswd

run:
	./genpasswd

