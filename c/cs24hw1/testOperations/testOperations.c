#include <stdio.h>

int main(int argc, char **argv) {
    int res;
    unsigned int n;
    res = sscanf(argv[1], "%u", &n);
    if (res == 1) {
      printf("n:  %u\n", n);
      n = n & (n-1);
      printf("n = n & (n-1): %u\n",n);
    }
}

