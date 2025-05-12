#include "solution.hpp"
#include <algorithm>
#include <cassert>
#include <type_traits>

using simd_score_t = std::array<int16_t, sequence_count_v>;
using simd_sequence_t = std::array<simd_score_t, sequence_size_v>;


simd_sequence_t transpose(std::vector<sequence_t> const &matrix) {
  simd_sequence_t result{};

  for (int col = 0; col < result.size(); ++col) {
    for (int row = 0; row < matrix.size(); ++row) {
      result[col][row] = matrix[row][col];
    }
  }

  return result;
}

// The alignment algorithm which computes the alignment of the given sequence
// pairs.
result_t compute_alignment(std::vector<sequence_t> const &sequences1,
                           std::vector<sequence_t> const &sequences2) {
  result_t result{};

  using column_t = std::array<simd_score_t, sequence_size_v + 1>;

  simd_sequence_t seq1_transpose = transpose(sequences1);
  simd_sequence_t seq2_transpose = transpose(sequences2);

  /*
   * Initialise score values.
   */
  simd_score_t gap_open;
  gap_open.fill(-11);
  simd_score_t gap_extension;
  gap_extension.fill(-1);
  simd_score_t match;
  match.fill(6);
  simd_score_t mismatch;
  mismatch.fill(-4);

  /*
   * Setup the matrix.
   * Note we can compute the entire matrix with just one column in memory,
   * since we are only interested in the last value of the last column in the
   * score matrix.
   */
  column_t score_column{};
  column_t horizontal_gap_column{};
  simd_score_t last_vertical_gap{};

  /*
   * Initialise the first column of the matrix.
   */
  horizontal_gap_column[0] = gap_open;
  last_vertical_gap = gap_open;

  for (size_t i = 1; i < score_column.size(); ++i) {
    for (size_t j = 0; j < sequence_count_v; ++j) {
      score_column[i][j] = last_vertical_gap[j];
      horizontal_gap_column[i][j] = last_vertical_gap[j] + gap_open[j];
      last_vertical_gap[j] += gap_extension[j];
    }
  }

  /*
   * Compute the main recursion to fill the matrix.
   */
  for (unsigned col = 1; col <= seq2_transpose.size(); ++col) {
    simd_score_t last_diagonal_score = score_column[0]; // Cache last diagonal score to compute this cell.
    for (size_t j = 0; j < sequence_count_v; ++j) {
      score_column[0][j] = horizontal_gap_column[0][j];
      last_vertical_gap[j] = horizontal_gap_column[0][j] + gap_open[j];
      horizontal_gap_column[0][j] += gap_extension[j];
    }

    for (unsigned row = 1; row <= seq1_transpose.size(); ++row) {
      // Compute next score from diagonal direction with match/mismatch.
      simd_score_t best_cell_score = last_diagonal_score;
      for (size_t j = 0; j < sequence_count_v; ++j) {
        best_cell_score[j] += (seq1_transpose[row - 1][j] == seq2_transpose[col - 1][j] ? match[j] : mismatch[j]);
      }
      
      for (size_t j = 0; j < sequence_count_v; ++j) {
        // Determine best score from diagonal, vertical, or horizontal direction.
        best_cell_score[j] = std::max(best_cell_score[j], last_vertical_gap[j]);
        best_cell_score[j] = std::max(best_cell_score[j], horizontal_gap_column[row][j]);
        
        // Cache next diagonal value and store optimum in score_column.
        last_diagonal_score[j] = score_column[row][j];
        score_column[row][j] = best_cell_score[j];
        
        // Compute the next values for vertical and horizontal gap.
        best_cell_score[j] += gap_open[j];
        last_vertical_gap[j] += gap_extension[j];
        horizontal_gap_column[row][j] += gap_extension[j];
        
        // Store optimum between gap open and gap extension.
        last_vertical_gap[j] = std::max(last_vertical_gap[j], best_cell_score[j]);
        horizontal_gap_column[row][j] = std::max(horizontal_gap_column[row][j], best_cell_score[j]);
      }
    }
  }

  // Report the best score.
  for (int j = 0; j < sequence_count_v; ++j) {
    result[j] = score_column.back()[j];
  }

  return result;
}
