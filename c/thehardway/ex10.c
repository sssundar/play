#include <stdio.h>

int main(int argc, char *argv[])
{
	int i = 0;
	
	// argv[] array is null terminated
	char *states[] = {
		"California", "Oregon",
		NULL, argv[1]
	};
	
	argv[1] = states[1];
	
	// argv[0] is the function name
	for (i=1;i<argc;i++) {
		printf("arg %d: %s\n", i, argv[i]);
	}

	// setting this > 7 segfaults
	// 4,5,6,7 are uninitialized
	int num_states = 4;

	for (i=0; i<num_states; i++) {
		printf("state %d: %s\n", i, states[i]);
	}	

	return 0;
}