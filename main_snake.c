#include "os.c"
#include "core.c"
#include <stdlib.h>

#define WIDTH 30
#define HEIGHT 30
#define MAX_SNAKE WIDTH *HEIGHT

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

void Entrypoint(void)
{
    enum Direction direction = DOWN;

    uint16_t seed = 0xB00B;

    // TODO(sam): Don't spawn on edge
    SnakeCell* head = (SnakeCell*) malloc(sizeof(SnakeCell));
    head->x = lfsr_fibonacci(&seed) % WIDTH;
    head->y = lfsr_fibonacci(&seed) % HEIGHT;
    
    SnakeCell* tail = head;
}