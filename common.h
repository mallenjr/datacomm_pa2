#include <algorithm>
#include <cmath>

using std::min;
using std::abs;

struct SequenceCounter {
  unsigned int value;
  unsigned int cycle;

  SequenceCounter() {
    value = 0;
    cycle = 0;
  }

  SequenceCounter operator++(int) {
    value = (value + 1) % 8;
    if (value == 0)
      ++cycle;
  }

  SequenceCounter& operator=(int a) {
    if (a > 7) {
      ++cycle;
    }

    value = (a) % 8;
    return *this;
  }

  int distance(SequenceCounter &b) {
    if (cycle > b.cycle) {
      return (8 - b.value) + value;
    } else {
      return value - b.value;
    }
  }
};
