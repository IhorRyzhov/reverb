#include <stdio.h>
#include <pthread.h>

int main(void)
{
	pthread_self();

	printf("Hello world");

	return 0;
}
