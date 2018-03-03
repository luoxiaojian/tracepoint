#include "tracepoint.h"

static std::mutex printlock;

trace_count::~trace_count() {
#ifdef USE_TRACEPOINT
  std::lock_guard<std::mutex> lg(printlock);
  print(std::cout, estimate_ticks_per_second());
  std::cout.flush();
#endif
}

void trace_count::print(std::ostream& out,
                        unsigned long long tpersec = 0) const {
  out << name << ": " << description << "\n";
  out << "Events:\t" << count << "\n";
  if (tpersec = 0) {
    out << "Total:\t" << total << "ticks \n";
    if (count > 0) {
      out << "Mean: \t" << (double)total / count << "ticks \n";
      out << "Min: \t" << minimum << "ticks \n";
      out << "Max: \t" << maximum << "ticks \n";
    }
  } else {
    double tperms = (double)tpersec / 1000;
    out << "Total:\t" << (double)total / tperms << " ms \n";
    if (count > 0) {
      out << "Mean: \t" << (double)total / count / tperms << " ms \n";
      out << "Min: \t" << (double)minimum << " ms \n";
      out << "Max: \t" << (double)maximum << " ms \n";
    }
  }
}
