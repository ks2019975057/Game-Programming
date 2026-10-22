#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(void)
{
	int i;
	srand(time(NULL)); //시간 시드로 랜덤 유지 
	for(i=1;i<=10;i++)
	printf("%2d:%d\n",i, rand()%6+1);
	return 0;
} 
