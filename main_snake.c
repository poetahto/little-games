#include "core.c"
#include "os.c"
#include "draw.c"
#include "gpu.c"
#include "profile.c"

#define SIZE 50
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WORLD_WIDTH (SCREEN_WIDTH / SIZE)
#define WORLD_HEIGHT (SCREEN_HEIGHT / SIZE)

INCBIN(PlayerImage, "resources/smile.bmp");

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

static u16 gSnakeRandomSeed;

void GetRandomFoodPosition(int position[2])
{
    position[0] = (LfsrFibonacci(&gSnakeRandomSeed) % (WORLD_WIDTH - 1));
    position[1] = (LfsrFibonacci(&gSnakeRandomSeed) % (WORLD_HEIGHT - 1));
}

void Entrypoint()
{
    Os_Random(&gSnakeRandomSeed, sizeof(gSnakeRandomSeed));
    Os_CreateWindow(800, 600);
    Gpu_Startup();
    Draw_Startup();

    Arena arena = Os_CreateArena(MB(16));
    bool isRunning = true;
    ArenaReset(&arena);

    Direction currentDirection = UP;
    int world[WORLD_WIDTH][WORLD_HEIGHT] = { 0 };
    int snakeLength = 3;
    int snakeHead[2] = { 4, 2 };
    int foodPosition[2] = { 11, 10 };
    GetRandomFoodPosition(foodPosition);

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
                        case OS_KEY_UP: currentDirection = UP; break;
                        case OS_KEY_DOWN: currentDirection = DOWN; break;
                        case OS_KEY_LEFT: currentDirection = LEFT; break;
                        case OS_KEY_RIGHT: currentDirection = RIGHT; break;
                        default: break;
                    }
                }
                default: break;
            }
        }

        world[snakeHead[0]][snakeHead[1]] = snakeLength;

        switch (currentDirection)
        {
            case UP: snakeHead[1]++; break;
            case DOWN: snakeHead[1]--; break;
            case LEFT: snakeHead[0]--; break;
            case RIGHT: snakeHead[0]++; break;
            default: assert(false); break;
        }

        if (snakeHead[0] == foodPosition[0] && snakeHead[1] == foodPosition[1])
        {
            GetRandomFoodPosition(foodPosition);
            snakeLength++;

            for (int x = 0; x < WORLD_WIDTH; x++)
                for (int y = 0; y < WORLD_HEIGHT; y++)
                    if (world[x][y] > 0)
                        world[x][y]++;
        }

        if (world[snakeHead[0]][snakeHead[1]] > 0 || snakeHead[0] > WORLD_WIDTH - 1 || snakeHead[1] > WORLD_HEIGHT - 1 || snakeHead[0] < 0 || snakeHead[1] < 0)
        {
            MemoryClear(world, sizeof(world));
            snakeHead[0] = 4;
            snakeHead[1] = 2;
            GetRandomFoodPosition(foodPosition);
            snakeLength = 3;
        }

        for (int x = 0; x < WORLD_WIDTH; x++)
            for (int y = 0; y < WORLD_HEIGHT; y++)
                if (world[x][y] > 0) world[x][y]--;

        Draw_BeginFrame();
        Draw_Rectangle((snakeHead[0] * SIZE) + (0.5f * SIZE), (snakeHead[1] * SIZE) + (0.5f * SIZE), SIZE, SIZE, DRAW_WHITE);
        Draw_Rectangle((foodPosition[0] * SIZE) + (0.5f * SIZE), (foodPosition[1] * SIZE) + (0.5f * SIZE), SIZE, SIZE, DRAW_GREEN);

        for (int x = 0; x < WORLD_WIDTH; x++)
            for (int y = 0; y < WORLD_HEIGHT; y++)
                if (world[x][y] > 0)
                    Draw_Rectangle((x * SIZE) + (0.5f * SIZE), (y * SIZE) + (0.5f * SIZE), SIZE, SIZE, DRAW_WHITE);

        Draw_EndFrame();

        if (isRunning) 
            Os_Sleep(150);
   }

    Draw_Shutdown();
    Gpu_Shutdown();
    Os_FreeWindow();
}
