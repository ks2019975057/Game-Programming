# 게임프로그래밍 1주차 수업내용 0904

## 커서의 위치 이동
    함수 gotoxy를 이용
    가로 : 1~80
    세로 : 1~25

    void gotoxy(int x, int y)
    {
        COORD Pos = {x - 1, y - 1}; // -1은 컴퓨터의 좌표계 시작점(0)과 우리가 인식하는 화면의 시작점(1)의 차이를 맞춰주기 위함
        SetConsoleCursorPosition (GetStdHandle (STD_OUTPUT_HANDLE), Pos);
    }

## 화면 지우기 
    #include <stdlib.h>
    system("cls");


