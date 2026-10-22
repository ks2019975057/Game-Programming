#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	char ch;
	printf("문자를 입력하고 Enter>");
	scanf("%c", &ch);
	system("cls"); //화면을 지우고 입력한 문자만 출력 
	printf("입력된 문자 %c\n", ch);
	return 0;
}

