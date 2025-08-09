#ifndef GAMES_PROFILE_H
#define GAMES_PROFILE_H

#define PROFILE(id) Profile_Push(id); for (bool done_ = false; !done_; Profile_Pop(), done_ = true)

void Profile_Push(const char *id);
void Profile_Pop(void);

#endif // GAMES_PROFILE_H
