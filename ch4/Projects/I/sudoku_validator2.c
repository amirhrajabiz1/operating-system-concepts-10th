#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 9 // Sudoku grid size

typedef struct {
  int row;
  int col;
} parameters;

int sudoku[N][N] = {
  // Initialize the Sudoku grid here
  // 0 represents empty cells
  {1, 3, 4, 6, 7, 8, 9, 2, 2},
  {6, 7, 2, 1, 9, 5, 3, 4, 8},
  {1, 9, 8, 3, 4, 2, 5, 6, 7},
  {8, 5, 9, 7, 6, 1, 4, 2, 3},
  {4, 2, 6, 8, 5, 3, 7, 9, 1},
  {7, 1, 3, 9, 2, 4, 8, 5, 6},
  {9, 6, 1, 5, 3, 7, 2, 8, 4},
  {2, 8, 7, 4, 1, 9, 6, 3, 5},
  {3, 4, 5, 2, 8, 6, 1, 7, 9}
};

int row_result[N] = {0};
int col_result[N] = {0};
int subgrid_result[N] = {0};


void *check_row(void *param) {
  parameters *data = (parameters *)param;
  int row = data->row;

  int used[10] = {0}; // To check digit occurrence
  for (int col = 0; col < N; ++col) {
    int digit = sudoku[row][col];
    if (digit < 1 || digit > 9 || used[digit]) {
      row_result[row] = 0;
      return NULL;
    }
    used[digit] = 1;
  }
  row_result[row] = 1;
  return NULL;
}

void *check_col(void *param) {
  parameters *data = (parameters *)param;
  int col = data->col;

  int used[10] = {0}; // To check digit occurrence
  for (int row = 0; row < N; ++row) {
    int digit = sudoku[row][col];
    if (digit < 1 || digit > 9 || used[digit]) {
      col_result[col] = 0;
      return NULL;
    }
    used[digit] = 1;
  }
  col_result[col] = 1;
  return NULL;
}


void *check_subgrid(void *param) {
  parameters *data = (parameters *)param;
  int start_row = data->row;
  int start_col = data->col;

  int used[10] = {0}; // To check digit occurrence
  for (int row = start_row; row < start_row + 3; ++row) {
    for (int col = start_col; col < start_col + 3; ++col) {
      int digit = sudoku[row][col];
      if (digit < 1 || digit > 9 || used[digit]) {
	subgrid_result[start_row + start_col / 3] = 0;
	return NULL;
      }
      used[digit] = 1;
    }
  }
  subgrid_result[start_row + start_col / 3] = 1;
  return NULL;
}

int main() {
  pthread_t threads[N * 3];
  parameters *data[N];
  parameters *data_for_subgrid[N];

  for (int i = 0; i < N; ++i) {
    data[i] = (parameters *)malloc(sizeof(parameters));
    data[i]->row = i;
    data[i]->col = i;

    pthread_create(&threads[i], NULL, check_row, data[i]);
    pthread_create(&threads[N + i], NULL, check_col, data[i]);
  }

  for (int i = 0; i < N; i += 3) {
    for (int j = 0; j < N; j += 3) {
      data_for_subgrid[i + j / 3] = (parameters *)malloc(sizeof(parameters));
      data_for_subgrid[i + j / 3]->row = i;
      data_for_subgrid[i + j / 3]->col = j;

      pthread_create(&threads[2 * N + i + j / 3], NULL, check_subgrid, data_for_subgrid[i + j / 3]);
    }
  }

  // Wait for threads to finish
  for (int i = 0; i < 3 * N; ++i) {
    pthread_join(threads[i], NULL);
  }

  // Check validity based on results
  int valid = 1;
  for (int i = 0; i < N; i++) {
    if (!row_result[i] || !col_result[i] || !subgrid_result[i]) {
      valid = 0;
      break;
    }
  }

  if (valid) {
    printf("Sudoku solution is valid.\n");
  } else {
    printf("Sudoku solution is not valid.\n");
  }

  return 0;
}
