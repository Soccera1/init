#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "config.h"

#define MAX_SCRIPTS 1000
#define MAX_NAME 256
#define MAX_DEPS 200

typedef struct {
    char generic_name[MAX_NAME];
    char full_name[MAX_NAME];
    char dependencies[MAX_DEPS][MAX_NAME];
    int num_dependencies;
    int in_degree;
    int processed;
} Script;

static Script scripts[MAX_SCRIPTS];
static int num_scripts = 0;

void check_init() {
  const char* init_directory = INIT_DIRECTORY;
  struct stat sb;

  if (stat(init_directory, &sb) == 0 && S_ISDIR(sb.st_mode)) {
    printf("Init directory found! Continuing.\n");
  } else {
    printf("Init directory not found!\n");
    exit(1);
  }
}

static int has_char_before_dot(const char *filename, char c) {
    int found_c = 0;
    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '.') return found_c;
        if (filename[i] == c) found_c = 1;
    }
    return 0;
}

static void get_generic_name(const char *filename, char *dest) {
    const char *dot = strchr(filename, '.');
    if (dot) {
        strncpy(dest, dot + 1, MAX_NAME - 1);
    } else {
        strncpy(dest, filename, MAX_NAME - 1);
    }
    dest[MAX_NAME - 1] = '\0';
}

static int find_script_idx(const char *generic_name) {
    for (int i = 0; i < num_scripts; i++) {
        if (strncmp(scripts[i].generic_name, generic_name, MAX_NAME) == 0) {
            return i;
        }
    }
    return -1;
}

void solve_dependencies(char runlevel) {
    DIR *d;
    struct dirent *dir;
    num_scripts = 0;

    d = opendir(INIT_DIRECTORY);
    if (!d) {
        perror("opendir");
        return;
    }

    while ((dir = readdir(d)) != NULL) {
        #ifndef DT_REG
        #define DT_REG 8
        #endif
        #ifndef DT_LNK
        #define DT_LNK 10
        #endif
        if ((dir->d_type == DT_REG || dir->d_type == DT_LNK) && has_char_before_dot(dir->d_name, runlevel)) {
            if (num_scripts >= MAX_SCRIPTS) break;

            Script *s = &scripts[num_scripts];
            strncpy(s->full_name, dir->d_name, MAX_NAME - 1);
            s->full_name[MAX_NAME - 1] = '\0';
            get_generic_name(dir->d_name, s->generic_name);
            s->num_dependencies = 0;
            s->in_degree = 0;
            s->processed = 0;

            char path[512];
            snprintf(path, sizeof(path), "%s/%s", INIT_DIRECTORY, dir->d_name);
            FILE *f = fopen(path, "r");
            if (f) {
                char line[1024];
                if (fgets(line, sizeof(line), f)) {
                    char *start = line;
                    if (strncmp(line, DEPENDENCY_PREFIX, strlen(DEPENDENCY_PREFIX)) == 0) {
                        start += strlen(DEPENDENCY_PREFIX);
                    }
                    char *token = strtok(start, " \t\n\r");
                    while (token && s->num_dependencies < MAX_DEPS) {
                        strncpy(s->dependencies[s->num_dependencies], token, MAX_NAME - 1);
                        s->dependencies[s->num_dependencies][MAX_NAME - 1] = '\0';
                        s->num_dependencies++;
                        token = strtok(NULL, " \t\n\r");
                    }
                }
                fclose(f);
            }
            num_scripts++;
        }
    }
    closedir(d);

    for (int i = 0; i < num_scripts; i++) {
        for (int j = 0; j < scripts[i].num_dependencies; j++) {
            int dep_idx = find_script_idx(scripts[i].dependencies[j]);
            if (dep_idx != -1) {
                scripts[i].in_degree++;
            }
        }
    }

    char out_path[512];
    snprintf(out_path, sizeof(out_path), "%s/%c", INIT_DIRECTORY, runlevel);
    FILE *out = fopen(out_path, "w");
    if (!out) {
        perror("fopen output");
        return;
    }

    int resolved = 0;
    while (resolved < num_scripts) {
        int found = 0;
        for (int i = 0; i < num_scripts; i++) {
            if (!scripts[i].processed && scripts[i].in_degree == 0) {
                fprintf(out, "%s\n", scripts[i].generic_name);
                scripts[i].processed = 1;
                found = 1;
                resolved++;

                for (int j = 0; j < num_scripts; j++) {
                    if (!scripts[j].processed) {
                        for (int k = 0; k < scripts[j].num_dependencies; k++) {
                            if (strcmp(scripts[j].dependencies[k], scripts[i].generic_name) == 0) {
                                scripts[j].in_degree--;
                            }
                        }
                    }
                }
                break; 
            }
        }
        if (!found) {
            fprintf(stderr, "Cycle detected or missing dependency in runlevel %c\n", runlevel);
            break;
        }
    }
    fclose(out);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <runlevel>\n", argv[0]);
        return 1;
    }
    char runlevel = argv[1][0];

    check_init();
    solve_dependencies(runlevel);
    return 0;
}
