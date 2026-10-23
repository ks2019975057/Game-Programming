#include <stdio.h>
#include <string.h>
int main(void)
{
	char number[20];
	int length, i;
	printf("숫자 입력");
	scanf("%s", number);
	length=strlen(number);
	for(i=0;i<length;i++)	//높은 단위 
		printf("%c\n", number[i]);
	for(i=length;i>=0;i--)	//낮은 단위 
		printf("%c\n", number[i]);
	getchar();
	getchar();
	return 0;
}

