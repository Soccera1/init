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

void run(int runlevel) {
  char command[1024];
  snprintf(command, sizeof(command), "cd %s && PATH=\"$PATH:%s\" %s %d", INIT_DIRECTORY, INIT_DIRECTORY, SHELL, runlevel);
  system(command);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <runlevel>\n", argv[0]);
    return 1;
  }
  int runlevel = atoi(argv[1]);

  check_init();
  run(runlevel);
  return 0;
}
