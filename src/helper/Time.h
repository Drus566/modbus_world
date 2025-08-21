#ifndef MB_TIME_H
#define MB_TIME_H

#include <sys/time.h>

namespace mb {
namespace helpers {

void millisecondsToSecondsMicroseconds(int milliseconds, int& seconds, int& microseconds);

void millisecondsToTimeval(int milliseconds, timeval &tv);

void sleep(int ms);

} // helpers
} // mb

#endif // MB_TIME_H