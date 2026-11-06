#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <algorithm>
#include <cmath>

// ==========================================
// [키코드 상수 정의]
// ==========================================
#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_LEFT  75
#define KEY_RIGHT 77
#define KEY_ESC   27
#define KEY_SPACE 32
#define KEY_ENTER 13
#define KEY_F1    59

// ==========================================
// [콘솔 제어 및 깜빡임 방지 엔진]
// ==========================================
enum ConsoleColor {
    COLOR_BLACK = 0,
    COLOR_DARK_BLUE = 1,
    COLOR_DARK_GREEN = 2,
    COLOR_DARK_CYAN = 3,
    COLOR_DARK_RED = 4,
    COLOR_DARK_MAGENTA = 5,
    COLOR_DARK_YELLOW = 6,
    COLOR_GRAY = 7,
    COLOR_DARK_GRAY = 8,
    COLOR_BLUE = 9,
    COLOR_GREEN = 10,
    COLOR_CYAN = 11,
    COLOR_RED = 12,
    COLOR_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15
};

void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

void gotoxy(int x, int y) {
    COORD pos;
    pos.X = (SHORT)x;
    pos.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void init_console_font() {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 12;
    cfi.dwFontSize.Y = 24;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_BOLD;
    lstrcpyW(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void set_cursor_visibility(bool visible) {
    CONSOLE_CURSOR_INFO cursor_info;
    cursor_info.bVisible = visible ? TRUE : FALSE;
    cursor_info.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// ==========================================
// [입력 안전 처리기 : 문자 입력 시 먹통 버그 방지]
// ==========================================
int safe_input_int(int min_v, int max_v, int default_v) {
    int val;
    if (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return default_v;
    }
    std::cin.ignore(10000, '\n');
    if (val < min_v) val = min_v;
    if (val > max_v) val = max_v;
    return val;
}

double safe_input_double(double min_v, double max_v, double default_v) {
    double val;
    if (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return default_v;
    }
    std::cin.ignore(10000, '\n');
    if (val < min_v) val = min_v;
    if (val > max_v) val = max_v;
    return val;
}

// ==========================================
// [사운드 및 환경 설정 관리자]
// ==========================================
bool g_opt_bgm = true;
bool g_opt_sfx = true;

bool g_bgm_running = false;
HANDLE g_bgm_thread = NULL;

DWORD WINAPI bgm_worker(LPVOID lpParam) {
    int notes[] = { 523, 659, 784, 1046 }; // C5, E5, G5, C6
    int note_len = 160;
    int gap_len  = 140;

    int idx = 0;
    while (g_bgm_running) {
        if (g_opt_bgm) {
            Beep(notes[idx], note_len);
        }
        idx = (idx + 1) % 4;

        for (int i = 0; i < gap_len / 20; i++) {
            if (!g_bgm_running) return 0;
            Sleep(20);
        }
    }
    return 0;
}

void start_lobby_bgm() {
    if (!g_opt_bgm) return;
    if (!g_bgm_running) {
        g_bgm_running = true;
        g_bgm_thread = CreateThread(NULL, 0, bgm_worker, NULL, 0, NULL);
    }
}

void stop_lobby_bgm() {
    if (g_bgm_running) {
        g_bgm_running = false;
        if (g_bgm_thread) {
            WaitForSingleObject(g_bgm_thread, 1000);
            CloseHandle(g_bgm_thread);
            g_bgm_thread = NULL;
        }
    }
}

void play_sfx(int type) {
    if (!g_opt_sfx) return;
    switch (type) {
    case 1: Beep(1046, 120); Beep(1318, 180); break;
    case 2: Beep(330, 200); Beep(261, 300); break;
    case 3: Beep(784, 80); break;
    case 4: Beep(1568, 150); break;
    case 5: Beep(440, 250); break;
    }
}

// ==========================================
// [데이터 저장소 : 모드별 & 난이도별 랭킹]
// ==========================================
struct RankEntry {
    std::string mode_category;
    std::string difficulty;
    std::string player_name;
    double record_val;
    std::string unit;

    RankEntry(std::string cat, std::string diff, std::string p, double r, std::string u) 
        : mode_category(cat), difficulty(diff), player_name(p), record_val(r), unit(u) {}
    RankEntry() : mode_category(""), difficulty(""), player_name(""), record_val(0.0), unit("") {}
};

struct PlayerStats {
    int total_games;
    int total_wins;
    double total_response_time;
    int max_combo;
    int hints_used;
    PlayerStats() : total_games(0), total_wins(0), total_response_time(0.0), max_combo(0), hints_used(0) {}
};

std::vector<RankEntry> load_rankings(const std::string& filename = "rank.txt") {
    std::vector<RankEntry> ranks;
    std::ifstream fin(filename.c_str());
    if (!fin.is_open()) return ranks;
    RankEntry r;
    while (fin >> r.mode_category >> r.difficulty >> r.player_name >> r.record_val >> r.unit) {
        ranks.push_back(r);
    }
    fin.close();
    return ranks;
}

void save_ranking(const RankEntry& entry, const std::string& filename = "rank.txt") {
    std::vector<RankEntry> ranks = load_rankings(filename);
    ranks.push_back(entry);
    std::ofstream fout(filename.c_str());
    if (!fout.is_open()) return;
    for (size_t i = 0; i < ranks.size(); i++) {
        fout << ranks[i].mode_category << " " 
             << ranks[i].difficulty << " " 
             << ranks[i].player_name << " " 
             << ranks[i].record_val << " " 
             << ranks[i].unit << "\n";
    }
    fout.close();
}

void clear_rankings_file(const std::string& filename = "rank.txt") {
    std::ofstream fout(filename.c_str(), std::ios::trunc);
    if (fout.is_open()) fout.close();
}

PlayerStats load_stats(const std::string& filename = "stats.txt") {
    PlayerStats st;
    std::ifstream fin(filename.c_str());
    if (fin.is_open()) {
        fin >> st.total_games >> st.total_wins >> st.total_response_time >> st.max_combo >> st.hints_used;
        fin.close();
    }
    return st;
}

void update_stats(bool is_win, double resp_time, int combo, bool used_hint, const std::string& filename = "stats.txt") {
    PlayerStats st = load_stats(filename);
    st.total_games++;
    if (is_win) {
        st.total_wins++;
        st.total_response_time += resp_time;
    }
    if (combo > st.max_combo) st.max_combo = combo;
    if (used_hint) st.hints_used++;

    std::ofstream fout(filename.c_str());
    if (fout.is_open()) {
        fout << st.total_games << " " << st.total_wins << " " << st.total_response_time << " " << st.max_combo << " " << st.hints_used << "\n";
        fout.close();
    }
}

void clear_stats_file(const std::string& filename = "stats.txt") {
    std::ofstream fout(filename.c_str(), std::ios::trunc);
    if (fout.is_open()) {
        fout << "0 0 0.0 0 0\n";
        fout.close();
    }
}

double get_size_multiplier(int size) { return size * 0.15; }
double get_symbol_multiplier(int symbols) { return symbols * 0.10; }
double get_time_multiplier(double t_limit) { return (std::max)(0.0, (10.0 - t_limit) * 0.08); }

double calculate_score_multiplier(int size, int symbols, double t_limit) {
    double mult = get_size_multiplier(size) + get_symbol_multiplier(symbols) + get_time_multiplier(t_limit);
    if (mult < 0.5) mult = 0.5;
    return mult;
}

// ==========================================
// 공용 UI 및 모드별 맞춤 도움말
// ==========================================
std::string get_game_rules_text() {
    return "  [ 게임 규칙 & 조작 ]\n"
           "  1. 격자 안에 가장 많이 나온 기호를 찾으세요.\n"
           "  2. 올바른 기호를 찾아 입력하면 점수를 얻습니다.\n"
           "  3. [0 ~ 9 / A ~ Z] 정답 입력\n"
           "  4. [H] 힌트(가장 적게 나온 기호 2개가 사라집니다. 라운드별 1회, 감점 50%)\n"
           "  5. 반응 속도에 따라 S(+50), A(+30), B(+15), C(+5) 보너스\n"
           "  6. 연속 정답 시 콤보 배율이 누적되어 최대 2.0배 보너스\n"
           "  7. [ESC] 일시정지 (타이머 동결) | [F1] 실시간 도움말 호출";
}

void show_f1_custom_help(const std::string& mode_type) {
    system("cls");
    set_color(COLOR_YELLOW);
    std::cout << "========================================================\n";
    std::cout << "               [ GAME GUIDE & HELP (F1) ]               \n";
    std::cout << "========================================================\n";
    set_color(COLOR_WHITE);

    if (mode_type == "Classic") {
        std::cout << get_game_rules_text() << "\n\n";
        set_color(COLOR_CYAN);
        std::cout << "  [ 보너스 & 랭크 규칙 ]\n";
        std::cout << "  - 반응 속도에 따라 S(+50), A(+30), B(+15), C(+5) 보너스\n";
        std::cout << "  - 연속 정답 시 콤보 배율이 누적되어 최대 2.0배 가산\n";
    } else if (mode_type == "Challenge") {
        std::cout << "  [ 챌린지 모드 공통 규칙 ]\n"
                  << "  - 총 5라운드를 완주할 때까지의 시간을 합산합니다.\n"
                  << "  - ★ 오답 입력 시 즉시 10.0초의 시간 페널티가 가산됩니다!\n\n"
                  << "  [ 미션별 특수 기믹 ]\n"
                  << "  1. 깜빡임    : 격자가 주기적으로 암전됩니다.\n"
                  << "  2. 위치 셔플 : 1초마다 기호가 인접 기호와 위치를 바꿉니다.\n"
                  << "  3. 손전등    : 방향키(↑↓←→)로 손전등을 비춰가며 기호를 찾으세요.\n";
    } else if (mode_type == "Arcade_Survival") {
        std::cout << "  [ 서바이벌 규칙 ]\n"
                  << "  - 하트 3개로 시작하여 오답 또는 시간 초과마다 하트 1개가 차감됩니다.\n"
                  << "  - 생명이 모두 소진될 때까지 도달한 최고 라운드로 순위를 겨룹니다.\n"
                  << "  - [ESC]를 눌러 타이머를 멈추고 게임을 일시정지할 수 있습니다.\n";
    } else if (mode_type == "Arcade_ClearOut") {
        std::cout << "  [ 전체 삭제 규칙 ]\n"
                  << "  - 보드에 현재 가장 많이 남아 있는 기호를 순서대로 맞히세요.\n"
                  << "  - 맞힌 숫자는 격자에서 사라집니다.\n"
                  << "  - 모든 숫자를 사라지게 만드세요.\n"
                  << "  - ★ 잘못된 숫자를 입력하면 +10.0초 페널티가 누적됩니다!\n"
                  << "  - [ESC]를 눌러 타이머를 멈추고 일시정지 메뉴를 열 수 있습니다.\n";
    }

    set_color(COLOR_YELLOW);
    std::cout << "========================================================\n";
    set_color(COLOR_GRAY);
    std::cout << "아무 키나 누르면 게임으로 복귀합니다...";
    _getch();
}

int show_pause_menu() {
    play_sfx(5);
    system("cls");
    set_color(COLOR_RED);
    std::cout << "========================================\n";
    std::cout << "            [ GAME PAUSED ]             \n";
    std::cout << "========================================\n";
    set_color(COLOR_WHITE);
    std::cout << "  1. 게임 계속하기 (Resume)\n";
    std::cout << "  2. 1라운드부터 재도전 (Restart)\n";
    std::cout << "  3. 메인 메뉴로 나가기 (Quit)\n";
    set_color(COLOR_RED);
    std::cout << "========================================\n";
    set_color(COLOR_GRAY);
    std::cout << "선택할 번호를 누르세요 > ";
    while (true) {
        char ch = _getch();
        if (ch == '1') return 1;
        if (ch == '2') return 2;
        if (ch == '3') return 3;
    }
}

char get_symbol_char(int val, bool is_alpha) {
    if (is_alpha) return (char)('A' + val);
    return (char)('0' + val);
}

void generate_board_with_unique_mode(int size, int symbol_count, std::vector<std::vector<int> >& board, int& correct_ans) {
    board.assign(size, std::vector<int>(size, 0));
    while (true) {
        std::vector<int> counts(symbol_count, 0);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int r = rand() % symbol_count;
                board[i][j] = r;
                counts[r]++;
            }
        }
        int max_c = -1, best_val = -1;
        int tie_c = 0;
        for (int v = 0; v < symbol_count; v++) {
            if (counts[v] > max_c) {
                max_c = counts[v];
                best_val = v;
                tie_c = 1;
            } else if (counts[v] == max_c) {
                tie_c++;
            }
        }
        if (tie_c == 1) {
            correct_ans = best_val;
            break;
        }
    }
}

void render_board_frame(const std::vector<std::vector<int> >& board, int size, bool is_alpha, 
                        int highlight_val = -1, const std::vector<bool>& excluded = std::vector<bool>(), 
                        bool mask_all = false, int light_r = -1, int light_c = -1, int light_radius = 1) {
    set_color(COLOR_GRAY);
    std::cout << "+";
    for (int j = 0; j < size; j++) std::cout << "---+";
    std::cout << "\n";

    for (int i = 0; i < size; i++) {
        std::cout << "|";
        for (int j = 0; j < size; j++) {
            int v = board[i][j];
            bool in_light = true;
            if (light_r != -1 && light_c != -1) {
                in_light = (abs(i - light_r) <= light_radius && abs(j - light_c) <= light_radius);
            }

            if (!in_light) {
                set_color(COLOR_DARK_GRAY);
                std::cout << " * ";
                set_color(COLOR_GRAY);
            } else if (mask_all) {
                set_color(COLOR_DARK_GRAY);
                std::cout << " ? ";
                set_color(COLOR_GRAY);
            } else if (highlight_val != -1 && v == highlight_val) {
                set_color(COLOR_GREEN);
                std::cout << " " << get_symbol_char(v, is_alpha) << " ";
                set_color(COLOR_GRAY);
            } else if (!excluded.empty() && v >= 0 && v < (int)excluded.size() && excluded[v]) {
                set_color(COLOR_DARK_RED);
                std::cout << " . ";
                set_color(COLOR_GRAY);
            } else if (v == -1) {
                std::cout << "   ";
            } else {
                set_color(COLOR_WHITE);
                std::cout << " " << get_symbol_char(v, is_alpha) << " ";
                set_color(COLOR_GRAY);
            }
            std::cout << "|";
        }
        std::cout << "\n+";
        for (int j = 0; j < size; j++) std::cout << "---+";
        std::cout << "\n";
    }
}

void render_timer_gauge_at(int x, int y, double remaining, double total) {
    gotoxy(x, y);
    int bar_width = 20;
    double ratio = (std::max)(0.0, remaining / total);
    int filled = (int)(ratio * bar_width);

    set_color(COLOR_WHITE);
    std::cout << " [TIME: ";
    if (ratio > 0.5) set_color(COLOR_GREEN);
    else if (ratio > 0.2) set_color(COLOR_YELLOW);
    else set_color(COLOR_RED);

    for (int i = 0; i < filled; i++) std::cout << "=";
    set_color(COLOR_DARK_GRAY);
    for (int i = filled; i < bar_width; i++) std::cout << ".";

    set_color(COLOR_WHITE);
    std::cout << "] " << std::fixed << std::setprecision(1) << (std::max)(0.0, remaining) << "s    ";
}

struct ChallengeCustomConfig {
    double blink_show_time;
    double blink_hide_time;
    double shuffle_interval;
    int shuffle_swap_count;
    int flashlight_radius;

    ChallengeCustomConfig() : blink_show_time(2.0), blink_hide_time(2.0), 
                              shuffle_interval(1.0), shuffle_swap_count(2), flashlight_radius(1) {}
};

bool select_challenge_difficulty(const std::string& mode_tag, int& out_size, int& out_symbols, 
                                 std::string& out_diff_name, ChallengeCustomConfig& cfg) {
    bool is_flashlight = (mode_tag == "Flashlight");
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "       [ 챌린지 : " << mode_tag << " 난이도 설정 ]           \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        if (is_flashlight) {
            std::cout << "  1. 쉬움   (5x5 격자 | 4종류)\n";
            std::cout << "  2. 보통   (7x7 격자 | 6종류)\n";
            std::cout << "  3. 어려움 (9x9 격자 | 8종류)\n";
        } else {
            std::cout << "  1. 쉬움   (3x3 격자 | 3종류)\n";
            std::cout << "  2. 보통   (5x5 격자 | 5종류)\n";
            std::cout << "  3. 어려움 (7x7 격자 | 8종류)\n";
        }
        set_color(COLOR_CYAN);
        std::cout << "  4. 커스텀 난이도 [※ 랭킹 집계 제외 (X)]\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 이전 미션 선택으로 돌아가기\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 [1~4/ESC] > ";

        char ch = _getch();
        if (ch == KEY_ESC) return false;

        if (ch == '1') {
            out_size = is_flashlight ? 5 : 3;
            out_symbols = is_flashlight ? 4 : 3;
            out_diff_name = "Easy";
            cfg.shuffle_swap_count = 1;
            cfg.shuffle_interval = 1.0;
            cfg.flashlight_radius = 1;
            return true;
        } else if (ch == '2') {
            out_size = is_flashlight ? 7 : 5;
            out_symbols = is_flashlight ? 6 : 5;
            out_diff_name = "Normal";
            cfg.shuffle_swap_count = 2;
            cfg.shuffle_interval = 1.0;
            cfg.flashlight_radius = 1;
            return true;
        } else if (ch == '3') {
            out_size = is_flashlight ? 9 : 7;
            out_symbols = 8;
            out_diff_name = "Hard";
            cfg.shuffle_swap_count = 3;
            cfg.shuffle_interval = 1.0;
            cfg.flashlight_radius = 1;
            return true;
        } else if (ch == '4') {
            out_diff_name = "Custom";
            system("cls");
            set_color(COLOR_CYAN);
            std::cout << "========================================================\n";
            std::cout << "   [ " << mode_tag << " 커스텀 상세 세팅 (랭킹 제외) ]  \n";
            std::cout << "========================================================\n";
            set_color(COLOR_WHITE);
            std::cout << "격자 크기 입력 (3 ~ 9) [기본: 5] > ";
            out_size = safe_input_int(3, 9, 5);

            std::cout << "기호 종류 수 입력 (2 ~ 10) [기본: 5] > ";
            out_symbols = safe_input_int(2, 10, 5);

            if (mode_tag == "Blink") {
                std::cout << "노출 시간(초) 입력 (0.5 ~ 5.0) [기본: 2.0] > ";
                cfg.blink_show_time = safe_input_double(0.5, 5.0, 2.0);

                std::cout << "암전 시간(초) 입력 (0.5 ~ 5.0) [기본: 2.0] > ";
                cfg.blink_hide_time = safe_input_double(0.5, 5.0, 2.0);
            } else if (mode_tag == "Shuffle") {
                std::cout << "셔플 변경 주기(초) 입력 (0.3 ~ 3.0) [기본: 1.0] > ";
                cfg.shuffle_interval = safe_input_double(0.3, 3.0, 1.0);

                std::cout << "한 번에 뒤바뀔 셀 쌍의 개수 (1 ~ 5) [기본: 2] > ";
                cfg.shuffle_swap_count = safe_input_int(1, 5, 2);
            } else if (mode_tag == "Flashlight") {
                std::cout << "손전등 시야 크기 선택 (1: 1x1 | 2: 3x3 | 3: 5x5) [기본: 2] > ";
                int f_opt = safe_input_int(1, 3, 2);
                if (f_opt == 1) cfg.flashlight_radius = 0;
                else if (f_opt == 3) cfg.flashlight_radius = 2;
                else cfg.flashlight_radius = 1;
            }
            return true;
        }
    }
}

bool select_arcade_difficulty(const std::string& mode_name, int& out_size, int& out_symbols, double& out_time, std::string& out_diff_name) {
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "       [ " << mode_name << " : 난이도 설정 ]           \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  1. 쉬움   (3x3 격자 | 3종류)\n";
        std::cout << "  2. 보통   (5x5 격자 | 5종류)\n";
        std::cout << "  3. 어려움 (7x7 격자 | 8종류)\n";
        set_color(COLOR_CYAN);
        std::cout << "  4. 사용자 정의 커스텀 (격자/기호 직접 조율)\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 이전 미션 선택으로 돌아가기\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 [1~4/ESC] > ";

        char ch = _getch();
        if (ch == KEY_ESC) return false;

        if (ch == '1') {
            out_size = 3; out_symbols = 3; out_time = 3.0; out_diff_name = "Easy";
            return true;
        } else if (ch == '2') {
            out_size = 5; out_symbols = 5; out_time = 5.0; out_diff_name = "Normal";
            return true;
        } else if (ch == '3') {
            out_size = 7; out_symbols = 8; out_time = 8.0; out_diff_name = "Hard";
            return true;
        } else if (ch == '4') {
            out_diff_name = "Custom";
            system("cls");
            set_color(COLOR_CYAN);
            std::cout << "========================================\n";
            std::cout << "       [ 커스텀 난이도 세부 세팅 ]      \n";
            std::cout << "========================================\n";
            set_color(COLOR_WHITE);
            std::cout << "격자 크기 입력 (3 ~ 9) [기본: 5] > ";
            out_size = safe_input_int(3, 9, 5);

            std::cout << "기호 종류 수 입력 (2 ~ 10) [기본: 5] > ";
            out_symbols = safe_input_int(2, 10, 5);
            out_time = 5.0;
            return true;
        }
    }
}

// ==========================================
// [MODE 1] 클래식 모드
// ==========================================
void run_classic_mode() {
    stop_lobby_bgm();
    int matrix_size = 5;
    int symbol_count = 5;
    double time_limit = 5.0;
    bool is_alpha = false;
    int total_rounds = 10;

    while (true) {
        system("cls");
        double m_size_val = get_size_multiplier(matrix_size);
        double m_sym_val  = get_symbol_multiplier(symbol_count);
        double m_time_val = get_time_multiplier(time_limit);
        double total_mult = calculate_score_multiplier(matrix_size, symbol_count, time_limit);

        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "          [ 클래식 모드 : 자유 설정 및 대기실 ]        \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  1. 격자 크기    : " << matrix_size << " x " << matrix_size 
                  << "  (기여 배율: +x" << std::fixed << std::setprecision(2) << m_size_val << ")\n";
        std::cout << "  2. 기호 범위    : " << symbol_count << "종류   " 
                  << "  (기여 배율: +x" << std::fixed << std::setprecision(2) << m_sym_val << ")\n";
        std::cout << "  3. 제한 시간    : " << std::fixed << std::setprecision(1) << time_limit << "초     "
                  << "  (기여 배율: +x" << std::fixed << std::setprecision(2) << m_time_val << ")\n";
        std::cout << "  4. 기호 형태    : " << (is_alpha ? "알파벳 (A~Z)" : "숫자 (0~9)") << "\n";
        std::cout << "--------------------------------------------------------\n";
        set_color(COLOR_GREEN);
        std::cout << "  ★ 최종 합산 점수 배율 : x" << std::fixed << std::setprecision(2) << total_mult << " 배율 적용 중!\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << get_game_rules_text() << "\n";
        std::cout << "--------------------------------------------------------\n";
        set_color(COLOR_CYAN);
        std::cout << "  [Space] 게임 시작!  [1~3] 수치 변경 기호 형태 전환  [ESC] 뒤로\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";

        char ch = _getch();
        if (ch == KEY_SPACE) break;
        if (ch == KEY_ESC) return;
        if (ch == '1') {
            std::cout << "\n원하는 격자 크기(3~9) 입력 > ";
            matrix_size = safe_input_int(3, 9, matrix_size);
        } else if (ch == '2') {
            std::cout << "\n원하는 기호 종류 수(2~10) 입력 > ";
            symbol_count = safe_input_int(2, 10, symbol_count);
        } else if (ch == '3') {
            std::cout << "\n원하는 제한 시간(초) 입력 (2.0~20.0) > ";
            time_limit = safe_input_double(2.0, 20.0, time_limit);
        } else if (ch == '4') {
            is_alpha = !is_alpha;
        }
    }

    int current_round = 1;
    int total_score = 0;
    int combo = 0;
    int max_combo = 0;

    while (current_round <= total_rounds) {
        std::vector<std::vector<int> > board;
        int correct_ans = 0;
        generate_board_with_unique_mode(matrix_size, symbol_count, board, correct_ans);

        std::vector<bool> excluded(symbol_count, false);
        bool round_hint_used = false;
        double mult = calculate_score_multiplier(matrix_size, symbol_count, time_limit);
        double cur_combo_mult = (combo > 0) ? (std::min)(2.0, 1.0 + (combo - 1) * 0.1) : 1.0;

        system("cls");
        set_color(COLOR_CYAN);
        std::cout << "========================================================\n";
        std::cout << " [ ROUND " << current_round << " / " << total_rounds << " ]"
                  << " SCORE: " << total_score << " | COMBO: " << combo << "연속 (x" 
                  << std::fixed << std::setprecision(1) << cur_combo_mult << "배)\n";
        std::cout << "========================================================\n";
        render_board_frame(board, matrix_size, is_alpha, -1, excluded);
        
        int timer_y = matrix_size * 2 + 4;
        gotoxy(0, timer_y + 1);
        set_color(COLOR_WHITE);
        std::cout << "가장 많이 출현한 기호 입력 > ";

        clock_t start_clock = clock();
        double elapsed_paused = 0.0;
        bool time_out = false;

        while (true) {
            double current_elapsed = (double)(clock() - start_clock) / CLOCKS_PER_SEC - elapsed_paused;
            double remaining = time_limit - current_elapsed;

            if (remaining <= 0.0) {
                time_out = true;
                break;
            }

            render_timer_gauge_at(0, timer_y, remaining, time_limit);

            if (_kbhit()) {
                int key = _getch();
                if (key == KEY_ESC) {
                    clock_t pause_start = clock();
                    int p_act = show_pause_menu();
                    elapsed_paused += (double)(clock() - pause_start) / CLOCKS_PER_SEC;
                    if (p_act == 2) { current_round = 1; total_score = 0; combo = 0; break; }
                    if (p_act == 3) return;
                    
                    system("cls");
                    set_color(COLOR_CYAN);
                    std::cout << "========================================================\n";
                    std::cout << " [ ROUND " << current_round << " / " << total_rounds << " ]"
                              << " SCORE: " << total_score << " | COMBO: " << combo << "연속 (x" 
                              << std::fixed << std::setprecision(1) << cur_combo_mult << "배)\n";
                    std::cout << "========================================================\n";
                    render_board_frame(board, matrix_size, is_alpha, -1, excluded);
                    gotoxy(0, timer_y + 1);
                    set_color(COLOR_WHITE);
                    std::cout << "가장 많이 출현한 기호 입력 > ";
                    continue;
                }
                if (key == 0 || key == 224) {
                    int ext = _getch();
                    if (ext == KEY_F1) {
                        clock_t pause_start = clock();
                        show_f1_custom_help("Classic");
                        elapsed_paused += (double)(clock() - pause_start) / CLOCKS_PER_SEC;
                        system("cls");
                        set_color(COLOR_CYAN);
                        std::cout << "========================================================\n";
                        std::cout << " [ ROUND " << current_round << " / " << total_rounds << " ]"
                                  << " SCORE: " << total_score << " | COMBO: " << combo << "연속 (x" 
                                  << std::fixed << std::setprecision(1) << cur_combo_mult << "배)\n";
                        std::cout << "========================================================\n";
                        render_board_frame(board, matrix_size, is_alpha, -1, excluded);
                        gotoxy(0, timer_y + 1);
                        set_color(COLOR_WHITE);
                        std::cout << "가장 많이 출현한 기호 입력 > ";
                    }
                    continue;
                }
                if (key == 'h' || key == 'H') {
                    if (!round_hint_used) {
                        play_sfx(4);
                        round_hint_used = true;

                        // 1. 각 기호별 보드 출현 빈도 계산
                        std::vector<int> counts(symbol_count, 0);
                        for (int r = 0; r < matrix_size; r++) {
                            for (int c = 0; c < matrix_size; c++) {
                                counts[board[r][c]]++;
                            }
                        }

                        // 2. 보드에 실제로 1개 이상 존재하고 정답이 아닌 오답 기호만 선별
                        std::vector<std::pair<int, int> > candidates;
                        for (int i = 0; i < symbol_count; i++) {
                            if (i != correct_ans && counts[i] > 0) {
                                candidates.push_back(std::make_pair(counts[i], i));
                            }
                        }
                        std::sort(candidates.begin(), candidates.end());

                        // 3. 가장 적게 나온 최대 2개 기호 소거 대상으로 설정
                        int remove_target = (std::min)(2, (int)candidates.size());
                        for (int i = 0; i < remove_target; i++) {
                            excluded[candidates[i].second] = true;
                        }

                        // 4. 화면 밀림을 원천 방지하기 위해 프레임 정밀 재출력
                        system("cls");
                        set_color(COLOR_CYAN);
                        std::cout << "========================================================\n";
                        std::cout << " [ ROUND " << current_round << " / " << total_rounds << " ]"
                                  << " SCORE: " << total_score << " | COMBO: " << combo << "연속 (x" 
                                  << std::fixed << std::setprecision(1) << cur_combo_mult << "배)\n";
                        std::cout << "========================================================\n";
                        render_board_frame(board, matrix_size, is_alpha, -1, excluded);
                        gotoxy(0, timer_y + 1);
                        set_color(COLOR_WHITE);
                        std::cout << "가장 많이 출현한 기호 입력 (힌트 적용됨: 빨간 점 '.') > ";
                    }
                    continue;
                }

                int user_ans = -1;
                if (!is_alpha && key >= '0' && key < '0' + symbol_count) {
                    user_ans = key - '0';
                } else if (is_alpha) {
                    char upper_key = (char)toupper(key);
                    if (upper_key >= 'A' && upper_key < 'A' + symbol_count) {
                        user_ans = upper_key - 'A';
                    }
                }

                if (user_ans == -1) continue;

                system("cls");
                render_board_frame(board, matrix_size, is_alpha, correct_ans, excluded);

                if (user_ans == correct_ans) {
                    play_sfx(1);
                    combo++;
                    if (combo > max_combo) max_combo = combo;
                    double used_time = current_elapsed;
                    
                    int rank_bonus = 0;
                    std::string rank_str = "C";
                    double ratio = used_time / time_limit;
                    if (ratio <= 0.25) { rank_str = "S"; rank_bonus = 50; }
                    else if (ratio <= 0.50) { rank_str = "A"; rank_bonus = 30; }
                    else if (ratio <= 0.75) { rank_str = "B"; rank_bonus = 15; }
                    else { rank_str = "C"; rank_bonus = 5; }

                    double combo_mult = (std::min)(2.0, 1.0 + (combo - 1) * 0.1);
                    int earned = (int)((100 * mult + rank_bonus) * combo_mult);
                    if (round_hint_used) earned /= 2;

                    total_score += earned;
                    set_color(COLOR_GREEN);
                    std::cout << "\n 정답입니다! [" << combo << "연속 정답! x" 
                              << std::fixed << std::setprecision(1) << combo_mult << "배] [Rank: " 
                              << rank_str << " | +" << earned << " pts]\n";
                    update_stats(true, used_time, combo, round_hint_used);
                } else {
                    play_sfx(2);
                    combo = 0;
                    set_color(COLOR_RED);
                    std::cout << "\n 오답입니다! (정답은: " << get_symbol_char(correct_ans, is_alpha) << ")\n";
                    update_stats(false, 0.0, combo, round_hint_used);
                }
                break;
            }
            Sleep(25);
        }

        if (time_out) {
            play_sfx(2);
            system("cls");
            render_board_frame(board, matrix_size, is_alpha, correct_ans, excluded);
            set_color(COLOR_RED);
            std::cout << "\n 시간 초과! (정답은: " << get_symbol_char(correct_ans, is_alpha) << ")\n";
            combo = 0;
            update_stats(false, 0.0, combo, round_hint_used);
        }

        std::cout << "\n다음 라운드로 넘어가려면 아무 키나 누르세요...";
        _getch();
        current_round++;
    }

    system("cls");
    set_color(COLOR_YELLOW);
    std::cout << "========================================================\n";
    std::cout << "         [ 클래식 모드 최종 결과 발표 (10R 완주) ]     \n";
    std::cout << "========================================================\n";
    set_color(COLOR_WHITE);
    std::cout << "  - 최종 획득 점수 : " << total_score << " 점\n";
    std::cout << "  - 최다 달성 콤보 : " << max_combo << " 연속 정답\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "랭킹 등록 > ";
    std::string name;
    std::cin >> name;
    save_ranking(RankEntry("Classic", "Score", name, (double)total_score, "pts"));
}

// ==========================================
// [MODE 2] 챌린지 모드
// ==========================================
void run_challenge_mode() {
    stop_lobby_bgm();
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "             [ 챌린지 모드 : 세부 미션 선택 ]           \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  1. 깜빡임\n";
        std::cout << "  2. 위치 셔플\n";
        std::cout << "  3. 손전등\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 메인 메뉴로 돌아가기\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 [1~3/ESC] > ";
        
        char mode_ch = _getch();
        if (mode_ch == KEY_ESC) return;
        if (mode_ch != '1' && mode_ch != '2' && mode_ch != '3') continue;

        std::string c_tag = (mode_ch == '1') ? "Blink" : (mode_ch == '2' ? "Shuffle" : "Flashlight");
        int m_size = 5, s_count = 5;
        std::string diff_name = "Normal";
        ChallengeCustomConfig cfg;

        if (!select_challenge_difficulty(c_tag, m_size, s_count, diff_name, cfg)) {
            continue;
        }

        clock_t start_all = clock();
        double total_paused = 0.0;
        double penalty_time = 0.0;
        int total_rounds = 5;

        for (int r = 1; r <= total_rounds; r++) {
            std::vector<std::vector<int> > board;
            int correct_ans = 0;
            generate_board_with_unique_mode(m_size, s_count, board, correct_ans);

            clock_t blink_clock = clock();
            bool show_board = true;
            clock_t shuffle_clock = clock();
            int light_r = m_size / 2, light_c = m_size / 2;

            system("cls");

            while (true) {
                double cur_elapsed = (double)(clock() - start_all) / CLOCKS_PER_SEC - total_paused + penalty_time;
                gotoxy(0, 0);
                set_color(COLOR_YELLOW);
                std::cout << " [ 챌린지: " << c_tag << " (" << diff_name << ") | R" << r << "/5 ]"
                          << " 경과 시간: " << std::fixed << std::setprecision(1) << cur_elapsed 
                          << "s (페널티: +" << penalty_time << "s)   \n";

                gotoxy(0, 2);
                if (mode_ch == '1') {
                    double phase_limit = show_board ? cfg.blink_show_time : cfg.blink_hide_time;
                    if ((double)(clock() - blink_clock) / CLOCKS_PER_SEC >= phase_limit) {
                        show_board = !show_board;
                        blink_clock = clock();
                    }
                    render_board_frame(board, m_size, false, -1, std::vector<bool>(), !show_board);
                    set_color(COLOR_CYAN);
                    if (show_board) {
                        std::cout << "\n [ 기억하세요! ]                     \n";
                    } else {
                        std::cout << "\n [ 암전 중! ]              \n";
                    }
                } else if (mode_ch == '2') {
                    if ((double)(clock() - shuffle_clock) / CLOCKS_PER_SEC >= cfg.shuffle_interval) {
                        int dr[] = { -1, 1, 0, 0 }, dc[] = { 0, 0, -1, 1 };
                        for (int k = 0; k < cfg.shuffle_swap_count; k++) {
                            int r1 = rand() % m_size, c1 = rand() % m_size;
                            int d = rand() % 4;
                            int nr = r1 + dr[d], nc = c1 + dc[d];
                            if (nr >= 0 && nr < m_size && nc >= 0 && nc < m_size) {
                                std::swap(board[r1][c1], board[nr][nc]);
                            }
                        }
                        shuffle_clock = clock();
                    }
                    render_board_frame(board, m_size, false);
                } else if (mode_ch == '3') {
                    render_board_frame(board, m_size, false, -1, std::vector<bool>(), false, light_r, light_c, cfg.flashlight_radius);
                    set_color(COLOR_CYAN);
                    std::cout << "\n(방향키 이동 / 숫자키 정답 입력)       \n";
                }

                set_color(COLOR_WHITE);
                std::cout << "가장 많이 나온 기호 입력 > ";

                if (_kbhit()) {
                    int key = _getch();
                    if (key == KEY_ESC) {
                        clock_t p_start = clock();
                        int p_act = show_pause_menu();
                        total_paused += (double)(clock() - p_start) / CLOCKS_PER_SEC;
                        if (p_act == 3) return;
                        system("cls");
                        continue;
                    }
                    if (key == 0 || key == 224) {
                        int ext = _getch();
                        if (ext == KEY_F1) {
                            clock_t p_start = clock();
                            show_f1_custom_help("Challenge");
                            total_paused += (double)(clock() - p_start) / CLOCKS_PER_SEC;
                            system("cls");
                        }
                        if (mode_ch == '3') {
                            if (ext == KEY_UP && light_r > 0) light_r--;
                            if (ext == KEY_DOWN && light_r < m_size - 1) light_r++;
                            if (ext == KEY_LEFT && light_c > 0) light_c--;
                            if (ext == KEY_RIGHT && light_c < m_size - 1) light_c++;
                        }
                        continue;
                    }

                    if (key >= '0' && key < '0' + s_count) {
                        int ans = key - '0';
                        system("cls");
                        set_color(COLOR_YELLOW);
                        std::cout << " [ 챌린지: " << c_tag << " (" << diff_name << ") | R" << r << "/5 ]\n\n";
                        render_board_frame(board, m_size, false, correct_ans);

                        if (ans == correct_ans) {
                            play_sfx(1);
                            set_color(COLOR_GREEN);
                            std::cout << "\n 성공! (정답: " << correct_ans << ")\n";
                            Sleep(650);
                            break;
                        } else {
                            play_sfx(2);
                            penalty_time += 10.0;
                            set_color(COLOR_RED);
                            std::cout << "\n 오답입니다! (+10초 페널티 부과 | 정답: " << correct_ans << ")\n";
                            Sleep(800);
                            break;
                        }
                    }
                }
                Sleep(30);
            }
        }

        double final_clear_time = (double)(clock() - start_all) / CLOCKS_PER_SEC - total_paused + penalty_time;
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "        챌린지 5라운드 전체 클리어 완주!                 \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  - 미션 및 난이도 : " << c_tag << " [" << diff_name << "]\n";
        std::cout << "  - 최종 기록(합산): " << std::fixed << std::setprecision(2) << final_clear_time << " 초\n";
        std::cout << "    (누적 페널티  : +" << penalty_time << " 초)\n";
        std::cout << "--------------------------------------------------------\n";

        if (diff_name == "Custom") {
            set_color(COLOR_DARK_GRAY);
            std::cout << "※ 커스텀 난이도는 랭킹에 등록되지 않습니다.\n";
            std::cout << "아무 키나 누르면 메뉴로 복귀합니다...";
            _getch();
        } else {
            std::cout << "랭킹 등록 이름 > ";
            std::string name;
            std::cin >> name;
            save_ranking(RankEntry(c_tag, diff_name, name, final_clear_time, "sec"));
        }
        break;
    }
}

// ==========================================
// [MODE 3] 아케이드 모드
// ==========================================
void run_arcade_mode() {
    stop_lobby_bgm();
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "                 [ 아케이드 모드 선택 ]                 \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  1. 서바이벌 \n";
        std::cout << "  2. 전체 삭제\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 메인 메뉴로 돌아가기\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 [1~2/ESC] > ";
        
        char mode_ch = _getch();
        if (mode_ch == KEY_ESC) return;
        if (mode_ch != '1' && mode_ch != '2') continue;

        std::string arc_tag = (mode_ch == '1') ? "Survival" : "ClearOut";
        int m_size = 5, s_count = 5;
        double t_limit = 5.0;
        std::string diff_name = "Normal";

        if (!select_arcade_difficulty(arc_tag, m_size, s_count, t_limit, diff_name)) {
            continue;
        }

        if (mode_ch == '1') {
            int lives = 3;
            int round = 1;
            while (lives > 0) {
                std::vector<std::vector<int> > board;
                int correct_ans = 0;
                generate_board_with_unique_mode(m_size, s_count, board, correct_ans);

                system("cls");
                set_color(COLOR_RED);
                std::cout << "[ LIFE: ";
                for (int i = 0; i < 3; i++) std::cout << (i < lives ? "♥ " : "♡ ");
                std::cout << "] ";
                set_color(COLOR_CYAN);
                std::cout << "ROUND: " << round << " [" << diff_name << "]\n";
                render_board_frame(board, m_size, false);

                int timer_y = m_size * 2 + 3;
                gotoxy(0, timer_y + 1);
                set_color(COLOR_WHITE);
                std::cout << "가장 많은 기호 입력 > ";

                clock_t start_c = clock();
                double paused_sec = 0.0;

                while (true) {
                    double elapsed = (double)(clock() - start_c) / CLOCKS_PER_SEC - paused_sec;
                    if (elapsed >= t_limit) {
                        play_sfx(2);
                        lives--;
                        system("cls");
                        set_color(COLOR_RED);
                        std::cout << "[ LIFE: ";
                        for (int i = 0; i < 3; i++) std::cout << (i < lives ? "♥ " : "♡ ");
                        std::cout << "] ROUND: " << round << "\n\n";
                        render_board_frame(board, m_size, false, correct_ans);
                        set_color(COLOR_RED);
                        std::cout << "\n시간 초과로 하트 1개 차감! (정답: " << correct_ans << ")\n";
                        Sleep(700);
                        break;
                    }

                    render_timer_gauge_at(0, timer_y, t_limit - elapsed, t_limit);

                    if (_kbhit()) {
                        int k = _getch();
                        if (k == KEY_ESC) {
                            clock_t p_start = clock();
                            int p_act = show_pause_menu();
                            paused_sec += (double)(clock() - p_start) / CLOCKS_PER_SEC;
                            if (p_act == 2) { lives = 3; round = 1; break; }
                            if (p_act == 3) return;
                            system("cls");
                            set_color(COLOR_RED);
                            std::cout << "[ LIFE: ";
                            for (int i = 0; i < 3; i++) std::cout << (i < lives ? "♥ " : "♡ ");
                            std::cout << "] ";
                            set_color(COLOR_CYAN);
                            std::cout << "ROUND: " << round << " [" << diff_name << "]\n";
                            render_board_frame(board, m_size, false);
                            gotoxy(0, timer_y + 1);
                            set_color(COLOR_WHITE);
                            std::cout << "가장 많은 기호 입력 > ";
                            continue;
                        }
                        if (k == 0 || k == 224) {
                            int ext = _getch();
                            if (ext == KEY_F1) {
                                clock_t p_start = clock();
                                show_f1_custom_help("Arcade_Survival");
                                paused_sec += (double)(clock() - p_start) / CLOCKS_PER_SEC;
                                system("cls");
                                set_color(COLOR_RED);
                                std::cout << "[ LIFE: ";
                                for (int i = 0; i < 3; i++) std::cout << (i < lives ? "♥ " : "♡ ");
                                std::cout << "] ";
                                set_color(COLOR_CYAN);
                                std::cout << "ROUND: " << round << " [" << diff_name << "]\n";
                                render_board_frame(board, m_size, false);
                                gotoxy(0, timer_y + 1);
                                set_color(COLOR_WHITE);
                                std::cout << "가장 많은 기호 입력 > ";
                            }
                            continue;
                        }

                        if (k >= '0' && k < '0' + s_count) {
                            int ans = k - '0';
                            system("cls");
                            set_color(COLOR_CYAN);
                            std::cout << "ROUND " << round << " 결과 확인\n\n";
                            render_board_frame(board, m_size, false, correct_ans);

                            if (ans == correct_ans) {
                                play_sfx(1);
                                round++;
                                set_color(COLOR_GREEN);
                                std::cout << "\n정답입니다! 다음 라운드로!\n";
                                Sleep(500);
                                break;
                            } else {
                                play_sfx(2);
                                lives--;
                                set_color(COLOR_RED);
                                std::cout << "\n오답으로 하트 1개 차감! (정답: " << correct_ans << ")\n";
                                Sleep(700);
                                break;
                            }
                        }
                    }
                    Sleep(20);
                }
            }
            system("cls");
            set_color(COLOR_RED);
            std::cout << "\nGAME OVER! 모든 생명이 소진되었습니다.\n";
            set_color(COLOR_WHITE);
            std::cout << "난이도: " << diff_name << " | 최종 도달 기록: ROUND " << round << "\n";
            std::cout << "랭킹 등록 이름 > ";
            std::string name;
            std::cin >> name;
            save_ranking(RankEntry("Survival", diff_name, name, (double)round, "Round"));
        } else {
            // [전체 소거 타임어택]
            std::vector<std::vector<int> > board(m_size, std::vector<int>(m_size));
            for (int i = 0; i < m_size; i++) {
                for (int j = 0; j < m_size; j++) {
                    board[i][j] = rand() % s_count;
                }
            }

            system("cls");
            clock_t start_c = clock();
            double penalty = 0.0;
            double paused_sec = 0.0;

            while (true) {
                std::vector<int> counts(s_count, 0);
                int total_remaining = 0;
                for (int i = 0; i < m_size; i++) {
                    for (int j = 0; j < m_size; j++) {
                        if (board[i][j] != -1) {
                            counts[board[i][j]]++;
                            total_remaining++;
                        }
                    }
                }
                if (total_remaining == 0) break;

                int max_c = 0;
                for (int v = 0; v < s_count; v++) {
                    if (counts[v] > max_c) max_c = counts[v];
                }

                double cur_time = (double)(clock() - start_c) / CLOCKS_PER_SEC - paused_sec + penalty;
                gotoxy(0, 0);
                set_color(COLOR_YELLOW);
                std::cout << " [전체 소거 : " << diff_name << "] 경과 시간: " 
                          << std::fixed << std::setprecision(1) << cur_time 
                          << "초 (페널티: +" << penalty << "초)    \n";
                gotoxy(0, 2);
                render_board_frame(board, m_size, false);
                set_color(COLOR_CYAN);
                std::cout << "★ 현재 가장 많이 남은 기호를 입력하세요! (남은 셀: " << total_remaining << ")   \n";
                set_color(COLOR_WHITE);
                std::cout << "입력 >          \n";

                if (_kbhit()) {
                    int ch = _getch();
                    if (ch == KEY_ESC) {
                        clock_t p_start = clock();
                        int p_act = show_pause_menu();
                        paused_sec += (double)(clock() - p_start) / CLOCKS_PER_SEC;
                        if (p_act == 3) return;
                        system("cls");
                        continue;
                    }
                    if (ch == 0 || ch == 224) {
                        int ext = _getch();
                        if (ext == KEY_F1) {
                            clock_t p_start = clock();
                            show_f1_custom_help("Arcade_ClearOut");
                            paused_sec += (double)(clock() - p_start) / CLOCKS_PER_SEC;
                            system("cls");
                        }
                        continue;
                    }

                    if (ch >= '0' && ch < '0' + s_count) {
                        int target = ch - '0';
                        if (counts[target] > 0 && counts[target] == max_c) {
                            play_sfx(1);
                            for (int i = 0; i < m_size; i++) {
                                for (int j = 0; j < m_size; j++) {
                                    if (board[i][j] == target) board[i][j] = -1;
                                }
                            }
                        } else {
                            play_sfx(2);
                            penalty += 10.0;
                            gotoxy(0, m_size * 2 + 6);
                            set_color(COLOR_RED);
                            std::cout << "가장 많은 기호가 아닙니다! (+10초 페널티 부과)    \n";
                            Sleep(500);
                        }
                    }
                }
                Sleep(25);
            }

            double final_time = (double)(clock() - start_c) / CLOCKS_PER_SEC - paused_sec + penalty;
            system("cls");
            set_color(COLOR_GREEN);
            std::cout << "\nALL CLEAR! 모든 기호를 개수 순서대로 완전히 지웠습니다!\n";
            set_color(COLOR_WHITE);
            std::cout << "난이도: " << diff_name << " | 최종 소요 시간: " << final_time << "초\n";
            std::cout << "랭킹 등록 이름 > ";
            std::string name;
            std::cin >> name;
            save_ranking(RankEntry("ClearOut", diff_name, name, final_time, "sec"));
        }
        break;
    }
}

// ==========================================
// [랭킹 : 세분화 랭킹 보드]
// ==========================================
void show_single_ranking_board(const std::string& title, const std::string& cat, const std::string& diff) {
    system("cls");
    std::vector<RankEntry> all_ranks = load_rankings();
    set_color(COLOR_YELLOW);
    std::cout << "========================================================\n";
    std::cout << "   [ " << title << " 랭킹 보드 ]\n";
    std::cout << "========================================================\n";
    set_color(COLOR_WHITE);
    std::cout << " 순위 |   모드   |  난이도  | 플레이어 |     기록     \n";
    std::cout << "--------------------------------------------------------\n";

    int rank_count = 0;
    for (size_t i = 0; i < all_ranks.size(); i++) {
        bool match = false;
        if (cat == "ALL") match = true;
        else if (cat == all_ranks[i].mode_category) {
            if (diff == "ALL" || diff == all_ranks[i].difficulty) match = true;
        }

        if (match) {
            rank_count++;
            std::cout << "  " << std::setw(2) << rank_count << "  | "
                      << std::setw(8) << all_ranks[i].mode_category << " | "
                      << std::setw(8) << all_ranks[i].difficulty << " | "
                      << std::setw(8) << all_ranks[i].player_name << " | "
                      << std::setw(7) << all_ranks[i].record_val << " " << all_ranks[i].unit << "\n";
        }
    }

    if (rank_count == 0) {
        std::cout << "      등록된 랭킹 기록이 아직 없습니다!\n";
    }
    set_color(COLOR_YELLOW);
    std::cout << "========================================================\n";
    set_color(COLOR_GRAY);
    std::cout << "아무 키나 누르면 이전 메뉴로 돌아갑니다...";
    _getch();
}

void show_difficulty_sub_ranking(const std::string& mode_name, const std::string& mode_tag) {
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "     [ " << mode_name << " : 세부 난이도별 랭킹 ]\n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  1. 쉬움   (Easy) 랭킹\n";
        std::cout << "  2. 보통   (Normal) 랭킹\n";
        std::cout << "  3. 어려움 (Hard) 랭킹\n";
        std::cout << "  4. 해당 모드 전체 순위 합쳐서 보기\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 이전 모드 선택으로 복귀\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 > ";

        char ch = _getch();
        if (ch == KEY_ESC) break;
        if (ch == '1') show_single_ranking_board(mode_name + " (Easy)", mode_tag, "Easy");
        else if (ch == '2') show_single_ranking_board(mode_name + " (Normal)", mode_tag, "Normal");
        else if (ch == '3') show_single_ranking_board(mode_name + " (Hard)", mode_tag, "Hard");
        else if (ch == '4') show_single_ranking_board(mode_name + " (ALL)", mode_tag, "ALL");
    }
}

void show_hall_of_fame_menu() {
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "                 [ 랭 킹 (Ranking) ]                    \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  조회할 랭킹 카테고리를 선택하세요:\n\n";
        std::cout << "  1. 클래식 모드 랭킹 (점수 기준 통합 보드)\n";
        std::cout << "  2. 챌린지 모드 랭킹 (깜빡임/셔플/손전등 세부 분리)\n";
        std::cout << "  3. 아케이드 모드 랭킹 (서바이벌/전체삭제 세부 분리)\n";
        std::cout << "  4. 전체 기록 한눈에 보기\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 메인 메뉴로 돌아가기\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 > ";

        char ch = _getch();
        if (ch == KEY_ESC) break;

        if (ch == '1') {
            show_single_ranking_board("클래식 모드", "Classic", "Score");
        } else if (ch == '2') {
            while (true) {
                system("cls");
                set_color(COLOR_YELLOW);
                std::cout << "========================================================\n";
                std::cout << "            [ 챌린지 모드 세부 랭킹 선택 ]              \n";
                std::cout << "========================================================\n";
                set_color(COLOR_WHITE);
                std::cout << "  1. 깜빡임 랭킹\n";
                std::cout << "  2. 위치 셔플 랭킹\n";
                std::cout << "  3. 손전등 랭킹\n";
                set_color(COLOR_GRAY);
                std::cout << "--------------------------------------------------------\n";
                std::cout << "  [ESC] 이전 랭킹 메뉴로 복귀\n";
                set_color(COLOR_YELLOW);
                std::cout << "========================================================\n선택 > ";
                char c_ch = _getch();
                if (c_ch == KEY_ESC) break;
                if (c_ch == '1') show_difficulty_sub_ranking("Blink", "Blink");
                else if (c_ch == '2') show_difficulty_sub_ranking("Shuffle", "Shuffle");
                else if (c_ch == '3') show_difficulty_sub_ranking("Flashlight", "Flashlight");
            }
        } else if (ch == '3') {
            while (true) {
                system("cls");
                set_color(COLOR_YELLOW);
                std::cout << "========================================================\n";
                std::cout << "            [ 아케이드 모드 세부 랭킹 선택 ]            \n";
                std::cout << "========================================================\n";
                set_color(COLOR_WHITE);
                std::cout << "  1. 서바이벌 랭킹\n";
                std::cout << "  2. 전체 삭제 랭킹\n";
                set_color(COLOR_GRAY);
                std::cout << "--------------------------------------------------------\n";
                std::cout << "  [ESC] 이전 랭킹 메뉴로 복귀\n";
                set_color(COLOR_YELLOW);
                std::cout << "========================================================\n선택 > ";
                char a_ch = _getch();
                if (a_ch == KEY_ESC) break;
                if (a_ch == '1') show_difficulty_sub_ranking("Survival", "Survival");
                else if (a_ch == '2') show_difficulty_sub_ranking("ClearOut", "ClearOut");
            }
        } else if (ch == '4') {
            show_single_ranking_board("전체 통합", "ALL", "ALL");
        }
    }
}

// ==========================================
// [환경 설정(Settings) 메뉴 루프]
// ==========================================
void show_settings_menu() {
    while (true) {
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "               [ 환 경 설 정 (Settings) ]               \n";
        std::cout << "========================================================\n";
        set_color(COLOR_WHITE);
        std::cout << "  1. 로비 배경음악 : " << (g_opt_bgm ? "[ ON ]" : "[ OFF ]") << "\n";
        std::cout << "  2. 효과음        : " << (g_opt_sfx ? "[ ON ]" : "[ OFF ]") << "\n";
        std::cout << "--------------------------------------------------------\n";
        set_color(COLOR_RED);
        std::cout << "  3. 랭킹 전체 초기화\n";
        std::cout << "  4. 전적 통계 전체 초기화\n";
        set_color(COLOR_GRAY);
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  [ESC] 메인 메뉴로 복귀\n";
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n선택 > ";

        char ch = _getch();
        if (ch == KEY_ESC) break;

        if (ch == '1') {
            g_opt_bgm = !g_opt_bgm;
            if (g_opt_bgm) start_lobby_bgm();
            else stop_lobby_bgm();
        } else if (ch == '2') {
            g_opt_sfx = !g_opt_sfx;
            play_sfx(3);
        } else if (ch == '3') {
            system("cls");
            set_color(COLOR_RED);
            std::cout << "\n 정말로 등록된 모든 랭킹 기록을 삭제(초기화)하시겠습니까? (Y/N) > ";
            char cf = _getch();
            if (cf == 'y' || cf == 'Y') {
                clear_rankings_file();
                play_sfx(1);
                std::cout << "\n 랭킹 데이터가 성공적으로 초기화되었습니다!\n";
                Sleep(800);
            }
        } else if (ch == '4') {
            system("cls");
            set_color(COLOR_RED);
            std::cout << "\n 정말로 누적된 플레이어 전적 통계를 초기화하시겠습니까? (Y/N) > ";
            char cf = _getch();
            if (cf == 'y' || cf == 'Y') {
                clear_stats_file();
                play_sfx(1);
                std::cout << "\n 전적 통계가 성공적으로 초기화되었습니다!\n";
                Sleep(800);
            }
        }
    }
}

// ==========================================
// [메인 진입점]
// ==========================================
int main() {
    srand((unsigned int)time(NULL));
    init_console_font();
    set_cursor_visibility(false);

    std::vector<std::string> menus;
    menus.push_back("1. 클래식 모드");
    menus.push_back("2. 챌린지 모드");
    menus.push_back("3. 아케이드 모드");
    menus.push_back("4. 랭킹");
    menus.push_back("5. 전적 통계");
    menus.push_back("6. 환경 설정"); 
    menus.push_back("7. 게임 종료");

    int selected = 0;

    while (true) {
        start_lobby_bgm();
        system("cls");
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        std::cout << "          가장 많이 나온 숫자 찾기 게임         \n";
        std::cout << "========================================================\n";
        
        for (int i = 0; i < (int)menus.size(); i++) {
            if (i == selected) {
                set_color(COLOR_GREEN);
                std::cout << "  > " << menus[i] << " <\n";
            } else {
                set_color(COLOR_GRAY);
                std::cout << "    " << menus[i] << "\n";
            }
        }
        set_color(COLOR_YELLOW);
        std::cout << "========================================================\n";
        set_color(COLOR_DARK_GRAY);
        std::cout << " [조작] 방향키(UP/DOWN) 이동 | [Space/Enter] 선택 | [ESC] 종료\n";

        int key = _getch();
        if (key == 0 || key == 224) {
            int ext = _getch();
            if (ext == KEY_UP) {
                play_sfx(3);
                selected = (selected - 1 + (int)menus.size()) % (int)menus.size();
            } else if (ext == KEY_DOWN) {
                play_sfx(3);
                selected = (selected + 1) % (int)menus.size();
            }
        } else if (key == KEY_SPACE || key == KEY_ENTER) {
            play_sfx(3);
            if (selected == 0) run_classic_mode();
            else if (selected == 1) run_challenge_mode();
            else if (selected == 2) run_arcade_mode();
            else if (selected == 3) {
                show_hall_of_fame_menu();
            } else if (selected == 4) {
                system("cls");
                PlayerStats st = load_stats();
                double wr = (st.total_games > 0) ? ((double)st.total_wins / st.total_games * 100.0) : 0.0;
                double avg_s = (st.total_wins > 0) ? (st.total_response_time / st.total_wins) : 0.0;
                set_color(COLOR_YELLOW);
                std::cout << "========================================================\n";
                std::cout << "        [ 플레이어 누적 전적 통계 대시보드 ]           \n";
                std::cout << "========================================================\n";
                set_color(COLOR_WHITE);
                std::cout << "  - 총 플레이 수 : " << st.total_games << " 판\n";
                std::cout << "  - 정답 승률    : " << std::fixed << std::setprecision(1) << wr << " %\n";
                std::cout << "  - 평균 반응속도: " << std::fixed << std::setprecision(2) << avg_s << " 초\n";
                std::cout << "  - 최다 콤보    : " << st.max_combo << " 연속\n";
                std::cout << "  - 힌트 사용    : " << st.hints_used << " 회\n";
                set_color(COLOR_YELLOW);
                std::cout << "========================================================\n";
                set_color(COLOR_GRAY);
                std::cout << "아무 키나 누르면 복귀합니다...";
                _getch();
            } else if (selected == 5) {
                show_settings_menu(); 
            } else if (selected == 6) {
                break;
            }
        } else if (key == KEY_ESC) {
            break;
        }
    }

    stop_lobby_bgm();
    system("cls");
    set_color(COLOR_WHITE);
    std::cout << "게임을 종료합니다. 플레이해주셔서 감사합니다!\n";
    return 0;
}
