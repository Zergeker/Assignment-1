#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <stdbool.h>
#include "crackme.h"

//function for computing a permutation for the next iteration
bool performNextOperation(char* str, int stringSize, int alphabetSize, int alphabetMaxValue, int addedNum)
{
	int a = addedNum / alphabetSize;
	int b = addedNum % alphabetSize;

	//if sum of addedNum modulo and last char numerical value is bigger than alphabet max size
	//the previous char gets increased by 1
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
					j--;
					str[j] = (int)str[j] + 1;
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
						j--;
						str[j] = (int)str[j] + 1;
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

	//initializing a zero-char filled char* of a size passed into the function
	char* str = (char*)malloc((sizePass + 1) * sizeof(char));
	for (int i = 0; i < sizePass; i++)
	{
		str[i] = -128;
	}

	str[sizePass] = '\0';

	if (p(sizePass, str) == 0)
	{
		printf("Password consisted of zero-chars");
		free(str);
		return 0;
	}

	int world_rank, world_size;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);	

	int check = 1;
	bool passwordFlag = true;
	int probeFlag;

	//storing time of the begging of computations
	time_t t1 = time(0);

	passwordFlag = performNextOperation(str, sizePass, 256, 127, world_rank);

	while (passwordFlag)
	{
		//check if current permutation is the one we are looking for
		check = p(sizePass, str);
		if (check == 0)
		{
			//if the needed permutation is found, break the loop
			passwordFlag = false;
			if (world_size > 0)
			{
				//send signal to other processes
				int msg = 1;
				MPI_Request request;
				for (int i = 0; i < world_size; i++)
					if (i!=world_rank)
						MPI_Isend(&msg, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request);
			}
		}
		else
			passwordFlag = performNextOperation(str, sizePass, 256, 127, world_size);

		//checking for the signal from the process which found the node
		if (world_size > 0)
		{
			MPI_Iprobe(MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &probeFlag, MPI_STATUSES_IGNORE);
			//if signal was found, break the loop
			if (probeFlag == 1)
				passwordFlag = false;
		}	
	}

	time_t t2 = time(0);

	if (check == 0)
		printf("Pass was found: %s  On rank: %d  It took %f seconds\n'", str, world_rank, difftime(t2, t1));
		
	free(str);
	MPI_Finalize();
	return 0;
}