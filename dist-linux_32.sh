mkdir -p build/dist/x86

mkdir -p build/dist/x86/mpc
gcc -m32 -c -std=c99 -Wall -O3 src/mpc/c/mpc.c -o build/dist/x86/mpc/mpc.o
ar -cvq build/dist/x86/libmpc.a build/dist/x86/mpc/*.o

mkdir -p build/dist/x86/hashmap
cd build/dist/x86/hashmap/
gcc -m32 -c -std=c99 -Wall -O3 ../../../../src/hashmap/c/*.c
ar -cvq ../libhashmap.a *.o
cd ../../../../

mkdir -p build/dist/x86/linenoise
gcc -m32 -c -std=c99 -Wall -O3 src/linenoise/c/linenoise.c -o build/dist/x86/linenoise/linenoise.o
ar -cvq build/dist/x86/liblinenoise.a build/dist/x86/linenoise/*.o

gcc -m32 -std=c99 -Wall -O3 src/main/c/*.c build/dist/x86/libmpc.a build/dist/x86/libhashmap.a build/dist/x86/liblinenoise.a -lm -o build/dist/x86/Lydrige-linux_x86_bin-v060a
