
#include "solution.h"
#include <memory>
#include <smmintrin.h>


void imageSmoothing(const InputVector &input, uint8_t radius,
                    OutputVector &output) {
  int pos = 0;
  int currentSum = 0;
  int size = static_cast<int>(input.size());

  // 1. left border - time spend in this loop can be ignored, no need to
  // optimize it
  for (int i = 0; i < std::min<int>(size, radius); ++i) {
    currentSum += input[i];
  }

  int limit = std::min(radius + 1, size - radius);
  for (pos = 0; pos < limit; ++pos) {
    currentSum += input[pos + radius];
    output[pos] = currentSum;
  }

  // 2. main loop.
  
  limit = size - radius;
  for (; pos < limit - 8; pos += 8) {
    __m128i simd_current_sum = _mm_set1_epi16(currentSum);

    __m128i sub_u8 = _mm_loadu_si64(input.data() + (pos - radius - 1));
    __m128i sub = _mm_cvtepu8_epi16(sub_u8);

    __m128i add_u8 = _mm_loadu_si64(input.data() + pos + radius);
    __m128i add = _mm_cvtepu8_epi16(add_u8);

    __m128i diff = _mm_sub_epi16(add, sub);

    // Compute the prefix sum in SIMD-way (by adding the shifted values).
    diff = _mm_add_epi16(diff, _mm_slli_si128(diff, 2)); // shift the entire 128-bit register by 2 bytes (1 uint16 value)
    diff = _mm_add_epi16(diff, _mm_slli_si128(diff, 4)); // shift the entire 128-bit register by 4 bytes (2 uint16 value)
    diff = _mm_add_epi16(diff, _mm_slli_si128(diff, 8)); // shift the entire 128-bit register by 8 bytes (4 uint16 value)

    // Store result into output
    __m128i final_sum = _mm_add_epi16(simd_current_sum, diff);
    _mm_storeu_si128((__m128i *)(output.data() + pos), final_sum);

    // Set currentSum to the last computed value
    int last_sum = _mm_extract_epi16(final_sum, 7);
    currentSum = last_sum;
  }
  
  
  // 2.1 main loop - remaining cases of previous loop.
  for (; pos < limit; ++pos) {
    currentSum -= input[pos - radius - 1];
    currentSum += input[pos + radius];
    output[pos] = currentSum;
  }

  // 3. special case, executed only if size <= 2*radius + 1
  limit = std::min(radius + 1, size);
  for (; pos < limit; pos++) {
    output[pos] = currentSum;
  }

  // 4. right border - time spend in this loop can be ignored, no need to
  // optimize it
  for (; pos < size; ++pos) {
    currentSum -= input[pos - radius - 1];
    output[pos] = currentSum;
  }
}
