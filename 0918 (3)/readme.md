# 게임프로그래밍 3주차 수업내용 0918

## C언어 컴파일 단위
    C언어 컴파일 단위 = 파일 (.c)

## getch()의 반환값
    getch() 함수는 키보드로 입력한 숫자 키를 ASCII 코드값 형태로 반환
    문자 0의 ASCII 코드값은 48
    정수 값으로 변환하기 위해 0의 ASCII 값인 48을 빼서 getch() - 48

## 모듈화
    void hamburger(void)
    {
        system("cls");                  
        printf("햄버거 만드는 방법\n"); 
        printf("중략...\n");
        press_any_key(); 
    }

    모듈화 장점
    가독성 및 관리 용이성
    유지보수의 편리함
    코드 재사용성

## 난수와 시드
    rand()%n : 0 ~ n-1 사이의 난수 생성 
    seed : rand() 난수 계산 공식의 시작점, 시드가 같으면 동일한 순서의 난수열 생성
    srand() : rand()의 seed를 설정

    time()을 seed로 사용하는 이유
    time()은 현재 시간이라 프로그램을 실행할 때마다 시간이 달라지므로 seed 값도 매번 달라짐
    > 실행할 때마다 시드값이 달라져, 진짜 랜덤처럼 보임

## 가변 인수
    가변 인수는 함수를 호출할 때 인수의 개수가 고정되어 있지 않고 변할 수 있는 인수
    ex> printf, scanf
    가변 인수를 처리하는 함수를 정의할 때 va_start, va_arg 그리고 va_end라는 매크로 함수를 사용

    va_list : 가변 인수를 처리하는데 있어서 필요한 정보를 보관할 포인터 변수를 정의하기 위해 사용
    va_start : 두 개의 매개변수(ap와 lastfix)를 취합니다. 이 함수는 va_list 형 변수 ap를 초기화 하므로 함수 va_arg 또는 va_end을 호출하기 전에 사용
    va_arg : 초기화가 완료된 va_list 형 변수 ap로부터 차례로 인수들을 반환하기위해 사용
    va_end : 호출된 함수가 정상적인 반환을 수행할 수 있도록 도와주는 함수

## 카드
    트럼프 카드는 4개의 모양(♠♥♣◆)으로 나누어져 있으며 각 모양은 13장 (1~10,JQK)의 카드로 구성되므로 모두 52장이 사용
    숫자, 모양 두개의 데이터이므로 구조체 사용
    struct card
    (
            char order;
            char number;
    )
    
    struct card
    (
            char order; //카드 우선순위
            char shape[3]; // 카드 모양
            char number; // 카드 숫자
    )
    trump card[52];

    카드제작, 셔플, 출력