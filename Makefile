default: build/main

build/main: main.c
	gcc -std=c99 -Wall main.c mpc/mpc.c -lreadline -lm -o build/main

run: build/main
	./build/main

clean:
	rm -rf build