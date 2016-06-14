mkdir -p build/dist/x86_64

mkdir -p build/dist/x86_64/mpc
gcc -c -m64 -std=c99 -Wall -O3 src/mpc/c/mpc.c -o build/dist/x86_64/mpc/mpc.o
ar -cvq build/dist/x86_64/libmpc.a build/dist/x86_64/mpc/*.o

mkdir -p build/dist/x86_64/hashmap
cd build/dist/x86_64/hashmap/
gcc -m64 -c -std=c99 -Wall -O3 ../../../../src/hashmap/c/*.c
ar -cvq ../libhashmap.a *.o
cd ../../../../

gcc -m64 -std=c99 -Wall -O3 src/main/c/*.c build/dist/x86_64/libmpc.a build/dist/x86_64/libhashmap.a -lreadline -lm -o build/dist/x86_64/main