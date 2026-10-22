#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
int main(void)
{
	int i, j;
	for(j=1;j<=9;j++)
	{
		system("cls"); // 단이 오를 때마다 화면 지우기 반복 
		for(i=1;i<=9;i++)
			printf("%d*%d=%d\n", j, i, j*i);
		printf("아무키나 누르시오.\n");
		getch(); //프로그램 대기 명령어 
	}
	return 0;
}
