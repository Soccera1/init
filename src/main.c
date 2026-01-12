#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include "config.h"

void check_init() {

  const char* init_directory;
  init_directory=INIT_DIRECTORY;
  struct stat sb;

  if (stat(init_directory, &sb) == 0 && S_ISDIR(sb.st_mode)) {
    printf("Init directory found! Continuing.\n");
  } else {
    printf ("Init directory not found!\n");
    exit(1);
  }

}

int has_char_before_dot(const char *filename, char c) {
  int found_c = 0;
  for (int i = 0; filename[i] != '\0'; i++) {
    if (filename[i] == '.') return found_c;
    if (filename[i] == c) {
      found_c = 1;
    }
  }
  return 0;
}

void parse_names(char runlevel) {
  DIR *d;
  struct dirent *dir;
  d = opendir(INIT_DIRECTORY);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      // Use 8 if DT_REG is not defined (standard on most Linux systems)
      #ifndef DT_REG
      #define DT_REG 8
      #endif
      #ifndef DT_LNK
      #define DT_LNK 10
      #endif
      if (dir->d_type == DT_REG || dir->d_type == DT_LNK) {
        if (has_char_before_dot(dir->d_name, runlevel)) {
          printf("File: %s, Number: %c\n", dir->d_name, runlevel);
        }
      }
    }
    closedir(d);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <runlevel>\n", argv[0]);
    return 1;
  }
  char runlevel = argv[1][0];

  check_init();
  parse_names(runlevel);
  return 0;
}
