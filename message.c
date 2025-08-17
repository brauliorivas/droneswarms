#include "message.h"
#include <stdio.h>

void print_error(const char *namespace, char *error_message) {
  fprintf(stderr, "[%s] %s\n", namespace, error_message);
}

void print_log(const char *namespace, char *info_message) {
  fprintf(stdout, "[%s] %s\n", namespace, info_message);
}
