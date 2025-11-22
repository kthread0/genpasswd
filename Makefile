FILES := $(wildcard src/*.c)

CFLAGS += -march=native -O3 -flto -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fstack-clash-protection -fstack-protector-strong -fexceptions -fPIE -pie -Wtrampolines -ftrivial-auto-var-init=zero -fcf-protection=full
LDFLAGS += -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries

WARNINGS += -Wall -Wpedantic -Werror

LIBS += -lsodium

build: $(FILES)
	$(CC) $(FILES) $(WARNINGS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o genpasswd

clean:
	rm -vf genpasswd

run:
	./genpasswd
