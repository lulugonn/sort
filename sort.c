#include "stdio.h"
#include "stdlib.h"
#include <sys/stat.h> 
#include "string.h"
#include "sorting.h"
#include "types.h"
#include <ctype.h>

#include <unistd.h>


int files_count = 0;
FileData *files;

void read_flags(int argc, char *argv[]) {
  int opt;
  init_global_flags();
  
  while ((opt = getopt(argc, argv, "frbcm")) != -1) {
    switch (opt) {
      case 'f':
        globalFlags.ignoreCase = 1;
        break;
      case 'r':
        globalFlags.reverse = 1;
        break;
      case 'b':
        globalFlags.ignoreLeadingBlank = 1;
        break;
      case 'c':
        globalFlags.check = 1;
        break;
      case 'm':
        globalFlags.merge = 1;
        break;
      }
  }
}

int file_exist(char *file_name) {
  struct stat buffer;
  if(stat(file_name, &buffer) != 0) {
    printf("%s não existe!\n", file_name);
    return 0;
  }
  return 1;
}

char *ltrim(char *s) {
  int len = strlen(s), spaces_count = 0;
  
  for(int i = 0; i < len; i++) {
    if(!isspace(s[i])) break;
    spaces_count++;
  }

  char * new_str = calloc(len-spaces_count, sizeof(char));

  for(int i = spaces_count, j = 0; i < len; i++, j++) {
    new_str[j] = s[i];
  }

  free(s);

  return new_str;
}

void read_file(char *file_name, int file_id) {
  FILE *file;
  char *current_line = NULL;
  size_t len = 0;
  
  file = fopen(file_name, "r");
  int lines_count = 0;
  while (getline(&current_line, &len, file) != -1) {
    lines_count++;
  }
  fclose(file);

  files[file_id].file_name = file_name;
  files[file_id].lines_count = lines_count;
  if(len > files[file_id].min_buffer) files[file_id].min_buffer = len;

  files[file_id].file_lines = malloc(lines_count * sizeof(char*));
  for(int i = 0; i < lines_count; i++) {
    files[file_id].file_lines[i] = calloc(len, sizeof(char));
  }

  file = fopen(file_name, "r");
  int row_id = 0;
  while (getline(&current_line, &len, file) != -1) {
    if(globalFlags.ignoreLeadingBlank) current_line = ltrim(current_line);

    int length = strlen(current_line);
    if(current_line[length - 1] == '\n') length--;

    for(int i = 0; i < length; i++) {
      files[file_id].file_lines[row_id][i] = current_line[i];
    }
    row_id++;
  }
  fclose(file);

}

void read_files(int argc, char const *argv[]) {
  
  for(int i = 1; i < argc; i++) {
    char *file_name = argv[i];
    if(file_name[0] == '-') continue;
    if(!file_exist(file_name)) exit(1);
    files_count++;
  }

  if(files_count == 0) {
    printf("É necessário informar um arquivo como parâmetro.\n");
    exit(1);
  }

  files = malloc(files_count * sizeof(FileData));

  int file_id = 0;

  for(int i = 1; i < argc; i++) {
    char *file_name = argv[i];
    if(file_name[0] == '-') continue;
    if(!file_exist(file_name)) exit(1);

    read_file(file_name, file_id);
    file_id++;
  }
}

void sort_files() {
  for(int i = 0; i < files_count; i++) {
    sort_lines(files[i].file_lines, files[i].lines_count, files[i].min_buffer);
  }
}

void check_input() {
  if(files_count > 1) {
    printf("Mais de um arquivo não é permitido usando a flag -c.\n");
    exit(1);
  }

  for(int i = 1; i < files[0].lines_count; i++) {
    char *prev = files[0].file_lines[i-1];
    char *current = files[0].file_lines[i];

    if(compare_string(prev, current) > 0) {
      printf("O index %d (%s) é maior que o index %d (%s)\n", i-1, prev, i, current);
      exit(1);
    }

  }

}

void print_result(FileData res) {
  if(globalFlags.reverse) {
    for(int i = res.lines_count - 1; i >= 0; i--) {
      printf("%s\n", res.file_lines[i]);
    }
  } else {
    for(int i = 0; i < res.lines_count; i++) {
      printf("%s\n", res.file_lines[i]);
    }
  }
}

int main(int argc, char const *argv[]) {
  FileData res;
  read_flags(argc, argv);
  read_files(argc, argv);

  if(globalFlags.check) {
    check_input();
    exit(0);
  }

  if(globalFlags.merge) {
    res = merge_sorted_files(files, files_count);
    print_result(res);
    return 0;
  }

  sort_files();
  res = merge_sorted_files(files, files_count);
  print_result(res);

  return 0;
}
