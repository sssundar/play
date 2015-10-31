#include <stdio.h>

int main(int argc, char *argv[])
{
	char name[4] = {'a'};
	int numbers[4] = {0};

	printf("numbers: %d %d %d %d\n",
		numbers[0],
		numbers[1],
		numbers[2],
		numbers[3]);
	printf("name each: %c %c %c %c\n",
		name[0],
		name[1],
		name[2],
		name[3]);
	printf("name: %s\n", name);
	
	// the point being, C apparently initializes structs to 0
	
	// initialize
	numbers[0] = 'c';
	numbers[1] = 'a';
	numbers[2] = 't';
	numbers[3] = '\0';
	name[0] = 'Z';
	name[1] = 'e';
	name[2] = 'd';
	name[3] = '\0';

	printf("numbers: %d %d %d %d\n",
		numbers[0],
		numbers[1],
		numbers[2],
		numbers[3]);
	printf("name each: %d %d %d %d\n",
		name[0],
		name[1],
		name[2],
		name[3]);
	printf("name: %s\n", name);

	// with pointers
	char *another = "Zed";
	printf("another: %s\n", another);
	printf("another each: %c %c %c %c\n",
		another[0],	
		another[1],	
		another[2],	
		another[3]);

	return 0;
}
