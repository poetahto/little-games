#ifndef GAMES_OS_H
#define GAMES_OS_H

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void OS_InitWindow(int width, int height);
void OS_FreeWindow();
void OS_Sleep(int milliseconds);
int OS_Timestamp();
void OS_Log(const char *message);

#endif // GAMES_OS_H
