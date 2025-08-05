#ifndef GAMES_OS_H
#define GAMES_OS_H

#include "core.h"

// NOTE(poe): Cross-platform main function - should always be defined.
void Entrypoint(void);

void Os_Sleep(int milliseconds);
void Os_Random(void *buffer, int bufferLength);
void Os_Log(const char *message, ...);

#endif // GAMES_OS_H
