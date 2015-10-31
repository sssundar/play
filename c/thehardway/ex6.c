#include <stdio.h>

int main(int argc, char *argv[])
{
	char initial = 'A';
	char first_name[] = "Zed";
	char last_name[] = "Shaw";

	printf("My name is %s %c. %s.\n",
		first_name, initial, last_name);

	return 0;
}
