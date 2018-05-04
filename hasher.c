#define _OPEN_SYS_ITOA_EXT
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
//HASHER TYPE Bernstein hash djb2
//source: http://www.cse.yorku.ca/~oz/hash.html
//this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c. another version of this algorithm (now favored by bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained.
//source2: https://gist.github.com/lmas/664afa94f922c1e58d5c3d73aed98f3f
// For when you ever need to implement a dictionary hash function, that's good enough, simple and fast. WARNING: Not cryptographicly secure!
//cusirosidad: Why are 5381 and 33 so important in the djb2 algorithm? https://stackoverflow.com/questions/1579721/why-are-5381-and-33-so-important-in-the-djb2-algorithm

unsigned long compute_hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int main(int argc, char *argv[])
{
    unsigned long hashResult = 0;

    hashResult = compute_hash(argv[1]);
    printf("%lu \n", hashResult);

    return 0;
}
