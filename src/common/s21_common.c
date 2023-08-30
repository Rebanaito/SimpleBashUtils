#include "s21_common.h"

int file_push(char *filename) {
  int flag = 0;
  file_node *tmp = file_head;
  file_node *newnode;
  while (tmp != NULL && tmp->next != NULL) {
    tmp = tmp->next;
  }
  newnode = malloc(sizeof(file_node));
  if (newnode == NULL) {
    flag++;
  } else {
    strcpy(newnode->filename, filename);
    newnode->next = NULL;
    if (tmp != NULL) {
      tmp->next = newnode;
    } else {
      file_head = newnode;
    }
  }
  return flag;
}

int file_pop(char *filename) {
  int flag = 0;
  if (file_head == NULL) {
    flag = 1;
  } else {
    strcpy(filename, file_head->filename);
    file_node *tmp = file_head;
    file_head = file_head->next;
    free(tmp);
  }
  return flag;
}

void clear_files() {
  while (file_head != NULL) {
    file_node *tmp = file_head;
    file_head = file_head->next;
    free(tmp);
  }
}
