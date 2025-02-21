#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_ARGS 2000

void myPrint(char *msg) {
  write(STDOUT_FILENO, msg, strlen(msg));
}

void raise_error() {
  myPrint("An error has occurred\n");
}

enum redirect {
  NONE,
  BASIC,
  ADV
};

/* Splits the input string by `delimiter` and stores the resulting
 * substrings in `args`. */
void parse_input(char* input, char* args[], char* delimiter) {
  char *token;
  int i = 0;
  while ((token = strsep(&input, delimiter)) != NULL) {
    if (*token == '\0') continue;
    args[i++] = token;
    if (i >= MAX_ARGS - 1) break;
  }
  args[i] = NULL;
}

/* Returns true if file descriptor is successfully changed. */
bool basic_redirect(char* args[]) {
  int fd, i;
  char* filename;
  char* arr[MAX_ARGS];
  for (i = 0; args[i]; i++) {
    if (strstr(args[i], ">")) {
      if (!strcmp(args[i], ">")) {
        filename = args[i+1];
      } else {
        parse_input(args[i], arr, ">");
        filename = arr[1];
      }
      break;
    }
  }
  if (!filename) return false;
  fd = open(filename, O_RDONLY, 0644);
  if (fd >= 0) return false;
  fd = open(filename, O_WRONLY | O_CREAT, 0644);
  if (dup2(fd, STDOUT_FILENO) < 0) {
    close(fd);
    return false;
  }
  close(fd);
  return true;
}

/* Returns true if file descriptor is successfully changed. */
bool adv_redirect(char* args[]) {
  int fd;
  return true;
}

void run_pwd(char* args[]) {
  char dir[512];
  getcwd(dir, 512);
  myPrint(dir);
  myPrint("\n");
}

void run_cd(char* args[]) {
  char* path = args[1];
  if (!path) {
    path = getenv("HOME");
    if (!path) {
      raise_error();
    }
  }
  if (chdir(path) < 0) {
    raise_error();
  }
}

void run_cmd(char* args[]) {
  enum redirect type = NONE;
  for (int i = 0; args[i]; i++) {
    if (strstr(args[i], ">+")) {
      type = ADV;
      break;
    } else if (strstr(args[i], ">")) {
      type = BASIC;
      break;
    }
  }
  if (!strcmp(args[0], "pwd")) {
    if (args[1] && strstr(">", args[1])) {
      raise_error();
      return;
    }
    run_pwd(args);
  }
  else if (!strcmp(args[0], "cd")) run_cd(args);
  else if (!strcmp(args[0], "exit")) exit(0);
  else {
    pid_t pid = fork();
    if (pid < 0) {
      raise_error();
      return;
    } else if (pid == 0) {
      switch (type) {
        case BASIC:
          if (!basic_redirect(args)) {
            raise_error();
            return;
          }
          break;
          case ADV:
          if (!adv_redirect(args)) {
            raise_error();
            return;
          }
          break;
        default:
          break;
      };
      if (execvp(args[0], args) < 0) {
        raise_error();
      }
    } else {
      wait(NULL);
    }
  }
}

void run_multiple_cmds(char* input) {
  char* cmds[MAX_ARGS];
  parse_input(input, cmds, ";\n");
  for (int i = 0; cmds[i]; i++) {
    char* args[MAX_ARGS];
    parse_input(cmds[i], args, " \n\t");
    run_cmd(args);
  }
}

void run_batch_file(char* args[]) {
  int fd = open(args[1], O_RDONLY);
  char buffer[500000];
  if (fd < 0) {
    raise_error();
    exit(1);
  }
  read(fd, buffer, 500000);
  run_multiple_cmds(buffer);
  int retval = close(fd);
  if (retval < 0) raise_error();
}

int main(int argc, char *argv[]) {
  if (argv[1]) {
    run_batch_file(argv);
    return 0;
  }
  char cmd_buff[514];
  char *pinput;
  while (true) {
    myPrint("myshell> ");
    pinput = fgets(cmd_buff, 514, stdin);
    if (!pinput) exit(0);
    else if (!strcmp(pinput, "\n")) continue;
    run_multiple_cmds(pinput);
  }
  return 0;
}
