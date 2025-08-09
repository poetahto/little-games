#include "os.c"
#include "core.c"
#include "draw.c"
#include "profile.c"
#include "gpu.c"

#define CELL_SIZE 50
#define MAX_SNAKE (WIDTH * HEIGHT)

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int width;
    int height;
    int length;
    int sizeBytes;
    bool* data;
} World;

static void DrawPoint(Point point, int size, Draw_Color color);
static int GetPointIndex(World world, Point point);
static void SetPoint(World world, Point point);
static void ClearPoint(World world, Point point);
static bool CheckPoint(World world, Point point);
static Point GetRandomPoint(World world, u16 *seed);
static Point GetPointRelative(Point point, Direction direction);

void Entrypoint()
{
    Os_CreateWindow(800, 600);
    Gpu_Startup();
    Draw_Startup();

    u16 seed; Os_Random(&seed, sizeof(seed));
    int width, height; Os_GetWindowSize(&width, &height);

    World world;
    world.width = width / CELL_SIZE;
    world.height = height / CELL_SIZE;
    world.length = world.width * world.height;
    world.sizeBytes = sizeof(bool) * world.length;
    world.data = Os_HeapAlloc(world.sizeBytes);
    MemoryClear(world.data, world.sizeBytes);

    int cX = world.width / 2;
    int cY = world.height / 2;
    Point head = { .x = cX, .y = cY };
    SetPoint(world, head);

    Point food = GetRandomPoint(world, &seed);
    Direction direction = DOWN;
    bool isRunning = true;

    while (isRunning)
    {
        Os_WindowEvent event;

        while (Os_PumpWindowEvents(&event))
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

        // TODO(poe): Needs to support updating the whole list, this was the
        // best I could figure w/ the data structure right now
        Point next = GetPointRelative(head, direction);
        SetPoint(world, next);
        ClearPoint(world, head);
        head = next;

        Draw_BeginFrame();
        Draw_Grid(CELL_SIZE);
        DrawPoint(food, CELL_SIZE, DRAW_YELLOW);
        
        for (int x = 0; x < world.width; x++)
        {
            for (int y = 0; y < world.height; y++)
            {
                Point point = { .x = x, .y = y };

                if (CheckPoint(world, point))
                    DrawPoint(point, CELL_SIZE, DRAW_WHITE);
            }
        }
        
        Draw_EndFrame();
        Os_Sleep(500);
    }

    Draw_Shutdown();
    Gpu_Shutdown();
    Os_FreeWindow();
}

static void DrawPoint(Point point, int size, Draw_Color color)
{
    int width, height; Os_GetWindowSize(&width, &height);
    Draw_Rectangle(point.x * size, height - (point.y * size), size, size, color);
}

static int GetPointIndex(World world, Point point)
{
    // NOTE(poe): Put the vibe code to the test here
    return world.width * point.y + point.x;
}

static void SetPoint(World world, Point point)
{
    world.data[GetPointIndex(world, point)] = true;
}

static void ClearPoint(World world, Point point)
{
    world.data[GetPointIndex(world, point)] = false;
}

static bool CheckPoint(World world, Point point)
{
    return world.data[GetPointIndex(world, point)];
}

static Point GetRandomPoint(World world, u16 *seed)
{
    return (Point) 
    { 
        .x = LfsrFibonacci(seed) % world.width, 
        .y = LfsrFibonacci(seed) % world.height, 
    };
}

static Point GetPointRelative(Point point, Direction direction)
{
    switch (direction)
    {
        case UP: return (Point) { .x = point.x, .y = point.y + 1 };
        case DOWN: return (Point) { .x = point.x, .y = point.y - 1 };
        case LEFT: return (Point) { .x = point.x - 1, .y = point.y };
        case RIGHT: return (Point) { .x = point.x + 1, .y = point.y };
        default: assert(false && "Invalid direction");
    }
}
