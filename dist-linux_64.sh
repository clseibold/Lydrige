mkdir -p build/dist/x86_64

mkdir -p build/dist/x86_64/mpc
gcc -c -m64 -std=c99 -Wall -O3 src/mpc/c/mpc.c -o build/dist/x86_64/mpc/mpc.o
ar -cvq build/dist/x86_64/libmpc.a build/dist/x86_64/mpc/*.o

mkdir -p build/dist/x86_64/hashmap
cd build/dist/x86_64/hashmap/
gcc -m64 -c -std=c99 -Wall -O3 ../../../../src/hashmap/c/*.c
ar -cvq ../libhashmap.a *.o
cd ../../../../

mkdir -p build/dist/x86_64/linenoise
gcc -m64 -c -std=c99 -Wall -O3 src/linenoise/c/linenoise.c -o build/dist/x86_64/linenoise/linenoise.o
ar -cvq build/dist/x86_64/liblinenoise.a build/dist/x86_64/linenoise/*.o

gcc -m64 -std=c99 -Wall -O3 src/main/c/*.c build/dist/x86_64/libmpc.a build/dist/x86_64/libhashmap.a build/dist/x86_64/liblinenoise.a -lm -o build/dist/x86_64/Lydrige-linux_x86_64_bin-v060a
