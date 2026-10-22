#include <stdio.h>
#include <conio.h>
#include <math.h>
void serial_number(long number);
void reverse_number(long number);
int main(void)
{
	long number=0;
	scanf("%ld", &number);
	printf("입력 숫자 : %ld\n\n", number);
	serial_number(number);
	reverse_number(number);
	getch();
	return 0;
}

void serial_number(long number)
{
	int num;
	int i, length=0;
	length=(int)(log10(number)+1);
	for(i=length;i>=1;i--)
	{
		num=number/(long) pow(10, i-1);
		printf("%ld\n", num);
		number=number-num*(long) pow(10,i-1);
	}
	printf("\n");
}

void reverse_number(long number)
{
	while(number>0)
	{
		printf("%ld\n", number%10);
		number/=10;
	}
}

