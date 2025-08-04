#include "os.c"
#include "core.c"

#include <stdlib.h>

#define WIDTH 30
#define HEIGHT 30
#define MAX_SNAKE (WIDTH * HEIGHT)

typedef enum Direction Direction;
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct SnakeCell SnakeCell;
struct SnakeCell
{
    int x;
    int y;
    SnakeCell* next;
};

void Entrypoint()
{
    Direction direction = DOWN;

    u16 seed = 0xB00B;

    // TODO(sam): Don't spawn on edge
    SnakeCell* head = (SnakeCell*) malloc(sizeof(SnakeCell));
    head->x = LfsrFibonacci(&seed) % WIDTH;
    head->y = LfsrFibonacci(&seed) % HEIGHT;

    SnakeCell* tail = head;
}
