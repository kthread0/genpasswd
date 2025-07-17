FILES := $(wildcard src/*.c)

FLAGS = -march=native -O3 -flto -pipe -fno-plt -fexceptions \
        -Wall -Wextra -Wpedantic -Werror \
        -fstack-clash-protection -fcf-protection \
	-Wl,-O1 -Wl,--sort-common -Wl,--as-needed -Wl,-z,relro -Wl,-z,now \
        -Wl,-z,pack-relative-relocs

LIBS = -lpthread -pthread -lsodium

build: $(FILES)
	$(CC) $(FILES) $(FLAGS) $(LIBS) -o genpasswd

clean:
	rm -vf genpasswd

run:
	./genpasswd
