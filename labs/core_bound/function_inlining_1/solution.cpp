
#include "solution.h"
#include <algorithm>
#include <stdlib.h>

// static int compare(const void *lhs, const void *rhs) {
//   auto &a = *reinterpret_cast<const S *>(lhs);
//   auto &b = *reinterpret_cast<const S *>(rhs);

//   if (a.key1 < b.key1)
//     return -1;

//   if (a.key1 > b.key1)
//     return 1;

//   if (a.key2 < b.key2)
//     return -1;

//   if (a.key2 > b.key2)
//     return 1;

//   return 0;
// }

static int compare(S a, S b) {
  if (a.key1 < b.key1)
    return -1;

  if (a.key1 > b.key1)
    return 1;

  if (a.key2 < b.key2)
    return -1;

  if (a.key2 > b.key2)
    return 1;

  return 0;
}

static void swap(std::array<S, N> &arr, int i, int j) {
  S temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

static int partition(std::array<S, N> &arr, int lo, int hi) {
  int i = lo, j = hi + 1;
  S &v = arr[lo];
  while (true) {
    while (compare(arr[++i], v) < 0) if (i == hi) break;
    while (compare(v, arr[--j]) < 0) if (j == lo) break;
    if (i >= j) break;
    swap(arr, i, j);
  }
  swap(arr, lo, j);
  return j;
}

static void custom_qsort(std::array<S, N> &arr, int lo, int hi) {
  if (hi <= lo) return;

  int j = partition(arr, lo, hi);
  custom_qsort(arr, lo, j - 1);
  custom_qsort(arr, j + 1, hi);
}

void solution(std::array<S, N> &arr) {
  // qsort(arr.data(), arr.size(), sizeof(S), compare);
  // std::sort(arr.begin(), arr.end(), [](S &a, S &b) {
  //   return a.key1 < b.key1 || ((a.key1 == b.key1) && (a.key2 < b.key2));
  // });
  custom_qsort(arr, 0, arr.size() - 1);
}
