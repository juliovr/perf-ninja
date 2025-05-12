#include "solution.hpp"

uint16_t checksum(const Blob &blob) {
  uint32_t sum = 0;
  for (auto value : blob) {
    sum += value;
  }

  while (sum >> 16) {
    // Every value in the upper 16 bits is the sum of the carries accumulated.
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  return sum;
}
