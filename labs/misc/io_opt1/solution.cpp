#include "solution.hpp"

#include "MappedFile.hpp"

uint32_t solution(const char *file_name) {
  MappedFile mapped_file(file_name);
  
  // Initial value has all bits set to 1
  uint32_t crc = 0xff'ff'ff'ff;

  // Update the CRC32 value character by character
  const char *contents = mapped_file.getContents().data();
  char c;
  while ((c = *contents++) != 0) {
    update_crc32(crc, static_cast<uint8_t>(c));
  }

  // Invert the bits
  crc ^= 0xff'ff'ff'ff;

  return crc;
}
