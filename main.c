/*
 * Advanced Snake Game
 * C11 console game for Windows (MinGW/GCC).
 * Controls: W/A/S/D move, P pause, Q quit.
 * AI mode uses BFS to find a safe path to food, with safe-move fallback.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define W 30
#define H 18
#define MAX_CELLS (W * H)
#define MAX_SNAKE (MAX_CELLS - 1)

typedef struct { int x, y; } Point;
typedef enum { UP, RIGHT, DOWN, LEFT } Dir;

static Point snake[MAX_SNAKE];
static int length, score, best, game_over, paused, ai_mode;
static Point food;
static Dir direction;
static int delay_ms = 130;

static const int dx[4] = {0, 1, 0, -1};
static const int dy[4] = {-1, 0, 1, 0};

static int same(Point a, Point b) { return a.x == b.x && a.y == b.y; }

static int inside(Point p) {
    return p.x >= 0 && p.x < W && p.y >= 0 && p.y < H;
}

static int occupied(Point p, int ignore_tail) {
    int limit = length - (ignore_tail ? 1 : 0);
    for (int i = 0; i < limit; ++i)
        if (same(snake[i], p)) return 1;
    return 0;
}

static void load_best(void) {
    FILE *f = fopen("highscore.txt", "r");
    if (f) { if (fscanf(f, "%d", &best) != 1) best = 0; fclose(f); }
    else best = 0;
}

static void save_best(void) {
    if (score > best) {
        best = score;
        FILE *f = fopen("highscore.txt", "w");
        if (f) { fprintf(f, "%d\n", best); fclose(f); }
    }
}

static void spawn_food(void) {
    Point p;
    do {
        p.x = rand() % W;
        p.y = rand() % H;
    } while (occupied(p, 0));
    food = p;
}

static void setup(int difficulty, int bot) {
    length = 3; score = 0; game_over = 0; paused = 0;
    ai_mode = bot; direction = RIGHT;
    delay_ms = difficulty == 1 ? 190 : difficulty == 2 ? 130 : 80;
    snake[0] = (Point){W / 2, H / 2};
    snake[1] = (Point){W / 2 - 1, H / 2};
    snake[2] = (Point){W / 2 - 2, H / 2};
    spawn_food();
}

static void draw(void) {
    system("cls");
    puts("========== ADVANCED SNAKE ==========");
    printf("Mode: %s | Score: %d | Best: %d\n",
           ai_mode ? "AI BOT" : "PLAYER", score, best);
    puts("Controls: W A S D | P pause | Q quit");
    putchar('+');
    for (int x = 0; x < W; ++x) putchar('-');
    puts("+");
    for (int y = 0; y < H; ++y) {
        putchar('|');
        for (int x = 0; x < W; ++x) {
            Point p = {x, y};
            char c = ' ';
            if (same(p, snake[0])) c = '@';
            else if (same(p, food)) c = '*';
            else {
                for (int i = 1; i < length; ++i)
                    if (same(p, snake[i])) { c = 'o'; break; }
            }
            putchar(c);
        }
        puts("|");
    }
    putchar('+');
    for (int x = 0; x < W; ++x) putchar('-');
    puts("+");
    if (paused) puts("PAUSED - press P to resume");
}

static int reverse_dir(Dir a, Dir b) { return ((int)a + 2) % 4 == (int)b; }

static int valid_step(Point p, int allow_tail) {
    if (!inside(p)) return 0;
    return !occupied(p, allow_tail);
}

/* BFS over board cells; returns first direction toward food, or -1. */
static int ai_direction(void) {
    int prev[MAX_CELLS], first[MAX_CELLS];
    int queue[MAX_CELLS], head = 0, tail = 0;
    for (int i = 0; i < MAX_CELLS; ++i) { prev[i] = -2; first[i] = -1; }
    int start = snake[0].y * W + snake[0].x;
    int goal = food.y * W + food.x;
    queue[tail++] = start; prev[start] = -1;

    while (head < tail) {
        int cur = queue[head++];
        if (cur == goal) return first[cur];
        Point p = {cur % W, cur / W};
        for (int d = 0; d < 4; ++d) {
            Point n = {p.x + dx[d], p.y + dy[d]};
            if (!inside(n)) continue;
            int id = n.y * W + n.x;
            if (prev[id] != -2) continue;
            /* Treat current tail as free because it moves on a normal step. */
            if (occupied(n, 1) && id != goal) continue;
            prev[id] = cur;
            first[id] = cur == start ? d : first[cur];
            queue[tail++] = id;
        }
    }

    /* No food path: choose any safe non-reversing move. */
    for (int k = 0; k < 4; ++k) {
        int d = (direction + k + 1) % 4;
        if (reverse_dir(direction, (Dir)d)) continue;
        Point n = {snake[0].x + dx[d], snake[0].y + dy[d]};
        if (valid_step(n, 1)) return d;
    }
    return (int)direction;
}

static void input(void) {
    if (!_kbhit()) return;
    int c = _getch();
    if (c == 'q' || c == 'Q') { game_over = 1; return; }
    if (c == 'p' || c == 'P') { paused = !paused; return; }
    if (ai_mode || paused) return;
    Dir next = direction;
    if (c == 'w' || c == 'W') next = UP;
    if (c == 'd' || c == 'D') next = RIGHT;
    if (c == 's' || c == 'S') next = DOWN;
    if (c == 'a' || c == 'A') next = LEFT;
    if (!reverse_dir(direction, next)) direction = next;
}

static void update(void) {
    if (ai_mode) direction = (Dir)ai_direction();
    Point next = {snake[0].x + dx[direction], snake[0].y + dy[direction]};
    int eating = same(next, food);
    if (!inside(next) || occupied(next, eating ? 0 : 1)) {
        game_over = 1; return;
    }
    if (eating && length < MAX_SNAKE) ++length;
    for (int i = length - 1; i > 0; --i) snake[i] = snake[i - 1];
    snake[0] = next;
    if (eating) { score += 10; spawn_food(); }
}

static int choose_difficulty(void) {
    int choice = 0;
    while (choice < 1 || choice > 3) {
        system("cls");
        puts("Choose difficulty:\n1. Easy\n2. Medium\n3. Hard");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            choice = 0;
        }
    }
    return choice;
}

static void play(int bot, int difficulty) {
    setup(difficulty, bot);
    while (!game_over) {
        draw();
        input();
        if (game_over) break;
        if (!paused) update();
        Sleep(delay_ms);
    }
    save_best();
    draw();
    printf("\nGAME OVER! Final score: %d\n", score);
    printf("Best score: %d\n", best);
    puts("Press any key to return to menu...");
    _getch();
}

int main(void) {
    srand((unsigned)time(NULL));
    load_best();
    for (;;) {
        int choice = 0;
        system("cls");
        puts("==================================");
        puts("       ADVANCED SNAKE GAME");
        puts("==================================");
        puts("1. Play (Player)");
        puts("2. Watch AI Bot");
        puts("3. Instructions");
        puts("4. Exit");
        printf("\nHigh score: %d\nSelect: ", best);
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            continue;
        }
        if (choice == 1) play(0, choose_difficulty());
        else if (choice == 2) play(1, choose_difficulty());
        else if (choice == 3) {
            system("cls");
            puts("W/A/S/D: Move   P: Pause   Q: Quit");
            puts("Eat * food to grow and earn 10 points.");
            puts("Avoid walls and your own body.");
            puts("AI uses breadth-first search to seek food.");
            puts("\nPress any key...");
            _getch();
        } else if (choice == 4) {
            save_best();
            puts("Thanks for playing!");
            break;
        }
    }
    return 0;
}
