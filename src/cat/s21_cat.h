#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/s21_common.h"

#define OPTIONS 9

#define FILENAME_LENGTH 200
#define ARGUMENT_MAX 200
#define BUFFER_MAX 4096
#define STACK_EMPTY 69420

typedef struct flags {
  char letter;
  int value;
} flags;

typedef struct long_options {
  char option[20];
  int *value;
} long_flags;

void clear_files();
void initial_setting(flags *selected_options, const char *available_options);
void initial_long_options(long_flags *long_options, flags *selected_options,
                          const char *available_options_long[]);
int help(int argc, char **argv, long_flags *long_options);
void print_help();
int flag_processing(int argc, char **argv, flags *selected_options,
                    long_flags *long_options, int *argument_counter);
int short_processing(int i, char **argv, flags *selected_options);
int long_processing(int i, char **argv, long_flags *long_options);
int file_processing(int argc, char **argv, flags *selected_options,
                    int argument_counter);
int look_for_files(int argc, char **argv, int argument_counter);
int open_files(flags *selected_options);
void read_lines(flags *selected_options, FILE *file);
void apply_options(int letter, flags *selected_options, int *line_number,
                   int *empty_lines, int *new_line);
void print_letter(int letter, flags *selected_options, int *empty_lines,
                  int *new_line);
void line_empty(int letter, flags *selected_options, int *line_number,
                int *empty_lines);
void line_not_empty(int letter, flags *selected_options, int *line_number,
                    int *empty_lines, int *new_line);
void non_printable(int letter);

#endif  // SRC_CAT_S21_CAT_H_
