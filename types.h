#ifndef _TYPES_H

#define _TYPES_H

#include "stdio.h"

/* 
-f, --ignore-case fold lower case to upper case characters
-r, --reverse reverse the result of comparisons
-R, --random-sort shuffle.
-c, --check, --check=diagnose-first check for sorted input; do not sort
-m, --merge, merge already sorted files; do not sort
*/
typedef struct ArgvFlags {
  int ignoreCase, reverse, ignoreLeadingBlank, check, merge;
} Flags;

Flags globalFlags;

void init_global_flags() {
  globalFlags.check = 0;
  globalFlags.ignoreCase = 0;
  globalFlags.reverse = 0;
  globalFlags.ignoreLeadingBlank = 0;
  globalFlags.merge = 0;
}

void print_flags() {
  printf("Flags:\n");
  if(globalFlags.ignoreCase) printf("Ignore case\n");
  if(globalFlags.reverse) printf("Reverse\n");
  if(globalFlags.ignoreLeadingBlank) printf("Ignore leading blank\n");
  if(globalFlags.check) printf("Check\n");
  if(globalFlags.merge) printf("Merge\n");
}

typedef struct FileData {
  int lines_count, min_buffer;
  char *file_name;
  char **file_lines;
} FileData;

#endif