#include "./s21_cat.h"

#include "../common/s21_common.c"

int main(int argc, char **argv) {
  int flag = 0;
  const char *available_options = "bEnsTvhet";
  const char *available_options_long[] = {
      "--number-nonblank", "--show-ends",        "--number", "--squeeze-blank",
      "--show-tabs",       "--show-nonprinting", "--help"};
  flags selected_options[OPTIONS] = {0};
  long_flags long_options[OPTIONS - 2] = {0};
  initial_setting(selected_options, available_options);
  initial_long_options(long_options, selected_options, available_options_long);
  if (help(argc, argv, long_options) == 1) {
    print_help();
  } else {
    int argument_counter = 1;
    if (flag_processing(argc, argv, selected_options, long_options,
                        &argument_counter) == 1) {
      flag++;
    } else {
      file_processing(argc, argv, selected_options, argument_counter);
      clear_files();
    }
  }
  return flag;
}

void initial_setting(struct flags *selected_options,
                     const char *available_options) {
  for (int i = 0; i < OPTIONS; i++) {
    selected_options[i].letter = available_options[i];
    selected_options[i].value = 0;
  }
}

void initial_long_options(long_flags *long_options, flags *selected_options,
                          const char *available_options_long[]) {
  for (int i = 0; i < OPTIONS - 2; i++) {
    strcpy(long_options[i].option, available_options_long[i]);
    long_options[i].value = &(selected_options[i].value);
  }
}

int help(int argc, char **argv, long_flags *long_options) {
  int flag = 0;
  for (int i = 1; i <= argc - 1; i++) {
    if (strcmp(argv[i], long_options[OPTIONS].option) == 0) {
      flag++;
      break;
    }
  }
  return flag;
}

void print_help() {
  puts(
      "NAME\n"
      "s21_cat - concatenate files and print on the standard output\n\n"
      "SYNOPSIS"
      "s21_cat [OPTION]... [FILE]...\n\n"
      "DESCRIPTION\n"
      "Concatenate FILE(s) to standard output.\n\n"
      "With no FILE, or when FILE is -, read standard input.\n\n"
      "-b, --number-nonblank\n"
      "number nonempty output lines, overrides -n\n\n"
      "-e,\n"
      "same as -vE\n"
      "-E, --show-ends\n"
      "display $ at end of each line\n\n"
      "-n, --number\n"
      "number all output lines\n\n"
      "-s, --squeeze-blank\n"
      "suppress repeated empty output lines\n"
      "-t,\n"
      "same as -vT\n"
      "-T, --show-tabs\n"
      "display TAB characters as ^I\n\n"
      "-h, --help\n"
      "display this help and exit\n\n"
      "EXAMPLES\n"
      "s21_cat f - g\n"
      "Output f's contents, then standard input, then g's contents.\n\n"
      "s21_cat    Copy standard input to standard output.\n\n"
      "AUTHOR\n"
      "Written by chronoss.");
}

int flag_processing(int argc, char **argv, flags *selected_options,
                    long_flags *long_options, int *argument_counter) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (long_processing(i, argv, long_options) == 1) {
          flag++;
          break;
        }
        (*argument_counter)++;
      } else {
        if (short_processing(i, argv, selected_options) == 1) {
          flag++;
          break;
        }
        (*argument_counter)++;
      }
    } else {
      break;
    }
  }
  return flag;
}

int short_processing(int i, char **argv, flags *selected_options) {
  int flag = 0;
  for (int j = 1; argv[i][j] != '\0'; j++) {
    int match = 0;
    for (int k = 0; k < OPTIONS && match == 0; k++) {
      if (argv[i][j] == selected_options[k].letter) {
        selected_options[k].value = 1;
        match++;
      }
    }
    if (match == 0) {
      fprintf(stderr, "Illegal option: -%c\n", argv[i][j]);
      flag++;
      break;
    }
  }
  return flag;
}

int long_processing(int i, char **argv, long_flags *long_options) {
  int flag = 0, match = 0;
  for (int j = 0; j < OPTIONS - 2 && match == 0; j++) {
    size_t flag_length = 0;
    flag_length = strlen(long_options[j].option);
    if (strncmp(argv[i], long_options[j].option, flag_length) == 0) match++;
  }
  if (match == 0) {
    fprintf(stderr, "Illegal option: %s\n", argv[i]);
    flag++;
  }
  return flag;
}

int file_processing(int argc, char **argv, flags *selected_options,
                    int argument_counter) {
  int flag = 0;
  if (look_for_files(argc, argv, argument_counter)) {
    flag++;
  } else {
    if (file_head == NULL) {
      fprintf(stderr, "No file provided\n");
      print_help();
    } else {
      if (open_files(selected_options) == 1) flag++;
    }
  }
  return flag;
}

int look_for_files(int argc, char **argv, int argument_counter) {
  int flag = 0;
  for (int i = argument_counter; i < argc; i++) {
    size_t filename_length = 0;
    filename_length = strlen(argv[i]) + 1;
    char *filename = (char *)malloc(filename_length * sizeof(char));
    if (filename == NULL) {
      fprintf(stderr, "Malloc on filenames\n");
      flag++;
      break;
    } else {
      strncpy(filename, argv[i], filename_length);
      if (file_push(filename)) {
        fprintf(stderr, "Malloc on file push\n");
        flag++;
        break;
      }
      free(filename);
    }
  }
  return flag;
}

int open_files(flags *selected_options) {
  int flag = 0;
  while (file_head != NULL) {
    size_t filename_length = 0;
    filename_length = strlen(file_head->filename) + 1;
    char *filename = (char *)malloc(filename_length * sizeof(char));
    if (filename == NULL) {
      fprintf(stderr, "Malloc on filename\n");
      flag++;
      break;
    } else {
      file_pop(filename);
      FILE *file = NULL;
      file = fopen(filename, "r");
      if (file == NULL) {
        fprintf(stderr, "cat: %s: No such file or directory\n", filename);
      } else {
        read_lines(selected_options, file);
        fclose(file);
      }
      free(filename);
    }
  }
  return flag;
}

void read_lines(flags *selected_options, FILE *file) {
  int empty_lines = 0, line_number = 1, new_line = 1;
  int letter;
  while ((letter = fgetc(file)) != EOF) {
    apply_options(letter, selected_options, &line_number, &empty_lines,
                  &new_line);
  }
}

void line_empty(int letter, flags *selected_options, int *line_number,
                int *empty_lines) {
  if (selected_options[3].value == 1 && *empty_lines == 1) {
  } else if ((selected_options[3].value == 1 && *empty_lines == 0) ||
             selected_options[3].value == 0) {
    *empty_lines = 1;
    if (selected_options[2].value == 1 && selected_options[0].value == 0) {
      printf("%6d\t", *line_number);
      (*line_number)++;
    }
    if (selected_options[1].value == 1 || selected_options[7].value == 1)
      printf("$");
    printf("%c", letter);
  }
}

void apply_options(int letter, flags *selected_options, int *line_number,
                   int *empty_lines, int *new_line) {
  if (letter == '\n' && *new_line == 1) {
    line_empty(letter, selected_options, line_number, empty_lines);
  } else if (*new_line == 1) {
    if ((selected_options[2].value == 1 && selected_options[0].value == 0) ||
        selected_options[0].value == 1) {
      printf("%6d\t", *line_number);
      (*line_number)++;
    }
    print_letter(letter, selected_options, empty_lines, new_line);
  } else if (letter == '\n') {
    if (selected_options[1].value == 1 || selected_options[7].value == 1)
      printf("$");
    printf("%c", letter);
    *new_line = 1;
  } else {
    print_letter(letter, selected_options, empty_lines, new_line);
  }
}

void print_letter(int letter, flags *selected_options, int *empty_lines,
                  int *new_line) {
  if ((letter == 9 || letter == 11) &&
      (selected_options[4].value == 1 || selected_options[8].value == 1)) {
    printf("^%c", letter + 64);
  } else if (selected_options[5].value == 1 || selected_options[7].value == 1 ||
             selected_options[8].value == 1) {
    non_printable(letter);
  } else {
    printf("%c", letter);
  }
  *new_line = 0;
  *empty_lines = 0;
}

void non_printable(int letter) {
  if (letter < -96) {
    printf("M-^%c", letter + 192);
  } else if (letter < 0) {
    printf("M-^%c", letter + 128);
  } else if (letter == 9 || letter == 10) {
    printf("%c", letter);
  } else if (letter < 32) {
    printf("^%c", letter + 64);
  } else if (letter < 127) {
    printf("%c", letter);
  } else if (letter == 127) {
    printf("^?");
  }
}
