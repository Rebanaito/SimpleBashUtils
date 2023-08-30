#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_
#define _GNU_SOURCE

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPTIONS 11
#define FILENAME_LENGTH 200
#define ARGUMENT_MAX 200
#define BUFFER_MAX 4096
#define OUTPUT_BUFFER_MAX 524288

typedef struct pattern_node {
  char pattern[BUFFER_MAX];
  struct pattern_node *next;
} pattern_node;

pattern_node *pattern_head = NULL;

typedef struct pattern_file_node {
  char filename[FILENAME_LENGTH];
  struct pattern_file_node *next;
} pattern_file_node;

pattern_file_node *pattern_file_head = NULL;

typedef struct match_node {
  char *p;
  size_t pattern_length;
  struct match_node *next;
} match_node;

match_node *match_head = NULL;

typedef struct flags {
  char letter;
  int value;
  int arg[ARGUMENT_MAX];
  int counter;
} flags;

typedef struct long_options {
  char option[25];
  int *value;
  int *arg;
  int *counter;
} long_flags;

int pattern_push(char *pattern);
int pattern_file_push(char *filename);
int pattern_file_pop(char *filname);
int match_push(char *pmatch, size_t pattern_length);
int match_pop(char *pmatch, size_t *pattern_length);
void clear_patterns();
void clear_pattern_files();
void clear_stacks();
void clear_matches();
void initial_setting(flags *selected_options, const char *available_options,
                     int *used_arguments);
void initial_long_options(long_flags *long_options, flags *selected_options,
                          const char *available_options_long[]);
int help(int argc, char **argv, long_flags *long_options);
void print_help();
int flag_processing(int argc, char **argv, const char *available_options,
                    flags *selected_options, long_flags *long_options,
                    int *used_arguments, int *argument_counter,
                    int *pattern_file_error);
int short_processing(int i, int argc, char **argv,
                     const char *available_options, flags *selected_options,
                     int *used_arguments, int *argument_counter);
int short_ef_flags(int i, int argc, char **argv, flags *selected_options,
                   int *used_arguments, int *argument_counter, int j, int k,
                   int *pattern_file_exit);
int long_processing(int i, char **argv, long_flags *long_options,
                    int *used_arguments, int *argument_counter);
int long_e_flag(int i, char **argv, long_flags *long_options,
                int *used_arguments, int *argument_counter, int *error, int j);
int long_f_flag(int i, char **argv, long_flags *long_options,
                int *used_arguments, int *argument_counter, int *error, int j);
int pattern_processing(char **argv, flags *selected_options,
                       int *pattern_file_error);
int pattern_files(flags *selected_options, int *pattern_file_error);
int stack_from_file(FILE *pattern_file);
int file_processing(int argc, char **argv, flags *selected_options,
                    int *used_arguments, int *argument_counter,
                    int pattern_file_error);
int look_for_files(int argc, char **argv, const int *used_arguments,
                   const int *argument_counter, int *file_counter,
                   int pattern_file_error);
int input_redirection(flags *selected_options);
int open_files(flags *selected_options, int file_counter);
int read_lines(flags *selected_options, char *filename, FILE *file,
               int file_counter, int *c_counter);
int read_patterns(flags *selected_options, char *output_buffer, int *c_counter);
int pattern_search(flags *selected_options, char *output_buffer, char *pattern);
void regex_compile(flags *selected_options, char *pattern, int *return_value,
                   regex_t *regex);
int regexec_func(flags *selected_options, char *output_buffer, char *s,
                 regex_t *regex, regmatch_t *pmatch, regoff_t *off,
                 regoff_t *len, int *return_value);
int regex_search(int *return_value, const char *output_buffer, char *s,
                 regex_t regex, regmatch_t *pmatch, regoff_t *off,
                 regoff_t *len);
void output(flags *selected_options, char *output_buffer, char *filename,
            int line_number, int file_counter);
void l_output(flags *selected_options, char *filename, const int *c_counter,
              int file_counter);
void c_output(flags *selected_options, char *filename, int *c_counter,
              int file_counter);
void normal_output(flags *selected_options, char *output_buffer, char *filename,
                   int line_number, int file_counter);
void o_output(flags *selected_options, char *output_buffer, char *filename,
              int file_counter, int line_number);
void print_output(flags *selected_options, char *output_buffer, char *filename,
                  int line_number, int file_counter);

#endif  // SRC_GREP_S21_GREP_H_
