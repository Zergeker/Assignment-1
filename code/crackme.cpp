#include "crackme.h"

int p(int sizePass, char* stringToTest)
{
	srand(sizePass);
	char* p = (char*)malloc((sizePass + 1) * sizeof(char));

	for (int i = 0; i < sizePass; i++)
		p[i] = (char)(rand() % 256 - 128);
	p[sizePass] = '\0';

	if (strcmp(stringToTest, p) == 0)
	{
		free(p);
		return 1;
	}

	free(p);
	return 0;
};