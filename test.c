#include <stdio.h>
#include <stdlib.h>

int main()
{
	int** a = (int**)malloc(sizeof(int*) * 10);

	for(int i = 0; i < 10; i++)
	{
		a[i] = (int*)malloc(sizeof(int));
	}
	return 0;
}