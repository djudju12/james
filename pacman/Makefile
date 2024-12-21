CC=gcc
RAYLIB=../vendor/raylib/src
CFLAGS=-Wall -Wextra -ggdb -I$(RAYLIB)
LDFLAGS=-L./bin -lraylib -lm
PROGRAM_NAME=pacman

build: main.c bin/ buildr
	$(CC) -o bin/$(PROGRAM_NAME) main.c $(CFLAGS) $(LDFLAGS)

buildr:
	make -C $(RAYLIB) PLATFORM=PLATFORM_DESKTOP
	cp $(RAYLIB)/libraylib.a ./bin

bin/:
	mkdir -p bin

clear:
	rm -r bin