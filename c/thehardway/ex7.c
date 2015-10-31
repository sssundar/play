#include <stdio.h>

int main(int argc, char *argv[])
{
	int bugs = 100;
	double bug_rate = 1.2;
	
	printf("You have %d bugs at the imaginary rate of %f.\n", 
		bugs, bug_rate);
	
	long universe_of_defects = 1L * 1024L * 1024L * 1024L;
	printf("The entire universe has %ld bugs.\n",
		universe_of_defects);
	
	double expected_bugs = bugs * bug_rate;
	printf("You are expected to have %f bugs.\n",
		expected_bugs);
	
	double part_of_universe = expected_bugs / universe_of_defects;
	printf("That is only a %e portion of the universe.\n",
		part_of_universe);

	printf("Print empty string %s\n", 
		"");

	// this makes no sense, just a demo of something weird
	char nul_byte = '\0';
	
	/* If %c is replaced with %s: 
	printing nul_byte works, prints (null), since
	I guess it's the string delimiter, but printing 'c' fails
	because printf is looking for the next character, for the
	null delimiter, and it segfaults. */
	printf("Test print char with string identifer: %c\n",
		'c');

	int care_percentage = bugs * nul_byte; 
	printf("Which means you should care %d%%.\n",
		care_percentage);

	return 0;
}
