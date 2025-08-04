#ifndef LITTLE_GAMES_OS_H
#define LITTLE_GAMES_OS_H

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void OS_Log(const char *message);

#endif // LITTLE_GAMES_OS_H
