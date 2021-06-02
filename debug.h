#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

void halt();

void die(const char *errorMessage, const char *file, int line);

#define VERIFY(condition) ((condition) ? 1 : (die("Assert failure - '" #condition "'", __FILE__, __LINE__), 0))
#define VERIFYMSG(condition, message) ((condition) ? 1 : (die(message " (" #condition ")", __FILE__, __LINE__), 0))
#define FAIL(message) die((message), __FILE__, __LINE__)

#endif
