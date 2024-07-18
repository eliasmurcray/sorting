#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#define ARRAY_SIZE 50000
#define MAX_ARRAY_VALUE 1024
#define BUCKET_COUNT 10

struct HashNode {
  int key;
  int value;
  struct HashNode* next;
};

void free_buckets(struct HashNode **buckets) {
  for (int i = 0; i < BUCKET_COUNT; i++) {
    struct HashNode *curr = buckets[i], *next;
    while (curr) {
      next = curr->next;
      free(curr);
      curr = next;
    }
  }
  free(buckets);
}

int *countsort(int *arr, int n) {
  struct HashNode **buckets = malloc(sizeof(struct HashNode*) * BUCKET_COUNT);
  for (int i = 0; i < BUCKET_COUNT; i++) {
    buckets[i] = NULL;
  }

  for (int i = 0; i < n; i++) {
    int j = arr[i] % BUCKET_COUNT;
    if (!buckets[j]) {
      struct HashNode *new_node = malloc(sizeof(struct HashNode));
      new_node->key = arr[i];
      new_node->value = 1;
      new_node->next = NULL;
      buckets[j] = new_node;
      continue;
    }
    struct HashNode *curr = buckets[j];
    int found = 0;
    while (curr) {
      if (curr->key == arr[i]) {
        curr->value ++;
        found = 1;
        break;
      }
      if (!curr->next) break;
      curr = curr->next;
    }
    if (found) continue;
    struct HashNode *new_node = malloc(sizeof(struct HashNode));
    new_node->key = arr[i];
    new_node->value = 1;
    new_node->next = NULL;
    curr->next = new_node;
  }

  int *new_arr = malloc(n * sizeof(int)), *p = new_arr;
  for (int i = 0; i < MAX_ARRAY_VALUE; i++) {
    int j = i % BUCKET_COUNT;
    if (!buckets[j]) continue;
    struct HashNode *curr = buckets[j];
    while (curr) {
      if (curr->key == i) {
        for (int k = 0; k < curr->value; k++, p++) {
          *p = i;
        }
        break;
      }
      curr = curr->next;
    }
  }
  free_buckets(buckets);
  return new_arr;
}

void print_arr(int *arr, int n) {
  if (arr == NULL) return;
  for (int i = 0; i < n - 1; i++) {
    printf("%d ", arr[i]);
  }
  printf("%d\n", arr[n - 1]);
}

int verify(int *arr, int n) {
  if (arr == NULL) return 1;
  int i = 0, last = -1;
  for (; i < n; i++) {
    if (arr[i] < last) return 1;
    last = arr[i];
  }
  return 0;
}

int main() {
  srand(time(NULL));
  int* arr = malloc(4 * ARRAY_SIZE);
  if (!arr) {
    perror("Failed to malloc arr");
    return 1;
  }
  for (int i = 0; i < ARRAY_SIZE; i++) {
    arr[i] = rand() % MAX_ARRAY_VALUE;
  }
  printf("Running countsort...\n");
  struct timespec end, start;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  int* sorted = countsort(arr, ARRAY_SIZE);
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
  printf("Verifying...\n");
  if (verify(sorted, ARRAY_SIZE)) {
    printf("Failed to sort %d objects.\n", ARRAY_SIZE);
    print_arr(sorted, ARRAY_SIZE);
  } else {
    printf("Successfully sorted %d objects in %" PRIu64 "us.\n", ARRAY_SIZE, delta_us);
  }
  free(arr);
  free(sorted);
}
