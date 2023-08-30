#include "./s21_grep.h"

#include "../common/s21_common.c"

int main(int argc, char **argv) {
  int flag = 0;
  const char *available_options = "eEivclnhsfo";
  const char *available_options_long[] = {
      "--regexp=",      "--extended-regexp", "--ignore-case",
      "--invert-match", "--count",           "--files-with-matches",
      "--line-number",  "--no-filename",     "--no-messages",
      "--file=",        "--only-matching",   "--help"};
  flags selected_options[OPTIONS] = {0};
  long_flags long_options[OPTIONS + 1] = {0};
  int used_arguments[ARGUMENT_MAX] = {0};
  int argument_counter = 0;
  initial_setting(selected_options, available_options, used_arguments);
  initial_long_options(long_options, selected_options, available_options_long);
  if (help(argc, argv, long_options) == 1) {
    print_help();
  } else {
    int pattern_file_error = 0;
    if (flag_processing(argc, argv, available_options, selected_options,
                        long_options, used_arguments, &argument_counter,
                        &pattern_file_error) == 1) {
      clear_stacks();
      print_help();
      flag++;
    }
    if (flag == 0) {
      if (file_processing(argc, argv, selected_options, used_arguments,
                          &argument_counter, pattern_file_error) == 1) {
        clear_stacks();
        clear_matches();
      }
    }
    clear_stacks();
    clear_matches();
  }
  return flag;
}

int pattern_push(char *pattern) {
  int flag = 0;
  pattern_node *newnode = malloc(sizeof(pattern_node));
  if (newnode == NULL) {
    flag++;
  } else {
    strcpy(newnode->pattern, pattern);
    newnode->next = pattern_head;
    pattern_head = newnode;
  }
  return flag;
}

int pattern_file_push(char *filename) {
  int flag = 0;
  pattern_file_node *newnode = malloc(sizeof(pattern_file_node));
  if (newnode == NULL) {
    flag++;
  } else {
    strcpy(newnode->filename, filename);
    newnode->next = pattern_file_head;
    pattern_file_head = newnode;
  }
  return flag;
}

int pattern_file_pop(char *filename) {
  int flag = 0;
  if (pattern_file_head == NULL) {
    flag++;
  } else {
    strcpy(filename, pattern_file_head->filename);
    pattern_file_node *tmp = pattern_file_head;
    pattern_file_head = pattern_file_head->next;
    free(tmp);
  }
  return flag;
}

int match_push(char *pmatch, size_t pattern_length) {
  int flag = 0;
  match_node *newnode = malloc(sizeof(match_node));
  if (newnode == NULL) {
    flag++;
  } else {
    newnode->p = pmatch;
    newnode->pattern_length = pattern_length;
    if (match_head == NULL || newnode->p < match_head->p) {
      newnode->next = match_head;
      match_head = newnode;
    } else {
      match_node *current = match_head;
      while (newnode->p > current->p && current->next != NULL) {
        current = current->next;
      }
      newnode->next = current->next;
      current->next = newnode;
    }
  }
  return flag;
}

int match_pop(char *pmatch, size_t *pattern_length) {
  int flag = 0;
  if (match_head == NULL) {
    flag++;
  } else {
    *pattern_length = match_head->pattern_length;
    strncpy(pmatch, match_head->p, *pattern_length);
    match_node *tmp = match_head;
    match_head = match_head->next;
    free(tmp);
  }
  return flag;
}

void clear_patterns() {
  while (pattern_head != NULL) {
    pattern_node *tmp = pattern_head;
    pattern_head = pattern_head->next;
    free(tmp);
  }
}

void clear_pattern_files() {
  while (pattern_file_head != NULL) {
    pattern_file_node *tmp = pattern_file_head;
    pattern_file_head = pattern_file_head->next;
    free(tmp);
  }
}

void clear_stacks() {
  clear_patterns();
  clear_files();
  clear_pattern_files();
}

void clear_matches() {
  while (match_head != NULL) {
    match_node *tmp = match_head;
    match_head = match_head->next;
    free(tmp);
  }
}

void initial_setting(struct flags *selected_options,
                     const char *available_options, int *used_arguments) {
  for (int i = 0; i < OPTIONS; i++) {
    selected_options[i].letter = available_options[i];
    selected_options[i].value = 0;
    selected_options[i].counter = 0;
    for (int j = 0; j < ARGUMENT_MAX; j++) {
      selected_options[i].arg[j] = 0;
    }
  }
  for (int i = 0; i < ARGUMENT_MAX; i++) {
    used_arguments[i] = 0;
  }
}

void initial_long_options(long_flags *long_options, flags *selected_options,
                          const char *available_options_long[]) {
  for (int i = 0; i < OPTIONS; i++) {
    strcpy(long_options[i].option, available_options_long[i]);
    long_options[i].value = &(selected_options[i].value);
    long_options[i].counter = &(selected_options[i].counter);
    for (int j = 0; j < ARGUMENT_MAX; j++) {
      long_options[i].arg = selected_options[i].arg;
    }
  }
}

int help(int argc, char **argv, long_flags *long_options) {
  int need_help = 0;
  if (argc == 1) {
    fprintf(stderr, "s21_grep requires arguments, see the manual below\n");
    need_help++;
  } else {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], long_options[OPTIONS].option) == 0) {
        need_help = 1;
        break;
      }
    }
  }
  return need_help;
}

int flag_processing(int argc, char **argv, const char *available_options,
                    flags *selected_options, long_flags *long_options,
                    int *used_arguments, int *argument_counter,
                    int *pattern_file_error) {
  int flag = 0;
  for (int i = 1; i <= argc - 1; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (long_processing(i, argv, long_options, used_arguments,
                            argument_counter) == 1) {
          flag++;
          break;
        }
      } else {
        if (short_processing(i, argc, argv, available_options, selected_options,
                             used_arguments, argument_counter) == 1) {
          flag++;
          break;
        }
      }
    }
  }
  if (pattern_processing(argv, selected_options, pattern_file_error) == 1)
    flag++;
  return flag;
}

int short_processing(int i, int argc, char **argv,
                     const char *available_options, flags *selected_options,
                     int *used_arguments, int *argument_counter) {
  int pattern_file_exit = 0, flag = 0;
  for (int j = 1; argv[i][j] != '\0' && pattern_file_exit == 0 && flag == 0;
       j++) {
    int match = 0;
    for (int k = 0; k < OPTIONS && match == 0 && flag == 0; k++) {
      if (argv[i][j] == available_options[k]) {
        selected_options[k].value = 1;
        used_arguments[*argument_counter] = i;
        (*argument_counter)++;
        if (k == 0 || k == 9) {
          if (short_ef_flags(i, argc, argv, selected_options, used_arguments,
                             argument_counter, j, k, &pattern_file_exit) == 1) {
            flag++;
            break;
          }
        } else {
          selected_options[k].arg[selected_options[k].counter] = i;
          (selected_options[k].counter)++;
        }
        match++;
      }
    }
    if (match == 0 && flag == 0) {
      fprintf(stderr, "Illegal option: -%c\n", argv[i][j]);
      flag++;
      break;
    }
  }
  return flag;
}

int short_ef_flags(int i, int argc, char **argv, flags *selected_options,
                   int *used_arguments, int *argument_counter, int j, int k,
                   int *pattern_file_exit) {
  int flag = 0;
  if (argv[i][j + 1] != '\0') {
    char *string = NULL;
    string = (char *)malloc(ARGUMENT_MAX * sizeof(char));
    if (string == NULL) {
      fprintf(stderr, "Malloc on short flag\n");
      flag++;
    } else {
      strcpy(string, argv[i] + j + 1);
      int check = 0;
      switch (k) {
        case 0:
          check = pattern_push(string);
          break;
        case 9:
          check = pattern_file_push(string);
          break;
      }
      free(string);
      if (check == 1) {
        fprintf(stderr, "Malloc on patter/pattern file push\n");
        flag++;
      }
      (*pattern_file_exit)++;
    }
  } else if (i == argc - 1) {
    fprintf(stderr, "grep: option requires an argument -- %c\n", argv[i][j]);
    flag++;
  } else {
    used_arguments[*argument_counter] = i + 1;
    (*argument_counter)++;
    selected_options[k].arg[selected_options[k].counter] = i;
    (selected_options[k].counter)++;
  }
  return flag;
}

int long_processing(int i, char **argv, long_flags *long_options,
                    int *used_arguments, int *argument_counter) {
  int match = 0, error = 0, flag = 0;
  for (int j = 0; j < OPTIONS && match == 0; j++) {
    if (j == 0) {
      if (long_e_flag(i, argv, long_options, used_arguments, argument_counter,
                      &error, j) == 1) {
        flag++;
        break;
      }
    } else if (j == 9) {
      if (long_f_flag(i, argv, long_options, used_arguments, argument_counter,
                      &error, j) == 1) {
        flag++;
        break;
      }
    } else if (strcmp(argv[i], long_options[j].option) == 0) {
      *(long_options[j].value) = 1;
      *(long_options[j].counter) = 1;
      match++;
      used_arguments[*argument_counter] = i;
      (*argument_counter)++;
    }
  }
  if (match == 0 && error != 0) {
    fprintf(stderr, "Illegal option: %s\n", argv[i]);
    flag++;
  }
  return flag;
}

int long_e_flag(int i, char **argv, long_flags *long_options,
                int *used_arguments, int *argument_counter, int *error, int j) {
  *error = 0;
  int flag = 0;
  for (int k = 0; k < 9 && *error == 0; k++) {
    if (long_options[j].option[k] != argv[i][k]) (*error)++;
  }
  if (*error == 0) {
    char *string = NULL;
    string = (char *)malloc(BUFFER_MAX * sizeof(char));
    if (string == NULL) {
      fprintf(stderr, "Malloc on long flag e\n");
      flag++;
    }
    if (flag == 0) {
      strcpy(string, argv[i] + 9);
      int check = 0;
      check = pattern_push(string);
      (*long_options[j].value) = 1;
      used_arguments[*argument_counter] = i;
      (*argument_counter)++;
      (*long_options[j].counter)++;
      free(string);
      if (check == 1) {
        fprintf(stderr, "Malloc on pattern push\n");
        flag++;
      }
    }
  }
  return flag;
}

int long_f_flag(int i, char **argv, long_flags *long_options,
                int *used_arguments, int *argument_counter, int *error, int j) {
  *error = 0;
  int flag = 0;
  for (int k = 0; k < 7 && *error == 0; k++) {
    if (long_options[j].option[k] != argv[i][k]) error++;
  }
  if (*error == 0) {
    char *string = NULL;
    string = (char *)malloc(strlen(argv[i]) * sizeof(char));
    if (string == NULL) {
      fprintf(stderr, "Malloc on long flag f\n");
      flag++;
    }
    if (flag == 0) {
      strcpy(string, argv[i] + 7);
      int check = 0;
      check = pattern_file_push(string);
      (*long_options[j].value) = 1;
      used_arguments[*argument_counter] = i;
      (*argument_counter)++;
      (*long_options[j].counter)++;
      free(string);
      if (check == 1) {
        fprintf(stderr, "Malloc on pattern file push\n");
        flag++;
      }
    }
  }
  return flag;
}

int pattern_processing(char **argv, flags *selected_options,
                       int *pattern_file_error) {
  int flag = 0;
  for (int i = 0; i < selected_options[9].counter; i++) {
    char *filename = (char *)malloc(FILENAME_LENGTH * sizeof(char));
    if (filename == NULL) {
      fprintf(stderr, "Malloc on pattern filename\n");
      flag++;
      break;
    }
    strcpy(filename, argv[selected_options[9].arg[i] + 1]);
    int check = pattern_file_push(filename);
    free(filename);
    if (check == 1) {
      fprintf(stderr, "Malloc on pattern file processing\n");
      flag++;
      break;
    }
  }
  for (int j = 0; j < selected_options[0].counter && flag == 0; j++) {
    char *pattern = NULL;
    pattern = (char *)malloc(BUFFER_MAX * sizeof(char));
    if (pattern == NULL) {
      fprintf(stderr, "Malloc on pattern\n");
      flag++;
      break;
    }
    strcpy(pattern, argv[selected_options[0].arg[j] + 1]);
    int check = pattern_push(pattern);
    free(pattern);
    if (check == 1) {
      fprintf(stderr, "Malloc on pattern push from file\n");
      flag++;
      break;
    }
  }
  if (flag == 0) pattern_files(selected_options, pattern_file_error);
  return flag;
}

int pattern_files(flags *selected_options, int *pattern_file_error) {
  int flag = 0;
  while (pattern_file_head != NULL) {
    char *filename = NULL;
    filename = (char *)malloc(FILENAME_LENGTH * sizeof(char));
    if (filename == NULL) {
      fprintf(stderr, "Malloc on pattern filename\n");
      flag++;
      break;
    }
    pattern_file_pop(filename);
    FILE *pattern_file = NULL;
    pattern_file = fopen(filename, "r");
    if (pattern_file == NULL && selected_options[8].value == 0) {
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
      (*pattern_file_error)++;
      free(filename);
      continue;
    } else if (pattern_file == NULL) {
      (*pattern_file_error)++;
      free(filename);
      continue;
    } else {
      if (stack_from_file(pattern_file) == 1) {
        free(filename);
        fclose(pattern_file);
        flag++;
        break;
      }
      free(filename);
      fclose(pattern_file);
    }
  }
  return flag;
}

int stack_from_file(FILE *pattern_file) {
  int flag = 0;
  char *string = NULL;
  string = (char *)malloc(BUFFER_MAX * sizeof(char));
  if (string == NULL) {
    fprintf(stderr, "Malloc on pattern file string\n");
    flag++;
  } else {
    int check = 0;
    size_t line_buffer = BUFFER_MAX;
    while ((int)getline(&string, &line_buffer, pattern_file) != EOF) {
      for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] == '\n' && string[i + 1] == '\0' && i != 0)
          string[i] = '\0';
      }
      check = pattern_push(string);
    }
    free(string);
    if (check == 1) {
      fprintf(stderr, "Malloc on pattern push from file\n");
      flag++;
    }
  }
  return flag;
}

int file_processing(int argc, char **argv, flags *selected_options,
                    int *used_arguments, int *argument_counter,
                    int pattern_file_error) {
  int file_counter = 0, flag = 0;
  if (look_for_files(argc, argv, used_arguments, argument_counter,
                     &file_counter, pattern_file_error) == 1) {
    flag++;
  }
  if (file_head == NULL && flag == 0) {
    if (input_redirection(selected_options) == 1) flag++;
  } else if (flag == 0) {
    if (open_files(selected_options, file_counter) == 1) flag++;
  }
  return flag;
}

int look_for_files(int argc, char **argv, const int *used_arguments,
                   const int *argument_counter, int *file_counter,
                   int pattern_file_error) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    int match = 0;
    for (int j = 0; j < *argument_counter && match == 0; j++) {
      if (i == used_arguments[j]) match++;
    }
    if (match == 0) {
      int check = 0;
      if (pattern_head == NULL && pattern_file_error == 0) {
        check = pattern_push(argv[i]);
      } else {
        check = file_push(argv[i]);
        (*file_counter)++;
      }
      if (check == 1) {
        fprintf(stderr, "Malloc on file or pattern push\n");
        flag++;
        break;
      }
    }
  }
  return flag;
}

int input_redirection(flags *selected_options) {
  int flag = 0;
  if (pattern_head == NULL) {
    print_help();
    flag++;
  } else {
    size_t buffer_size = BUFFER_MAX;
    char *output_buffer = NULL;
    output_buffer = (char *)malloc(BUFFER_MAX * sizeof(char));
    if (output_buffer == NULL) {
      fprintf(stderr, "Malloc on line buffer\n");
    } else {
      int line_number = 1, c_counter = 0;
      while ((int)getline(&output_buffer, &buffer_size, stdin) != EOF) {
        if (read_patterns(selected_options, output_buffer, &c_counter) == 1) {
          free(output_buffer);
          flag++;
          break;
        }
        if (flag == 0) {
          if (selected_options[4].value == 0 &&
              selected_options[5].value == 0) {
            output(selected_options, output_buffer, "stdin", line_number, 1);
          }
          line_number++;
          clear_matches();
        }
        if (selected_options[5].value == 1) {
          l_output(selected_options, "stdin", &c_counter, 1);
        } else if (selected_options[5].value == 0 &&
                   selected_options[4].value == 1) {
          c_output(selected_options, "stdin", &c_counter, 1);
        }
        clear_matches();
      }
      free(output_buffer);
    }
  }
  return flag;
}

int open_files(flags *selected_options, int file_counter) {
  int flag = 0;
  while (file_head != NULL) {
    size_t filename_length = 0;
    filename_length = strlen(file_head->filename) + 1;
    char *filename = NULL;
    filename = (char *)malloc(filename_length * sizeof(char));
    if (filename == NULL) {
      fprintf(stderr, "Malloc on filename\n");
      flag++;
      break;
    } else {
      file_pop(filename);
      FILE *file = NULL;
      file = fopen(filename, "r");
      if (file == NULL && selected_options[8].value == 0) {
        fprintf(stderr, "grep: %s: No such file or directory\n", filename);
        free(filename);
        continue;
      } else if (file == NULL && selected_options[8].value == 1) {
        free(filename);
        continue;
      } else {
        int c_counter = 0;
        if (read_lines(selected_options, filename, file, file_counter,
                       &c_counter) == 1) {
          fclose(file);
          free(filename);
          flag++;
          break;
        }
        fclose(file);
      }
      free(filename);
    }
  }
  return flag;
}

int read_lines(flags *selected_options, char *filename, FILE *file,
               int file_counter, int *c_counter) {
  int flag = 0;
  size_t buffer_size = BUFFER_MAX;
  char *output_buffer = NULL;
  output_buffer = (char *)malloc(buffer_size * sizeof(char));
  if (output_buffer == NULL) {
    fprintf(stderr, "Malloc on output buffer\n");
    flag++;
  } else {
    int line_number = 1;
    while ((int)getline(&output_buffer, &buffer_size, file) != EOF) {
      if (read_patterns(selected_options, output_buffer, c_counter) == 1) {
        flag++;
        break;
      }
      if (selected_options[4].value == 0 && selected_options[5].value == 0) {
        output(selected_options, output_buffer, filename, line_number,
               file_counter);
      }
      line_number++;
      clear_matches();
    }
    if (selected_options[5].value == 1 && flag == 0) {
      l_output(selected_options, filename, c_counter, file_counter);
    } else if (selected_options[5].value == 0 &&
               selected_options[4].value == 1 && flag == 0) {
      c_output(selected_options, filename, c_counter, file_counter);
    }
    free(output_buffer);
  }
  return flag;
}

int read_patterns(flags *selected_options, char *output_buffer,
                  int *c_counter) {
  int flag = 0;
  pattern_node *current = pattern_head;
  while (current != NULL) {
    size_t pattern_length = 0;
    pattern_length = strlen(current->pattern) + 1;
    char *pattern = NULL;
    pattern = (char *)malloc(pattern_length * sizeof(char));
    if (pattern == NULL) {
      fprintf(stderr, "Malloc on output buffer\n");
      flag++;
      break;
    } else {
      strcpy(pattern, current->pattern);
      int result = 0;
      result = pattern_search(selected_options, output_buffer, pattern);
      if (result == -1) {
        free(pattern);
        flag++;
        break;
      } else if (result == 1 && selected_options[5].value == 1 &&
                 selected_options[3].value == 0) {
        free(pattern);
        break;
      } else if (result == 1 && selected_options[4].value == 1 &&
                 selected_options[3].value == 0) {
        free(pattern);
        break;
      }
      free(pattern);
    }
    current = current->next;
  }
  if ((match_head == NULL && selected_options[3].value == 1) ||
      (match_head != NULL && selected_options[3].value == 0))
    (*c_counter)++;
  return flag;
}

int pattern_search(flags *selected_options, char *output_buffer,
                   char *pattern) {
  int flag = 0;
  char *s = output_buffer;
  regex_t regex;
  regmatch_t pmatch[1] = {0};
  regoff_t off, len;
  int return_value = 0;
  regex_compile(selected_options, pattern, &return_value, &regex);
  if (return_value && selected_options[8].value == 0) {
    char error[BUFFER_MAX] = {""};
    regerror(return_value, &regex, error, 100);
    printf("regcomp() failed with '%s'\n", error);
    flag = -1;
  } else if (return_value && selected_options[8].value == 1) {
    flag = -1;
  } else {
    int check = 0;
    check = regexec_func(selected_options, output_buffer, s, &regex, pmatch,
                         &off, &len, &return_value);
    if (check == -1) {
      flag = -1;
    } else if (check == 1) {
      flag = 1;
    }
  }
  regfree(&regex);
  return flag;
}

void regex_compile(flags *selected_options, char *pattern, int *return_value,
                   regex_t *regex) {
  if (selected_options[1].value == 1) {
    switch (selected_options[2].value) {
      case 0:
        *return_value = regcomp(regex, pattern, REG_EXTENDED + REG_NEWLINE);
        break;
      case 1:
        *return_value =
            regcomp(regex, pattern, REG_EXTENDED + REG_NEWLINE + REG_ICASE);
        break;
    }
  } else {
    switch (selected_options[2].value) {
      case 0:
        *return_value = regcomp(regex, pattern, REG_NEWLINE);
        break;
      case 1:
        *return_value = regcomp(regex, pattern, REG_NEWLINE + REG_ICASE);
        break;
    }
  }
}

int regexec_func(flags *selected_options, char *output_buffer, char *s,
                 regex_t *regex, regmatch_t *pmatch, regoff_t *off,
                 regoff_t *len, int *return_value) {
  int check, flag = 0;
  *return_value = regexec(regex, s, 1, pmatch, 0);
  if (*return_value == 0) {
    *off = pmatch[0].rm_so + (s - output_buffer);
    *len = pmatch[0].rm_eo - pmatch[0].rm_so;
    check = match_push(s + pmatch[0].rm_so, *len);
    if (check == 1) {
      fprintf(stderr, "Malloc on match push\n");
      flag = -1;
    }
    s += pmatch[0].rm_eo;
  }
  if (flag == 0) {
    if (*return_value == 0 && selected_options[3].value == 1) {
    } else if (*return_value == REG_NOMATCH && selected_options[3].value == 0) {
    } else if (*return_value == REG_NOMATCH && selected_options[3].value == 1) {
      flag = 1;
    } else if (*return_value == 0 && selected_options[3].value == 0) {
      check = regex_search(return_value, output_buffer, s, *regex, pmatch, off,
                           len);
      if (check == -1) {
        flag = -1;
      } else {
        flag = 1;
      }
    }
  }
  return flag;
}

int regex_search(int *return_value, const char *output_buffer, char *s,
                 regex_t regex, regmatch_t *pmatch, regoff_t *off,
                 regoff_t *len) {
  int flag = 0;
  for (int i = 0;; i++) {
    *return_value = regexec(&regex, s, 1, pmatch, 0);
    if (*return_value) break;
    *off = pmatch[0].rm_so + (s - output_buffer);
    *len = pmatch[0].rm_eo - pmatch[0].rm_so;
    int check = 0;
    check = match_push(s + pmatch[0].rm_so, *len);
    if (check == 1) {
      fprintf(stderr, "Malloc on match push\n");
      flag = -1;
    }
    s += pmatch[0].rm_eo;
  }
  return flag;
}

void output(flags *selected_options, char *output_buffer, char *filename,
            int line_number, int file_counter) {
  if (selected_options[10].value == 1) {
    o_output(selected_options, output_buffer, filename, file_counter,
             line_number);
  } else {
    normal_output(selected_options, output_buffer, filename, line_number,
                  file_counter);
  }
}

void l_output(flags *selected_options, char *filename, const int *c_counter,
              int file_counter) {
  if (*c_counter != 0) {
    if (selected_options[4].value == 1 && selected_options[7].value == 0 &&
        file_counter > 1) {
      printf("%s:", filename);
    }
    if (selected_options[4].value == 1) printf("1\n");
    printf("%s\n", filename);
  } else {
    if (selected_options[4].value == 1) {
      if (selected_options[7].value == 0) printf("%s:", filename);
      printf("0\n");
    }
  }
}

void c_output(flags *selected_options, char *filename, int *c_counter,
              int file_counter) {
  if (selected_options[7].value == 0 && file_counter > 1)
    printf("%s:", filename);
  printf("%d\n", *c_counter);
}

void o_output(flags *selected_options, char *output_buffer, char *filename,
              int file_counter, int line_number) {
  if (match_head == NULL && selected_options[3].value == 1) {
    if (selected_options[7].value == 0 && file_counter > 1)
      printf("%s:", filename);
    if (selected_options[6].value == 1) printf("%d:", line_number);
    for (int i = 0; output_buffer[i] != '\0'; i++) {
      printf("%c", output_buffer[i]);
      if (output_buffer[i + 1] == '\0' && output_buffer[i] != '\n') {
        printf("\n");
      }
    }
  } else if (match_head != NULL && selected_options[3].value == 0) {
    if (selected_options[7].value == 0 && file_counter > 1)
      printf("%s:", filename);
    if (selected_options[6].value == 1) printf("%d:", line_number);
    while (match_head != NULL) {
      char *pmatch = NULL;
      pmatch = (char *)malloc((match_head->pattern_length) * sizeof(char));
      size_t pattern_length = 0;
      match_pop(pmatch, &pattern_length);
      printf("%.*s\n", (int)pattern_length, pmatch);
      free(pmatch);
    }
  }
}

void normal_output(flags *selected_options, char *output_buffer, char *filename,
                   int line_number, int file_counter) {
  if ((match_head == NULL && selected_options[3].value == 1) ||
      (match_head != NULL && selected_options[3].value == 0)) {
    print_output(selected_options, output_buffer, filename, line_number,
                 file_counter);
  }
}

void print_output(flags *selected_options, char *output_buffer, char *filename,
                  int line_number, int file_counter) {
  if (file_counter > 1 && selected_options[7].value == 0)
    printf("%s:", filename);
  if (selected_options[6].value == 1) printf("%d:", line_number);
  for (int i = 0; output_buffer[i] != '\0'; i++) {
    printf("%c", output_buffer[i]);
    if (output_buffer[i + 1] == '\0' && output_buffer[i] != '\n') {
      printf("\n");
    }
  }
}

void print_help() {
  fprintf(
      stderr,
      "\n\nNAME\n"
      "\ts21_grep - print lines that match patterns\n\n"

      "SYNOPSIS\n"
      "\ts21_grep [OPTION...] PATTERNS [FILE...]\n"
      "\ts21_grep [OPTION...] -e PATTERNS ... [FILE...]\n"
      "\ts21_grep [OPTION...] -f PATTERN_FILE ... [FILE...]\n"

      "DESCRIPTION\n"
      "\ts21_grep  searches  for  PATTERNS  in  each  FILE.  PATTERNS is one "
      "or\n"
      "\tpatterns separated by newline characters,  and  grep  prints  each\n"
      "\tline that matches a pattern.\n\n"

      "\tIf no FILE is given, searches read standard input.\n\n"

      "OPTIONS\n"
      "\t--help Output a usage message and exit.\n\n"

      "\t-E, --extended-regexp\n"
      "\t\tInterpret  PATTERNS  as extended regular expressions (EREs,\n"
      "\t\tsee below).\n\n"

      "\t-e PATTERNS, --regexp=PATTERNS\n"
      "\t\tUse  PATTERNS  as  the  patterns.   If  this option is used\n"
      "\t\tmultiple times or is combined with the -f (--file)  option,\n"
      "\t\tsearch  for all patterns given.  This option can be used to\n"
      "\t\tprotect a pattern beginning with “-”.\n\n"

      "\t-f FILE, --file=FILE\n"
      "\t\tObtain patterns from FILE, one per line.  If this option is\n"
      "\t\tused  multiple  times or is combined with the -e (--regexp)\n"
      "\t\toption, search for all  patterns  given.   The  empty  file\n"
      "\t\tcontains zero patterns, and therefore matches nothing.\n\n"

      "\t-i, --ignore-case\n"
      "\t\tIgnore  case  distinctions,  so that characters that differ\n"
      "\t\tonly in case match each other.\n\n"

      "\t-v, --invert-match\n"
      "\t\tInvert the sense of matching, to select non-matching lines.\n\n"

      "\t-c, --count\n"
      "\t\tSuppress normal output; instead print a count  of  matching\n"
      "\t\tlines  for  each  input  file.  With the -v, --invert-match\n"
      "\t\toption (see below), count non-matching lines.\n\n"

      "\t-l, --files-with-matches\n"
      "\t\tSuppress  normal  output;  instead  print  the name of each\n"
      "\t\tinput file from  which  output  would  normally  have  been\n"
      "\t\tprinted.  The scanning will stop on the first match.\n\n"

      "\t-o, --only-matching\n"
      "\t\tPrint  only  the  matched  (non-empty)  parts of a matching\n"
      "\t\tline, with each such part on a separate output line.\n\n"

      "\t-s, --no-messages\n"
      "\t\tSuppress  error  messages  about  nonexistent or unreadable\n"
      "\t\tfiles.\n\n"

      "\t-h, --no-filename\n"
      "\t\tSuppress  the  prefixing  of file names on output.  This is\n"
      "\t\tthe default when there is only one file (or  only  standard\n"
      "\t\tinput) to search.\n\n"

      "\t-n, --line-number\n"
      "\t\tPrefix each line of output with  the  1-based  line  number\n"
      "\t\twithin its input file.\n");
}
