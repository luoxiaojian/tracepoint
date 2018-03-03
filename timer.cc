#include "timer.h"

static unsigned long long rtdsc_ticks_per_sec = 0;
static std::mutex rtdsc_ticks_per_sec_mutex;

unsigned long long estimate_ticks_per_second() {
  if (rtdsc_ticks_per_sec == 0) {
    std::lock_guard<std::mutex> lg(rtdsc_ticks_per_sec_mutex);
    if (rtdsc_ticks_per_sec == 0) {
      unsigned long long tstart = rdtsc();
      mysleep(1);
      unsigned long long tend = rdtsc();
      rtdsc_ticks_per_sec = tend - tstart;
    }
  }
  return rtdsc_ticks_per_sec;
}

void mysleep(size_t sleeplen) {
  struct timespec timeout;
  timeout.tv_sec = sleeplen;
  timeout.tv_nsec = 0;
  while (nanosleep(&timeout, &timeout) == -1)
    ;
}
