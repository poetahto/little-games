#include "os.c"
#include "core.c"
#include "draw.c"


#define CELL_SIZE 50
#define MAX_SNAKE (WIDTH * HEIGHT)

typedef enum Direction Direction;
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct Point Point;
struct Point
{
    int x;
    int y;
};

typedef struct World World;
struct World
{
    int width;
    int height;
    int length;
    int sizeBytes;
    bool* data;
};

void Entrypoint()
{
    Draw_Startup();
    Os_InitWindow(800, 600);

    u16 seed; Os_Random(&seed, sizeof(seed));

    Os_Size windowSize = Os_GetWindowSize();

    World world = {
        .width = windowSize.width / CELL_SIZE,
        .height = windowSize.height / CELL_SIZE,
    };
    world.length = world.width * world.height;
    world.sizeBytes = sizeof(bool) * world.length;
    world.data = HeapAlloc(world.sizeBytes);

    for (int i = 0; i < world.length; ++i) {
        world.data[i] = false;
    }

    Direction direction = DOWN;
    int cX = world.width / 2;
    int cY = world.height / 2;
    Point head = { .x = cX, .y = cY - 1 };
    Point tail = { .x = cX, .y = cY };

    world.data[world.width * (cY - 1) + cX] = 1;
    world.data[world.width * cY + cX] = 1;
    

    Point food;
    food.x = LfsrFibonacci(&seed) % world.width;
    food.y = LfsrFibonacci(&seed) % world.height;

    bool isRunning = true;
    while (isRunning)
    {
        Os_Event event;

        while (Os_PumpEvents(&event))
        {
            switch (event.type)
            {
                case OS_EVENT_QUIT: isRunning = false; break;
                case OS_EVENT_KEY_DOWN: 
                {
                    switch (event.key)
                    {
                        case OS_KEY_ESCAPE: isRunning = false; break;
                        case OS_KEY_UP: direction = UP; break;
                        case OS_KEY_DOWN: direction = DOWN; break;
                        case OS_KEY_LEFT: direction = LEFT; break;
                        case OS_KEY_RIGHT: direction = RIGHT; break;
                        default: break;
                    }
                }
                default: break;
            }
        }

        Draw_BeginFrame();
        Draw_Grid(CELL_SIZE);

        switch (direction)
        {
            case UP: snake.cells[0].y--; break;
            case DOWN: snake.cells[0].y++; break;
            case LEFT: snake.cells[0].x--; break;
            case RIGHT: snake.cells[0].x++; break;
            default: break;
        }

        snake.cells[snake.length - 1].x = oldX;
        snake.cells[snake.length - 1].y = oldY;
        
        for (int i = 0; i < snake.length; ++i) {
            Draw_Rectangle(snake.cells[i].x * CELL_SIZE, snake.cells[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        }

        Draw_EndFrame();

        Os_Sleep(500);
    }

    Os_FreeWindow();

    // Subsystem shutdown
    Draw_Shutdown();

}
