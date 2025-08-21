#include "Time.h"

#include <thread>
#include <chrono>
#include <unistd.h>

namespace mb {
namespace helpers {

void millisecondsToSecondsMicroseconds(int milliseconds, int& seconds, int& microseconds) {
   int total_microseconds = milliseconds * 1000; 
   seconds = total_microseconds / 1000000; 
   microseconds = total_microseconds % 1000000;
}

void millisecondsToTimeval(int milliseconds, timeval& tv) {
   tv.tv_sec = milliseconds / 1000;           // Целое количество секунд
   tv.tv_usec = (milliseconds % 1000) * 1000; // Остаток в микросекундах
}

void sleep(int ms) {
   usleep(ms * 1000);
}

} // helpers
} // mb