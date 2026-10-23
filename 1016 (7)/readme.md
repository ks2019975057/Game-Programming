# 게임프로그래밍 7주차 수업내용 1016

## 숫자의 변환과 표현
    숫자에 대한 자리 수 분리
    
    숫자를 단위 별로 분리하여 순서대로 출력하는 방법
    입력된 금액 또는 숫자가 756826 이라면 순서대로 출력하는 방법에는 다음과 같이 두 가지 방법을 생각할 수 있다.
   
    높은 단위부터 출력 : 7 5 6 8 2 6
    낮은 단위부터 출력 : 6 2 8 6 5 7

### 입력될 숫자를 문자열로 처리
    
    입력된 내용을 충분한 크기의 문자형 배열에 저장하여 문자열의 길이를 계산한 다음 순서대로 또는 역순으로 배열요소를 출력.
    입력될 숫자 (756826)를 scanf를 이용하여 문자형 배열 number[]에 저장한다.
    | 배열 첨자 | number[0] | number[1] | number[2] | number[3] | number[4] | number[5] | number[6] |
    |:---------:|:---------:|:---------:|:---------:|:---------:|:---------:|:---------:|:---------:|
    | 값        | '7'       | '5'       | '6'       | '8'       | '9'       | '6'       | '\0'      |

### 입력된 숫자를 정수형으로 처리
    문자열은 첨자로 구분하여 문자별로 출력이 가능하지만 숫자는 해당 단위수로 나누어야 하기에 나눗셈과 나머지 연산이 필요.
    입력된 정수가 756896이라면 다음과 같이 나누어 가면서 각 자릿수의 숫자를 출력
    
    높은 단위부터 출력 : 756896/100000 = 7
                        56896/10000 = 5
                        6896/1000 = 6
                        896/100 = 8
                        96/10 = 9
                        6/1 = 6
    낮은 단위부터 출력 : 756896 % 10 = 6
                        75689 % 10 = 9
                        7568 % 10 = 8
                        756 % 10 = 6
                        75 % 10 = 5
                        7 % 10 = 7

    높은 단위부터 출력
    
    입력된 숫자가 최대 몇 자리의 정수인지를 계산하는 방법으로 
    라이브러리 함수 log10,
    x^y를 계산하는 라이브러리 함수 pow(x,y)를 이용

    length=(int)(log10(number)+1); //최대 자리수 계산
    pow(10, i-1); // 나누기를 사용할 때 이용 /1, /10 , /100 ...

    낮은 단위부터 출력

    나머지 연산자를 이용하여 10으로 나눈 나머지를 이용
    void reverse_number(long number)
    {
        while(number>0)
        {
        printf("%ld\n", number%10);
        number/=10;
        }
    }

### 재귀적 호출을 이용

    자기 자신을 호출하는 재귀 함수를 사용

    높은 자리부터 출력

    (123/100) % 10 > 1	
    (123/10) % 10 > 2	
    (123) % 10 > 3	

    void serial_number(long number)
    {
        if (number>0)
        {
            serial_number(number/10);
            printf("%ld\n", number%10);
        }
        else
            return;
    }

    자기 자신을 먼저 호출해서 printf를 보류하고, 반환 과정에서 보류된 출력이 역순으로 실행
    
    낮은 자리부터 출력
    
    (123) % 10 > 3	
    (123/10) % 10 > 2	
    (123/100) % 10 > 1	
    
    void reverse_number(long number)
    {
        printf("%ld\n", number%10);
        if((number/10)>0)
            reverse_number(number/10);
        else
            return;
    }

## 디지털 숫자

### 숫자 출력
     각 배열의 값이 1인 경우는 기호 ■을 출력하고 값이 0인 경우에는 두 자리의 공백을 출력
    
    | 해당 숫자 | 1 | 2 | 3 |
    |:---------:|:----:|:----:|:----:|
    | 출력될 1 번째 줄 | `■   ` | `■■■■` | `■■■■` |
    | 출력될 2 번째 줄 | `■   ` | `   ■` | `   ■` |
    | 출력될 3 번째 줄 | `■   ` | `■■■■` | `■■■■` |
    | 출력될 4 번째 줄 | `■   ` | `■   ` | `   ■` |
    | 출력될 5 번째 줄 | `■   ` | `■■■■` | `■■■■` |


    0~9를 int 배열 20칸에 저장
    ex> int zero[20]  = {1,1,1,1, 
                         1,0,0,1, 
                         1,0,0,1, 
                         1,0,0,1, 
                         1,1,1,1};
                            
    void digit_print(int dim[], int line) // 디지털 숫자 출력
    {
        int i;
        for(i=line*4;i<=line*4+3; i++)
            if (dim[i]==1)
                printf("■");
            else
                printf(" ");
            printf(" ");
    }

### 줄 분리
    연속적으로 줄을 분리해 가면서 출력해야 하므로, 출력할 줄마다 각 자리수의 숫자를 차례로 출력해야 하므로 재귀적 호출 방법을 이용

    number_check : 입력된 숫자에 해당하는 배열로 구분하여 호출
    digit_print : 배열의 이름을 인자로 하여 디지털 숫자를 출력

    number_check(1, 0 1 2 3 4) > 출력 라인 1,2,3,4,5 줄
    
    void number_check(int k, int i)
    {
        if (k>0)
        {
            number_check(k/10, i);
            switch(k%10)
        {
            case 0 :
                digit_print(zero, i);
                break;
            case 1 :
                digit_print(one, i);
                break;
            case 2 :
                digit_print(two, i);
                break;
            case 3 :
                digit_print(three, i);
                break;
            case 4 :
                digit_print(four, i);
                break;
            case 5 :
                digit_print(five, i);
                break;
            case 6 :
                digit_print(six, i);
                break;
            case 7 :
                digit_print(seven, i);
                break;
            case 8 :
                digit_print(eight, i);
                break;
            case 9 :
                digit_print(nine, i);
                break;
            }
        }
    }

 ## 수평 수직 슬라이드바
    슬라이드 바(slide bar)는 마우스를 이용한 제어에 유용하게 사용되는 인터페이스 도구

    수평과 수직 슬라이드 바의 크기는 입력 값에 의해 결정하고, 바의 표현은 직사각형을 그리는 함수인 draw_rectangle을 이용

    수직 슬라이드바 표시
    
    void draw_vertical_slide(int x, int y, int length, char *s)
    {
        gotoxy(x, 1);
        draw_rectangle(1, length);
        gotoxy(x+1, y+1);
        printf("%s", s);
        gotoxy(x+6, length+1);
        printf("%2d", y);
    }

    수평 슬라이드바 표시

    void draw_horizontal_slide(int x, int y, int length, char *s)
    {
        int real_length=length;
        gotoxy(1, y);
        draw_rectangle(real_length+1, 1);
        gotoxy(x+2, y+1);
        printf("%s", s);
        gotoxy(real_length + 4, y);
        printf("   ");
        gotoxy(real_length*2+2, y-1);
        printf("%2d", x);
    }

    긴 막대부분(bar)의 표시는 사각형을 그리는 함수 draw_rectangle을 이용
    
    직사각형 그리기

    void draw_rectangle(int c, int r)
    {
        int i, j;
        unsigned char a=0xa6;
        unsigned char b[7];
        for(i=1;i<7;i++)
            b[i]=0xa0+i;
        printf("%c%c",a, b[3]);
        for(i=0;i<c;i++)
            printf("%c%c", a, b[1]);
        printf("%c%c", a, b[4]);
        printf("\n");
        for(i=0;i<r;i++)
        {
            printf("%c%c", a, b[2]);
            for(j=0;j<c;j++)
                printf(" ");
            printf("%c%c",a, b[2]);
            printf("\n");
        }
        printf("%c%c", a, b[6]);
        for(i=0;i<c;i++)
            printf("%c%c", a, b[1]);
        printf("%c%c", a, b[5]);
        printf("\n");
    }


    화살표 키의 이동은 함수 move_arrow_key를 사용

    화살표 키의 제어

    void move_arrow_key(char key, int *x1, int *y1, int x_b, int y_b)
    {
        switch(key)
        {
        case 72: //↑ 
            *y1=*y1-1;
            if (*y1<1) *y1=1; //y좌표의 최소값
            break;
        case 75: //←
            *x1=*x1-1;
            if (*x1<1) *x1=1; //x좌표의 최소값
            break;
        case 77: //→ 
            *x1=*x1+1;
            if (*x1>x_b) *x1=x_b; //x좌표의 최대값
            break;
        case 80: //↓
            *y1=*y1+1;
            if (*y1>y_b) *y1=y_b; //y좌표의 최대값
            break;
        default:
            return;
        }
    }

    커서의 위치 제어

    void gotoxy(int x, int y)
    {
        COORD Pos = {x - 1, y - 1};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
    }

    현재의 크기를 나타낼 검은 사각형 부분(슬라이드, slide)은 확장 아스키코드에서 기호 ■을 사용
    
    char *slide="■", key;
    