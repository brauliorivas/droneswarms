#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

void parse_constans(char file[], Constants *constants) {
  FILE *fp;
  fp = fopen(file, "r");
  if (fp == NULL) {
    fprintf(stderr, "There was an error opening constants file");
    exit(1);
  }

  char var;
  int val;
  while (fscanf(fp, "%c %d", &var, &val) == 2) {
    switch (var) {
    case 'X':
      constants->x = val;
      break;
    case 'W':
      constants->w = val;
      break;
    case 'Q':
      constants->q = val;
      break;
    case 'Z':
      constants->z = val;
      break;
    default:
      break;
    }
  }

  fclose(fp);
}
