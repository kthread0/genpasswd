# genpasswd

simple password generator written in C, originally made by [lilyyllyyllyly](https://github.com/lilyyllyyllyly), turned into a cryptographically secure password generator by me ([kthread0](https://github.com/kthread0))

# Installation

## Dependencies

- libsodium
- POSIX Threads

```
git clone https://github.com/kthread0/genpasswd
cd genpasswd
make
```

you probably want to put it in your PATH (somewhere like `~/.local/bin/genpasswd` should be fine, if that directory is in your `PATH`!)
