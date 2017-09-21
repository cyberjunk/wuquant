gcc -O3 -march=native -c -Wall -Werror -fpic WuQuant.c
gcc -O3 -march=native -shared -lc -o libwuquant.so WuQuant.o
cp libwuquant.so /usr/lib/
