#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdbool.h>
#include "crackme.h"

const void showCurrentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	printf(buf);
	printf("\n");
}

bool performNextOperation(char* str, int stringSize, int alphabetSize, int alphabetMaxValue, int addedNum)
{
	int a = addedNum / alphabetSize;
	int b = addedNum % alphabetSize;

	if ((int)str[stringSize - 1] + b > alphabetMaxValue)
	{
		if (stringSize > 1)
		{
			str[stringSize - 1] = (int)str[stringSize - 1] + b - alphabetSize;
			int j = stringSize - 2;
			str[j] = (int)str[j] + 1;
			while ((int)str[j] == alphabetMaxValue)
			{
				if (j > 0)
				{
					str[j] = -128;
					str[--j] = (int)str[j] + 1;
				}
				else
					return false;
			}
		}
		else
			return false;
	}
	else
		str[stringSize - 1] = (int)str[stringSize - 1] + b;

	int counter = 0;
	while (a > 0)
	{
		int c = a % alphabetSize;
		a = a / alphabetSize;


		if ((int)str[stringSize - 2 - counter] + c > alphabetMaxValue)
		{
			if (stringSize > 1)
			{
				str[stringSize - 2 - counter] = (int)str[stringSize - 2 - counter] + c - alphabetSize;
				int j = stringSize - 3 - counter;
				str[j] = (int)str[j] + 1;
				while ((int)str[j] == alphabetMaxValue)
				{
					if (j > 0)
					{
						str[j] = -128;
						str[--j] = (int)str[j] + 1;
					}
					else
						return false;
				}
			}
			else
				return false;
		}
		else
			str[stringSize - 2 - counter] = (int)str[stringSize - 2 - counter] + c;

		counter++;
	}
	return true;
}


int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Wrong usage: %s <size of password to generate>\n", argv[0]);
		exit(1);
	}

	int sizePass = atoi(argv[1]);
	const int alphabet = 127;

	char* str = (char*)malloc((sizePass + 1) * sizeof(char));
	for (int i = 0; i < sizePass; i++)
	{
		str[i] = -128;
	}

	str[sizePass] = '\0';

	if (p(sizePass, str) == 1)
	{
		showCurrentDateTime();
		printf("SUCCESS ON ZERO CHARS");
		free(str);
		return 0;
	}

	
	fflush(stdout);
	showCurrentDateTime();

	int world_rank, world_size;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);	

	int check = 0;
	bool flag = true;

	flag = performNextOperation(str, sizePass, 256, 127, world_rank);

	while (flag)
	{
		check = p(sizePass, str);
		if (check == 1)
		{
			showCurrentDateTime();
			printf("SUCCESS: %s  ON RANK:  << %d \n'", str, world_rank);
			flag = false;
		}
		else
			flag = performNextOperation(str, sizePass, 256, 127, world_size);
	}


	free(str);

	MPI_Finalize();
	return 0;
}