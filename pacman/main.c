#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

#define DEBUG

#define BACKGROUD_COLOR BLACK
#define SWIDTH  800
#define SHEIGHT 800

#define WORLD_OFFESET ((Vector2) { .x = 100, .y = 10 })

#define VECTOR2(xx, yy) ((Vector2) { .x = (xx), .y = (yy) })

#define WWIDTH  28
#define WHEIGHT 31
#define BLOCK_WIDTH  (SWIDTH / WWIDTH)
#define BLOCK_HEIGHT (SHEIGHT / WHEIGHT)

#define TIME_TO_NEXT_BLK 0.150

typedef enum {
    WALL_1 = '#',
    NO_BLOCK = ' '
} Blocks;

Blocks world[WHEIGHT][WWIDTH] = {
    { '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#' },
    { '#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ',' ',' ',' ',' ',' ','#','#',' ',' ',' ',' ','#','#',' ',' ',' ',' ','#','#',' ',' ',' ',' ',' ',' ','#' },
    { '#','#','#','#','#','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#','#','#','#','#','#' },
    { ' ',' ',' ',' ',' ','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#',' ',' ',' ',' ',' ' },
    { ' ',' ',' ',' ',' ','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#',' ',' ',' ',' ',' ' },
    { ' ',' ',' ',' ',' ','#',' ','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#',' ','#',' ',' ',' ',' ',' ' },
    { '#','#','#','#','#','#',' ','#','#',' ','#','#','#',' ',' ','#','#','#',' ','#','#',' ','#','#','#','#','#','#' },
    { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' },
    { '#','#','#','#','#','#',' ','#','#',' ','#',' ',' ',' ',' ',' ',' ','#',' ','#','#',' ','#','#','#','#','#','#' },
    { ' ',' ',' ',' ',' ','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#',' ',' ',' ',' ',' ' },
    { ' ',' ',' ',' ',' ','#',' ','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#',' ','#',' ',' ',' ',' ',' ' },
    { ' ',' ',' ',' ',' ','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#',' ',' ',' ',' ',' ' },
    { '#','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#','#' },
    { '#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ','#','#','#','#',' ','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#',' ','#','#','#','#',' ','#' },
    { '#',' ',' ',' ','#','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#','#',' ',' ',' ','#' },
    { '#','#','#',' ','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#',' ','#','#','#' },
    { '#','#','#',' ','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#',' ','#','#','#' },
    { '#',' ',' ',' ',' ',' ',' ','#','#',' ',' ',' ',' ','#','#',' ',' ',' ',' ','#','#',' ',' ',' ',' ',' ',' ','#' },
    { '#',' ','#','#','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#','#','#',' ','#' },
    { '#',' ','#','#','#','#','#','#','#','#','#','#',' ','#','#',' ','#','#','#','#','#','#','#','#','#','#',' ','#' },
    { '#',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','#' },
    { '#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#' },
};

static bool quit = false;

Vector2 world2screen(Vector2 w) {
    return Vector2Add(Vector2Multiply(w, VECTOR2(BLOCK_WIDTH, BLOCK_HEIGHT)), WORLD_OFFESET);
}

Vector2 screen2world(Vector2 s) {
    s = Vector2Subtract(s, WORLD_OFFESET);
    if (s.x != 0) s.x = (int) s.x / BLOCK_WIDTH;
    if (s.y != 0) s.y = (int) s.y / BLOCK_HEIGHT;
    return s;
}

typedef enum {
    UP = 0, DOWN, LEFT, RIGHT
} Direction;

typedef struct {
    Vector2 pos;
    Vector2 last_pos;
    Vector2 screen_pos;
    Vector2 size;
    Direction direction;
} Entity;

struct Pacman {
    Entity entity;
    Direction nextDirection;
};

struct Ghost {
    Entity entity;
};

static bool pacman_moving = false;
static struct Pacman pacman = {
    .entity = {
        .direction = RIGHT,
        .size = VECTOR2(BLOCK_WIDTH / 2, BLOCK_WIDTH / 2),
        .pos = VECTOR2(1, 29),
        .last_pos = VECTOR2(1, 29),
        .screen_pos = { 0 }
    },
    .nextDirection = RIGHT,
};

static struct Ghost ghosts[] = {
    {
        .entity = {
            .direction = RIGHT,
            .size = VECTOR2(BLOCK_WIDTH / 2, BLOCK_WIDTH / 2),
            .pos = VECTOR2(1, 14),
            .last_pos = VECTOR2(16, 14),
            .screen_pos = { 0 }
        }
    },
    {
        .entity = {
            .direction = LEFT,
            .size = VECTOR2(BLOCK_WIDTH / 2, BLOCK_WIDTH / 2),
            .pos = VECTOR2(1, 14),
            .last_pos = VECTOR2(16, 14),
            .screen_pos = { 0 }
        }
    },
};

static_assert(sizeof(ghosts) > 0);
const size_t TOTAL_GHOSTS = sizeof(ghosts)/sizeof(ghosts[0]);

void handle_input() {
    int k = GetKeyPressed();
    switch (k) {
        case KEY_UP:    pacman.nextDirection = UP;    break;
        case KEY_DOWN:  pacman.nextDirection = DOWN;  break;
        case KEY_LEFT:  pacman.nextDirection = LEFT;  break;
        case KEY_RIGHT: pacman.nextDirection = RIGHT; break;
    }
}

Vector2 directionVector(Direction d) {
    switch (d) {
        case UP:    return VECTOR2( 0, -1);
        case DOWN:  return VECTOR2( 0, +1);
        case LEFT:  return VECTOR2(-1,  0);
        case RIGHT: return VECTOR2(+1,  0);
    }

    assert(0 && "UNREACHBEALE");
}

bool isWall(Vector2 pos) {
    return world[(int) pos.y][(int) pos.x] == '#';
}

static float timeacc = TIME_TO_NEXT_BLK;
bool isTimeToMove(float dt) {
    timeacc += dt;
    bool itsTime = timeacc >= TIME_TO_NEXT_BLK;
    if (itsTime) {
        timeacc = 0;
    }

    return itsTime;
}

void movePacman(void) {
    pacman_moving = false;

    Vector2 newpos;
    bool moved = false;
    if (pacman.entity.direction != pacman.nextDirection) {
        newpos = Vector2Add(pacman.entity.pos, directionVector(pacman.nextDirection));
        if (!isWall(newpos)) {
            pacman.entity.direction = pacman.nextDirection;
            moved = true;
        }
    }

    if (!moved) newpos = Vector2Add(pacman.entity.pos, directionVector(pacman.entity.direction));

    if (!isWall(newpos)) {
        pacman.entity.last_pos = pacman.entity.pos;
        pacman.entity.pos = newpos;
        pacman.entity.screen_pos = world2screen(pacman.entity.last_pos);
        pacman_moving = true;
    }
}

void moveGhosts(void) {
    for (size_t i = 0; i < TOTAL_GHOSTS; i++) {
        ghosts[i].entity.last_pos = ghosts[i].entity.pos;
        ghosts[i].entity.screen_pos = world2screen(ghosts[i].entity.pos);
        Vector2 new_pos = Vector2Add(ghosts[i].entity.pos, directionVector(ghosts[i].entity.direction));

        for (Direction d = GetRandomValue(0, 3); isWall(new_pos); d++, d%=4) {
            ghosts[i].entity.direction = d;
            new_pos = Vector2Add(ghosts[i].entity.pos, directionVector(ghosts[i].entity.direction));
        }

        ghosts[i].entity.pos = new_pos;
    }
}

Vector2 animate(Entity entity) {
    float nt = timeacc / TIME_TO_NEXT_BLK;
    return Vector2Add(world2screen(entity.last_pos), Vector2Scale(Vector2Multiply(directionVector(entity.direction), VECTOR2(BLOCK_WIDTH, BLOCK_HEIGHT)), nt));
}

void animateGhosts(void) {
    for (size_t i = 0; i < TOTAL_GHOSTS; i++) ghosts[i].entity.screen_pos = animate(ghosts[i].entity);
}

Rectangle getRekt(Entity entity) {
    return (Rectangle) {
        .height = entity.size.y,
        .width = entity.size.x,
        .x = entity.screen_pos.x,
        .y = entity.screen_pos.y,
    };
}

bool checkCollissions(void) {
    Rectangle pacman_rect = getRekt(pacman.entity);
    for (size_t i = 0; i < TOTAL_GHOSTS; i++) {
        if (CheckCollisionRecs(pacman_rect, getRekt(ghosts[i].entity))) {
            return true;
        }
    }

    return false;
}

void update(void) {
    float dt = GetFrameTime();
    if (isTimeToMove(dt)) {
        movePacman();
        moveGhosts();
    } else {
        if (pacman_moving) {
            pacman.entity.screen_pos = animate(pacman.entity);
        }

        animateGhosts();
    }

    if (checkCollissions()) quit = true;
}

void drawWorld() {
    ClearBackground(BACKGROUD_COLOR);
    for (size_t y = 0; y < WHEIGHT; y++) {
        for (size_t x = 0; x < WWIDTH; x++) {
            Vector2 spos = world2screen(VECTOR2(x, y));
            switch (world[y][x]) {
                case '#': DrawRectangleLinesEx((Rectangle){ .height = BLOCK_HEIGHT, .width = BLOCK_WIDTH, .x = spos.x, .y = spos.y }, 1, WHITE);
                default: { }
            }
        }
    }
}

void drawPacman() {
    Vector2 center = Vector2AddValue(pacman.entity.screen_pos, pacman.entity.size.x);
    DrawCircleV(center, pacman.entity.size.x, YELLOW);
}

void drawGhosts() {
    for (size_t i = 0; i < TOTAL_GHOSTS; i++) {
        Vector2 center = Vector2AddValue(ghosts[i].entity.screen_pos, ghosts[i].entity.size.x);
        DrawCircleV(center, ghosts[i].entity.size.x, BLUE);
    }
}

void drawDebugInfo(void) {
    Vector2 mouse = GetMousePosition();
    static char s[512] = { 0 };

    sprintf(s, "M(%.2f, %.2f)", mouse.x, mouse.y);
    DrawText(s, 10, 10, 10, WHITE);

    mouse = screen2world(mouse);
    sprintf(s, "S(%.2f, %.2f)", mouse.x, mouse.y);
    DrawText(s, 10, 30, 10, WHITE);

    mouse = world2screen(mouse);
    sprintf(s, "W(%.2f, %.2f)", mouse.x, mouse.y);
    DrawText(s, 10, 50, 10, WHITE);

    sprintf(s, "B(%d, %d)", BLOCK_WIDTH, BLOCK_HEIGHT);
    DrawText(s, 10, 70, 10, WHITE);

    sprintf(s, "P(%.0f, %.0f)", pacman.entity.pos.x, pacman.entity.pos.y);
    DrawText(s, 10, 90, 10, WHITE);


}

void draw() {
    BeginDrawing();
    drawWorld();
    drawPacman();
    drawGhosts();
#ifdef DEBUG
    drawDebugInfo();
#endif
    EndDrawing();
}

int main(void) {
    InitWindow(SWIDTH + WORLD_OFFESET.x*2, SHEIGHT + WORLD_OFFESET.y*2, "Pacman");
    pacman.entity.screen_pos = world2screen(pacman.entity.pos);

    for (size_t i = 0; i < TOTAL_GHOSTS; i++) {
        ghosts[i].entity.screen_pos = world2screen(ghosts[i].entity.pos);
    }

    SetRandomSeed(time(NULL));

    while (!WindowShouldClose() && !quit) {
        handle_input();
        update();
        draw();
    }

    CloseWindow();
    return 0;
}

