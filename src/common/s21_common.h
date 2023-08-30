#ifndef SRC_COMMON_S21_COMMON_H_
#define SRC_COMMON_S21_COMMON_H_
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME_LENGTH 200
#define ARGUMENT_MAX 200
#define BUFFER_MAX 4096

typedef struct file_node {
  char filename[FILENAME_LENGTH];
  struct file_node *next;
} file_node;

file_node *file_head = NULL;

int file_push(char *filename);
int file_pop(char *filename);
void clear_files();

#endif  // SRC_COMMON_S21_COMMON_H_
