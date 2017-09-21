gcc -c -Wall -Werror -fpic WuQuant.c
gcc -shared -lc -o libwuquant.so WuQuant.o
cp libwuquant.so /usr/lib/
