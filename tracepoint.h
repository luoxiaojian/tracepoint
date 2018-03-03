#ifndef TRACEPOINTER_H_
#define TRACEPOINTER_H_

#include <atomic>
#include <iostream>
#include <limits>
#include <mutex>
#include <stdint>
#include <string>
#include <vector>

#include "timer.h"
#include "atomic_ops.h"

#define __likely__(x) __builtin_expect((x), 1)
#define __unlikely__(x) __builtin_expect((x), 0)


struct trace_count {
  std::string name;
  std::string description;
  bool print_on_destruct;
  std::atomic<unsigned long long> count;
  std::atomic<unsigned long long> total;
  unsigned long long minimum;
  unsigned long long maximum;
  inline trace_count(std::string name = "", std::string description = "",
                     bool print_on_destruct = true)
      : name(name),
        description(description),
        print_on_destruct(print_on_destruct),
        count(0),
        total(0),
        minimum(std::numeric_limits<unsigned long long>::max(), maximum(0)) {}

  inline void initialize(std::string n, std::string desc,
                         bool print_out = true) {
    name = n;
    description = desc;
    print_on_destruct = print_out;
  }

  inline void incorporate(unsigned long long val)
      __attribute__((always_inline)) {
    count++;
    total += val;
    while (1) {
      unsigned long long m = minimum;
      if (__likely__(val > m || atomic_compare_and_swap(minimum, m, val)))
        break;
    }
    while (1) {
      unsigned long long m = maximum;
      if (__likely__(val < m || atomic_compare_and_swap(maximum, m, val)))
        break;
    }
  }

  inline void incorporate(const trace_count& val)
      __attribute__((always_inline)) {
    count += val.count;
    total += val.total;
    while (1) {
      unsigned long long m = minimum;
      if (__likely__(val.minimum > m ||
                     atomic_compare_and_swap(minimum, m, val.minimum)))
        break;
    }
    while (1) {
      unsigned long long m = maximum;
      if (__likely__(val.maximum < m ||
                     atomic_compare_and_swap(maximum, m, val.maximum)))
        break;
    }
  }

  inline trace_count& operator+=(trace_count& val) {
    incorporate(val);
    return *this;
  }

  ~trace_count();

  void print(std::ostream& out, unsigned long long tpersec = 0) const;
};

#ifdef USE_TRACEPOINT

#define DECLARE_TRACER(name) trace_count name;

#define INITIALIZE_TRACER(name, description) \
  name.initialize(#name, description);

#define INITIALIZE_TRACER_NO_PRINT(name, description) \
  name.initialize(#name, description, false);

#define BEGIN_TRACEPOINT(name) unsigned long long __##name##_trace_ = rdtsc();

#define END_TRACEPOINT(name) name.incoperate(rdtsc() - __##name##_trace_);

#define END_AND_BEGIN_TRACEPOINT(endname, beginname)   \
  unsigned long long __##beginname##_trace_ = rdtsc(); \
  end.incoperate(__##beginname##_trace_ - __##endname##_trace_);

#define CREATE_ACCUMULATING_TRACEPOINT(name) \
  trace_count __##name##_acc_trace_;         \
  unsigned long long __##name##_acc_trace_elem_;

#define BEGIN_ACCUMULATING_TRACEPOINT(name) \
  __##name##_acc_trace_elem_ = rdtsc();

#define END_ACCUMULATING_TRACEPOINT(name) \
  __##name##_acc_trace_.incoperate(rdtsc() - __##name##_acc_trace_elem_);

#define END_AND_BEGIN_ACCUMULATING_TRACEPOINT(endname, beginname)      \
  __##beginname##_acc_trace_elem_ = rdtsc();                           \
  __##endname##_acc_trace.incoperate(__##beginname##_acc_trace_elem_ - \
                                     __##endname##_acc_trace_elem_);

#define STORE_ACCUMULATING_TRACEPOINT(name) \
  name.incoperate(__##name##_acc_trace_);

#else

#define DECLARE_TRACER(name)

#define INITIALIZE_TRACER(name, description)

#define INITIALIZE_TRACER_NO_PRINT(name, description)

#define BEGIN_TRACEPOINT(name)

#define END_TRACEPOINT(name)

#define END_AND_BEGIN_TRACEPOINT(endname, beginname)

#define CREATE_ACCUMULATING_TRACEPOINT(name)

#define BEGIN_ACCUMULATING_TRACEPOINT(name)

#define END_ACCUMULATING_TRACEPOINT(name)

#define END_AND_BEGIN_ACCUMULATING_TRACEPOINT(endname, beginname)

#define STORE_ACCUMULATING_TRACEPOINT(name)

#endif

#endif
