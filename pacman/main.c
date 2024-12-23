#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#define DEBUG

#define ARRAY_SIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
#define BACKGROUD_COLOR BLACK

#define SWIDTH  800
#define SHEIGHT 800
#define WWIDTH  28
#define WHEIGHT 31

#define BLOCK_WIDTH  (SWIDTH / WWIDTH)
#define BLOCK_HEIGHT (SHEIGHT / WHEIGHT)

#define OUT_OF_BOUNDS(v) ( (v).x < 0 || (v).x >= WWIDTH ) || ( (v).y < 0 || (v).y >= WHEIGHT )

#define WORLD_OFFESET ((Vector2) { .x = 100, .y = 10 })
#define VECTOR2(xx, yy) ((Vector2) { .x = (xx), .y = (yy) })

#define TIME_TO_NEXT_BLK 0.220

#define CAPACITY 64
#define DA_APPEND(da, v) { \
    if ((da).items == NULL) { \
        (da).items = malloc(sizeof((v)) * CAPACITY); \
        (da).len = 0; \
        (da).capacity = CAPACITY; \
    } \
    if ((da).capacity <= (da).len) { \
        (da).items = realloc((da).items, sizeof((v)) * 2*(da).capacity); \
        (da).capacity = 2*(da).capacity; \
    } \
    (da).items[(da).len++] = v; \
} while (0);

typedef enum {
    WALL_1 = '#',
    NO_BLOCK = ' ',
    POINT = 'p' // not used directly in the map construction, but inserted in runtime
} Block;

Block world[WHEIGHT][WWIDTH] = {
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

Vector2 validPositions[] = {
    VECTOR2( 1,  1), VECTOR2( 2,  1), VECTOR2( 3,  1), VECTOR2( 4,  1), VECTOR2( 5,  1), VECTOR2( 6,  1), VECTOR2( 7,  1), VECTOR2( 8,  1), VECTOR2( 9,  1), VECTOR2(10,  1),
    VECTOR2(11,  1), VECTOR2(12,  1), VECTOR2(15,  1), VECTOR2(16,  1), VECTOR2(17,  1), VECTOR2(18,  1), VECTOR2(19,  1), VECTOR2(20,  1), VECTOR2(21,  1), VECTOR2(22,  1),
    VECTOR2(23,  1), VECTOR2(24,  1), VECTOR2(25,  1), VECTOR2(26,  1), VECTOR2( 1,  2), VECTOR2( 6,  2), VECTOR2(12,  2), VECTOR2(15,  2), VECTOR2(21,  2), VECTOR2(26,  2),
    VECTOR2( 1,  3), VECTOR2( 6,  3), VECTOR2(12,  3), VECTOR2(15,  3), VECTOR2(21,  3), VECTOR2(26,  3), VECTOR2( 1,  4), VECTOR2( 6,  4), VECTOR2(12,  4), VECTOR2(15,  4),
    VECTOR2(21,  4), VECTOR2(26,  4), VECTOR2( 1,  5), VECTOR2( 2,  5), VECTOR2( 3,  5), VECTOR2( 4,  5), VECTOR2( 5,  5), VECTOR2( 6,  5), VECTOR2( 7,  5), VECTOR2( 8,  5),
    VECTOR2( 9,  5), VECTOR2(10,  5), VECTOR2(11,  5), VECTOR2(12,  5), VECTOR2(13,  5), VECTOR2(14,  5), VECTOR2(15,  5), VECTOR2(16,  5), VECTOR2(17,  5), VECTOR2(18,  5),
    VECTOR2(19,  5), VECTOR2(20,  5), VECTOR2(21,  5), VECTOR2(22,  5), VECTOR2(23,  5), VECTOR2(24,  5), VECTOR2(25,  5), VECTOR2(26,  5), VECTOR2( 1,  6), VECTOR2( 6,  6),
    VECTOR2( 9,  6), VECTOR2(18,  6), VECTOR2(21,  6), VECTOR2(26,  6), VECTOR2( 1,  7), VECTOR2( 6,  7), VECTOR2( 9,  7), VECTOR2(18,  7), VECTOR2(21,  7), VECTOR2(26,  7),
    VECTOR2( 1,  8), VECTOR2( 2,  8), VECTOR2( 3,  8), VECTOR2( 4,  8), VECTOR2( 5,  8), VECTOR2( 6,  8), VECTOR2( 9,  8), VECTOR2(10,  8), VECTOR2(11,  8), VECTOR2(12,  8),
    VECTOR2(15,  8), VECTOR2(16,  8), VECTOR2(17,  8), VECTOR2(18,  8), VECTOR2(21,  8), VECTOR2(22,  8), VECTOR2(23,  8), VECTOR2(24,  8), VECTOR2(25,  8), VECTOR2(26,  8),
    VECTOR2( 6,  9), VECTOR2(12,  9), VECTOR2(15,  9), VECTOR2(21,  9), VECTOR2( 6, 10), VECTOR2(12, 10), VECTOR2(15, 10), VECTOR2(21, 10), VECTOR2( 6, 11), VECTOR2(12, 11),
    VECTOR2(15, 11), VECTOR2(21, 11), VECTOR2( 6, 12), VECTOR2( 9, 12), VECTOR2(10, 12), VECTOR2(11, 12), VECTOR2(12, 12), VECTOR2(13, 12), VECTOR2(14, 12), VECTOR2(15, 12),
    VECTOR2(16, 12), VECTOR2(17, 12), VECTOR2(18, 12), VECTOR2(21, 12), VECTOR2( 6, 13), VECTOR2( 9, 13), VECTOR2(13, 13), VECTOR2(14, 13), VECTOR2(18, 13), VECTOR2(21, 13),
    VECTOR2( 6, 14), VECTOR2( 7, 14), VECTOR2( 8, 14), VECTOR2( 9, 14), VECTOR2(18, 14), VECTOR2(19, 14), VECTOR2(20, 14), VECTOR2(21, 14), VECTOR2(22, 14), VECTOR2(23, 14),
    VECTOR2(24, 14), VECTOR2(25, 14), VECTOR2(26, 14), VECTOR2(27, 14), VECTOR2( 6, 15), VECTOR2( 9, 15), VECTOR2(18, 15), VECTOR2(21, 15), VECTOR2( 6, 16), VECTOR2( 9, 16),
    VECTOR2(18, 16), VECTOR2(21, 16), VECTOR2( 6, 17), VECTOR2( 9, 17), VECTOR2(10, 17), VECTOR2(11, 17), VECTOR2(12, 17), VECTOR2(13, 17), VECTOR2(14, 17), VECTOR2(15, 17),
    VECTOR2(16, 17), VECTOR2(17, 17), VECTOR2(18, 17), VECTOR2(21, 17), VECTOR2( 6, 18), VECTOR2( 9, 18), VECTOR2(18, 18), VECTOR2(21, 18), VECTOR2( 6, 19), VECTOR2( 9, 19),
    VECTOR2(18, 19), VECTOR2(21, 19), VECTOR2( 1, 20), VECTOR2( 2, 20), VECTOR2( 3, 20), VECTOR2( 4, 20), VECTOR2( 5, 20), VECTOR2( 6, 20), VECTOR2( 7, 20), VECTOR2( 8, 20),
    VECTOR2( 9, 20), VECTOR2(10, 20), VECTOR2(11, 20), VECTOR2(12, 20), VECTOR2(15, 20), VECTOR2(16, 20), VECTOR2(17, 20), VECTOR2(18, 20), VECTOR2(19, 20), VECTOR2(20, 20),
    VECTOR2(21, 20), VECTOR2(22, 20), VECTOR2(23, 20), VECTOR2(24, 20), VECTOR2(25, 20), VECTOR2(26, 20), VECTOR2( 1, 21), VECTOR2( 6, 21), VECTOR2(12, 21), VECTOR2(15, 21),
    VECTOR2(21, 21), VECTOR2(26, 21), VECTOR2( 1, 22), VECTOR2( 6, 22), VECTOR2(12, 22), VECTOR2(15, 22), VECTOR2(21, 22), VECTOR2(26, 22), VECTOR2( 1, 23), VECTOR2( 2, 23),
    VECTOR2( 3, 23), VECTOR2( 6, 23), VECTOR2( 7, 23), VECTOR2( 8, 23), VECTOR2( 9, 23), VECTOR2(10, 23), VECTOR2(11, 23), VECTOR2(12, 23), VECTOR2(13, 23), VECTOR2(14, 23),
    VECTOR2(15, 23), VECTOR2(16, 23), VECTOR2(17, 23), VECTOR2(18, 23), VECTOR2(19, 23), VECTOR2(20, 23), VECTOR2(21, 23), VECTOR2(24, 23), VECTOR2(25, 23), VECTOR2(26, 23),
    VECTOR2( 3, 24), VECTOR2( 6, 24), VECTOR2( 9, 24), VECTOR2(18, 24), VECTOR2(21, 24), VECTOR2(24, 24), VECTOR2( 3, 25), VECTOR2( 6, 25), VECTOR2( 9, 25), VECTOR2(18, 25),
    VECTOR2(21, 25), VECTOR2(24, 25), VECTOR2( 1, 26), VECTOR2( 2, 26), VECTOR2( 3, 26), VECTOR2( 4, 26), VECTOR2( 5, 26), VECTOR2( 6, 26), VECTOR2( 9, 26), VECTOR2(10, 26),
    VECTOR2(11, 26), VECTOR2(12, 26), VECTOR2(15, 26), VECTOR2(16, 26), VECTOR2(17, 26), VECTOR2(18, 26), VECTOR2(21, 26), VECTOR2(22, 26), VECTOR2(23, 26), VECTOR2(24, 26),
    VECTOR2(25, 26), VECTOR2(26, 26), VECTOR2( 1, 27), VECTOR2(12, 27), VECTOR2(15, 27), VECTOR2(26, 27), VECTOR2( 1, 28), VECTOR2(12, 28), VECTOR2(15, 28), VECTOR2(26, 28),
    VECTOR2( 1, 29), VECTOR2( 2, 29), VECTOR2( 3, 29), VECTOR2( 4, 29), VECTOR2( 5, 29), VECTOR2( 6, 29), VECTOR2( 7, 29), VECTOR2( 8, 29), VECTOR2( 9, 29), VECTOR2(10, 29),
    VECTOR2(11, 29), VECTOR2(12, 29), VECTOR2(13, 29), VECTOR2(14, 29), VECTOR2(15, 29), VECTOR2(16, 29), VECTOR2(17, 29), VECTOR2(18, 29), VECTOR2(19, 29), VECTOR2(20, 29),
    VECTOR2(21, 29), VECTOR2(22, 29), VECTOR2(23, 29), VECTOR2(24, 29), VECTOR2(25, 29), VECTOR2(26, 29),
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
    int state;
    int points;
    int lifes;
};

static bool pacman_moving = false;
static struct Pacman pacman = {
    .entity = {
        .direction = RIGHT,
        .size = VECTOR2(BLOCK_HEIGHT, BLOCK_HEIGHT),
        .pos = VECTOR2(1, 29),
        .last_pos = VECTOR2(1, 29),
        .screen_pos = { 0 }
    },
    .points = 0,
    .state = 1,
    .nextDirection = RIGHT,
    .lifes = 3
};

struct Ghost {
    Entity entity;
    Vector2 target;
    struct {
        Direction *items;
        size_t len;
        size_t capacity;
        size_t curr;
    } path;
};

#define TOTAL_GHOSTS 4
static struct Ghost ghosts[TOTAL_GHOSTS] = {
    {
        .entity = {
            .direction = RIGHT,
            .size = VECTOR2(BLOCK_HEIGHT, BLOCK_HEIGHT),
            .pos = VECTOR2(16, 14),
            .last_pos = VECTOR2(16, 14),
            .screen_pos = { 0 },
        },
        .target = VECTOR2(16, 14),
        .path = { 0 }
    },
    {
        .entity = {
            .direction = LEFT,
            .size = VECTOR2(BLOCK_HEIGHT, BLOCK_HEIGHT),
            .pos = VECTOR2(15, 14),
            .last_pos = VECTOR2(15, 14),
            .screen_pos = { 0 }
        },
        .target = VECTOR2(15, 14),
        .path = { 0 }
    },
    {
        .entity = {
            .direction = LEFT,
            .size = VECTOR2(BLOCK_HEIGHT, BLOCK_HEIGHT),
            .pos = VECTOR2(14, 14),
            .last_pos = VECTOR2(14, 14),
            .screen_pos = { 0 }
        },
        .target = VECTOR2(14, 14),
        .path = { 0 }
    },
    {
        .entity = {
            .direction = LEFT,
            .size = VECTOR2(BLOCK_HEIGHT, BLOCK_HEIGHT),
            .pos = VECTOR2(14, 14),
            .last_pos = VECTOR2(14, 14),
            .screen_pos = { 0 }
        },
        .target = VECTOR2(14, 14),
        .path = { 0 }
    },
};

static struct {
    Texture2D textures;
} game = { 0 };

bool follow_pacman = false;
void handle_input() {
    int k = GetKeyPressed();
    switch (k) {
        case KEY_UP:    pacman.nextDirection = UP;     break;
        case KEY_DOWN:  pacman.nextDirection = DOWN;   break;
        case KEY_LEFT:  pacman.nextDirection = LEFT;   break;
        case KEY_RIGHT: pacman.nextDirection = RIGHT;  break;
        case KEY_X:     follow_pacman = !follow_pacman; break;
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

Vector2 calcNewPosition(Vector2 old_position, Direction d) {
    return Vector2Add(old_position, directionVector(d));
}

void movePacman(void) {
    // TraceLog(LOG_INFO, "%d", pacman.state);
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

    if (!moved) newpos = calcNewPosition(pacman.entity.pos, pacman.entity.direction);

    if (!isWall(newpos)) {
        pacman.entity.last_pos = pacman.entity.pos;
        pacman.entity.pos = newpos;
        pacman.entity.screen_pos = world2screen(pacman.entity.last_pos);
        pacman_moving = true;
        pacman.state = !pacman.state;
    }
}

void tracePath(struct Ghost *ghost) {
    if (Vector2Equals(ghost->target, ghost->entity.pos)) return;
    static Direction all_directions[] = { UP, DOWN, LEFT, RIGHT };

    struct { Direction direction; int px, py; }
                squares[WHEIGHT][WWIDTH] = { 0 };
    static Vector2 queue[WHEIGHT*WWIDTH] = { 0 };

    bool visited[WHEIGHT][WWIDTH] = { 0 };

    size_t qlen = 0;
    size_t qhead = 0;
    size_t l = ARRAY_SIZE(all_directions);
    size_t limit = ARRAY_SIZE(queue);
    queue[qlen++] = ghost->entity.pos;
    visited[(int) ghost->entity.pos.y][(int) ghost->entity.pos.x] = true;
    while (qlen <= limit && qhead < qlen) {
        Vector2 current_position = queue[qhead++];

        for (size_t d = 0; d < l; d++) {
            Direction current_direction = all_directions[d];
            Vector2 pos = calcNewPosition(current_position, current_direction);
            int iy = pos.y;
            int ix = pos.x;

            if (OUT_OF_BOUNDS(pos) || visited[iy][ix] || isWall(pos)) continue;

            squares[iy][ix].py = (int) current_position.y;
            squares[iy][ix].px = (int) current_position.x;
            squares[iy][ix].direction = current_direction;
            visited[iy][ix] = true;
            queue[qlen++] = pos;

            if (Vector2Equals(ghost->target, pos)) goto found;
        }
    }

    assert(0 && "how da fuck we didnt find the path?");
found:
    ghost->path.len = 0;
    int ix = ghost->target.x;
    int iy = ghost->target.y;

    do {
        assert(visited[iy][ix]);
        DA_APPEND(ghost->path, squares[iy][ix].direction);
        int temp = squares[iy][ix].px;
        iy = squares[iy][ix].py;
        ix = temp;
    } while (!Vector2Equals(VECTOR2(ix, iy), ghost->entity.pos));

    ghost->path.curr = ghost->path.len;
}

Direction ghostDirection(struct Ghost *ghost) {
    return ghost->path.items[--ghost->path.curr];
}

void moveGhosts(void) {
    for (size_t i = 0; i < TOTAL_GHOSTS; i++) {
        if (follow_pacman) {
            ghosts[i].target = pacman.entity.pos;
            tracePath(&ghosts[i]);
        } else {
            if (Vector2Equals(ghosts[i].entity.pos, ghosts[i].target)) {
                do {
                    ghosts[i].target = validPositions[GetRandomValue(0, ARRAY_SIZE(validPositions) - 1)];
                } while (Vector2Equals(ghosts[i].entity.pos, ghosts[i].target));
                tracePath(&ghosts[i]);
            }
        }

        ghosts[i].entity.last_pos = ghosts[i].entity.pos;
        ghosts[i].entity.direction = ghostDirection(&ghosts[i]);
        ghosts[i].entity.pos = calcNewPosition(ghosts[i].entity.pos, ghosts[i].entity.direction);
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

void updatePoints(void) {
    int ix = pacman.entity.pos.x;
    int iy = pacman.entity.pos.y;
    Block b = world[iy][ix];
    switch (b) {
        case POINT: pacman.points++; break;
        default: return;
    }

    world[iy][ix] = ' ';
}

void update(void) {
    float dt = GetFrameTime();
    if (isTimeToMove(dt)) {
        updatePoints();
        movePacman();
        moveGhosts();
    } else {
        if (pacman_moving) {
            pacman.entity.screen_pos = animate(pacman.entity);
        }

        animateGhosts();
    }

    // if (checkCollissions()) {
    //     pacman.lifes--;
    //     quit = pacman.lifes == 0;
    // }
}

static const int point_size = 4;
static const int point_offsetx = BLOCK_WIDTH/2 - point_size/2;
static const int point_offsety = BLOCK_HEIGHT/2 - point_size/2;
void drawWorld() {
    ClearBackground(BACKGROUD_COLOR);
    for (size_t y = 0; y < WHEIGHT; y++) {
        for (size_t x = 0; x < WWIDTH; x++) {
            Vector2 spos = world2screen(VECTOR2(x, y));
            switch (world[y][x]) {
                case '#': DrawRectangleLinesEx((Rectangle){ .height = BLOCK_HEIGHT, .width = BLOCK_WIDTH, .x = spos.x, .y = spos.y }, 1, WHITE); break;
                case 'p': {
                    DrawRectangleRec((Rectangle){
                        .height = point_size,
                        .width = point_size,
                        .x = spos.x + point_offsetx,
                        .y = spos.y + point_offsety
                    }, YELLOW);
                } break;
                default: { }
            }
        }
    }
}

typedef enum {
    UPPER_LEFT = 0,
    CENTER,
    MIDDLE_BOTTOM,
} Dest_Offset;

void drawEntity(Texture2D texture, Entity entity, int offsetx, int offsety, Dest_Offset origin, float rot) {
    Rectangle source = {
        .height = 32,
        .width = 32,
        .x = offsetx*32,
        .y = offsety*32
    };

    Rectangle dest = {
        .height = entity.size.y,
        .width = entity.size.x,
        .x = entity.screen_pos.x,
        .y = entity.screen_pos.y
    };

    Vector2 vorigin;
    switch (origin) {
        case MIDDLE_BOTTOM: assert(0 && "TODO: not implemented");

        case CENTER: {
            vorigin.x = dest.width/2;
            vorigin.y = dest.height/2;
            dest.x += vorigin.x;
            dest.y += vorigin.y;
        } break;

        default: vorigin = Vector2Zero();
    }

    DrawTexturePro(texture, source, dest, vorigin, rot, WHITE);
}

void drawPacman() {
    static float rots[] = {
        [RIGHT] = 000.0f,
        [DOWN]  = 090.0f,
        [LEFT]  = 180.0f,
        [UP]    = 270.0f
    };

    drawEntity(
        game.textures,
        pacman.entity,
        pacman.state,
        5,
        CENTER,
        rots[pacman.entity.direction]
    );
}

void drawGhosts() {
    for (size_t i = 0; i < TOTAL_GHOSTS; i++) {
        drawEntity(game.textures, ghosts[i].entity, ghosts[i].entity.direction, i, UPPER_LEFT, 0.0f);
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


    Image image = LoadImage("./assets/pacman.png");
    game.textures = LoadTextureFromImage(image);

    const size_t lvalid_positions = ARRAY_SIZE(validPositions);
    for (size_t i = 0; i < lvalid_positions; i++) {
        int ix = validPositions[i].x;
        int iy = validPositions[i].y;
        if (world[iy][ix] == ' ') {
            world[iy][ix] = 'p';
        }
    }

    while (!WindowShouldClose() && !quit) {
        handle_input();
        update();
        draw();
    }

    CloseWindow();
    return 0;
}

