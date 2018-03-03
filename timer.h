#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>

#include <mutex>

void mysleep(size_t sleeplen);
unsigned long long estimate_ticks_per_second();

#if defined(__i386__)
static inline unsigned long long rdtsc(void) {
  unsigned long long int x;
  __asm__ volatile(".byte 0x0f, 0x31" : "=A"(x));
  return x;
}
#elif defined(__x86_64__)
static inline unsigned long long rdtsc(void) {
  unsigned hi, lo;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}
#else
static inline unsigned long long rdtsc(void) { return 0; }
#endif

#endif
