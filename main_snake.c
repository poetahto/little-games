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

typedef enum { NONE, UP, DOWN, LEFT, RIGHT } Direction;
typedef enum { PLAYING, PAUSED } Snake_GameState;

typedef struct {
    int x, y;
} Vector2i;

typedef struct {
    Direction data[3];
    int read;
    int write;
} Snake_InputBuffer;

static u16 gSnakeRandomSeed;

bool Snake_IsBlocked(const int *world, int x, int y)
{
    return world[x * WORLD_HEIGHT + y] > 0;
}

void Snake_PushInput(Snake_InputBuffer *inputBuffer, Direction direction)
{
    int nextIndex = (inputBuffer->write + 1) % ARR_LEN(inputBuffer->data);

    if (nextIndex != inputBuffer->read)
    {
        inputBuffer->data[inputBuffer->write] = direction;
        inputBuffer->write = nextIndex;
    }
}

Direction Snake_PopInput(Snake_InputBuffer *inputBuffer)
{
    if (inputBuffer->read == inputBuffer->write) return NONE;
    Direction input = inputBuffer->data[inputBuffer->read];
    inputBuffer->read = (inputBuffer->read + 1) % ARR_LEN(inputBuffer->data);
    return input;
}

Vector2i Snake_RandomFoodPosition(const int *world)
{
    Vector2i availablePositions[WORLD_WIDTH * WORLD_HEIGHT];
    int availablePositionCount = 0;

    for (int x = 0; x < WORLD_WIDTH; x++)
        for (int y = 0; y < WORLD_HEIGHT; y++)
            if (!Snake_IsBlocked(world, x, y))
                availablePositions[availablePositionCount++] = (Vector2i) { .x = x, .y = y };

    int randomIndex = LfsrFibonacci(&gSnakeRandomSeed) % availablePositionCount;
    return availablePositions[randomIndex];
}

void Snake_DrawGridSquare(int x, int y, Draw_Color color)
{
    Draw_Rectangle((x * SIZE) + (0.5f * SIZE), (y * SIZE) + (0.5f * SIZE), SIZE, SIZE, color);
}

void Entrypoint()
{
    Os_Random(&gSnakeRandomSeed, sizeof(gSnakeRandomSeed));
    Os_CreateWindow(800, 600);
    Draw_Startup();

    Arena arena = Os_CreateArena(MB(16));
    bool isRunning = true;
    ArenaReset(&arena);

    Direction currentDirection = UP;
    int world[WORLD_WIDTH][WORLD_HEIGHT] = { 0 };
    int snakeLength = 3;
    int snakeHead[2] = { 4, 2 };
    world[snakeHead[0]][snakeHead[1]] = 1;
    Vector2i foodPosition = Snake_RandomFoodPosition((const int *)world);
    Snake_GameState gameState = PLAYING;
    Snake_InputBuffer inputBuffer = {0};

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
                        case OS_KEY_UP: Snake_PushInput(&inputBuffer, UP); break;
                        case OS_KEY_DOWN: Snake_PushInput(&inputBuffer, DOWN); break;
                        case OS_KEY_LEFT: Snake_PushInput(&inputBuffer, LEFT); break;
                        case OS_KEY_RIGHT: Snake_PushInput(&inputBuffer, RIGHT); break;
                        default: break;
                    }
                }
                default: break;
            }
        }

        if (gameState == PLAYING)
        {
            world[snakeHead[0]][snakeHead[1]] = snakeLength;

            Direction inputDirection = Snake_PopInput(&inputBuffer);
            if (inputDirection == UP && currentDirection != DOWN) currentDirection = UP;
            if (inputDirection == DOWN && currentDirection != UP) currentDirection = DOWN;
            if (inputDirection == LEFT && currentDirection != RIGHT) currentDirection = LEFT;
            if (inputDirection == RIGHT && currentDirection != LEFT) currentDirection = RIGHT;

            switch (currentDirection)
            {
                case UP: snakeHead[1]++; break;
                case DOWN: snakeHead[1]--; break;
                case LEFT: snakeHead[0]--; break;
                case RIGHT: snakeHead[0]++; break;
                default: assert(false); break;
            }

            if (snakeHead[0] > WORLD_WIDTH - 1) snakeHead[0] = 0;
            if (snakeHead[0] < 0) snakeHead[0] = WORLD_WIDTH - 1;
            if (snakeHead[1] > WORLD_HEIGHT - 1) snakeHead[1] = 0;
            if (snakeHead[1] < 0) snakeHead[1] = WORLD_HEIGHT - 1;

            if (snakeHead[0] == foodPosition.x && snakeHead[1] == foodPosition.y)
            {
                foodPosition = Snake_RandomFoodPosition((const int *) world);
                snakeLength++;

                for (int x = 0; x < WORLD_WIDTH; x++)
                    for (int y = 0; y < WORLD_HEIGHT; y++)
                        if (world[x][y] > 0)
                            world[x][y]++;
            }

            if (Snake_IsBlocked((const int *)world, snakeHead[0], snakeHead[1]))
            {
                MemoryClear(world, sizeof(world));
                snakeHead[0] = 4;
                snakeHead[1] = 2;
                foodPosition = Snake_RandomFoodPosition((const int *)world);
                snakeLength = 3;
                /*gameState = PAUSED;*/
            }

            for (int x = 0; x < WORLD_WIDTH; x++)
                for (int y = 0; y < WORLD_HEIGHT; y++)
                    if (world[x][y] > 0) world[x][y]--;
        }

        Draw_BeginFrame();

        for (int x = 0; x < WORLD_WIDTH; x++)
            for (int y = 0; y < WORLD_HEIGHT; y++)
                if (world[x][y] > 0)
                    Snake_DrawGridSquare(x, y, DRAW_WHITE);

        Snake_DrawGridSquare(snakeHead[0], snakeHead[1], DRAW_WHITE);
        Snake_DrawGridSquare(foodPosition.x, foodPosition.y, DRAW_GREEN);

        Draw_EndFrame();

        if (isRunning) 
            Os_Sleep(150);
   }

    Draw_Shutdown();
    Os_FreeWindow();
}
