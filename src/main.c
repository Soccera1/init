#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
  char runlevel_str[12];
  snprintf(runlevel_str, sizeof(runlevel_str), "%d", runlevel);

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    if (chdir(INIT_DIRECTORY) != 0) {
      perror("chdir");
      _exit(1);
    }

    const char *old_path = getenv("PATH");
    char *new_path = NULL;
    if (old_path) {
      size_t len = strlen(old_path) + strlen(INIT_DIRECTORY) + 2;
      new_path = (char *)malloc(len);
      if (new_path) {
        snprintf(new_path, len, "%s:%s", old_path, INIT_DIRECTORY);
      }
    } else {
      size_t len = strlen(INIT_DIRECTORY) + 1;
      new_path = (char *)malloc(len);
      if (new_path) {
        snprintf(new_path, len, "%s", INIT_DIRECTORY);
      }
    }

    if (new_path) {
      setenv("PATH", new_path, 1);
      free(new_path);
    }

    execl(SHELL, SHELL, runlevel_str, (char *)NULL);
    perror("execl");
    _exit(1);
  } else {
    // Parent process
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
      exit(1);
    }
  }
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
