#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_ARGS 400

void myPrint(char *msg) {
  write(STDOUT_FILENO, msg, strlen(msg));
}

/* Prints error message but does not kill the process. */
void raise_error() {
  char* error_message = "An error has occurred\n";
  myPrint(error_message);
}

void basic_redirect(char* msg, char* filename) {
  pid_t forkret = fork();
  if (forkret < 0) raise_error();
  else if (forkret == 0) {
    int fd = open(filename, O_RDONLY);
    if (fd > 0) {
      raise_error();
      return;
    }
    fd = open(filename, O_CREAT | O_WRONLY);
    if (fd < 0) {
      raise_error();
    }
    ssize_t num_bytes = write(fd, msg, strlen(msg));
    if (num_bytes < 0) {
      raise_error();
    }
    close(fd);
  } else {
    wait(NULL);
  }
}

void adv_redirect(char* msg, char* filename) {
  int fd;
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    basic_redirect(msg, filename);
    return;
  }

}

enum redirect {
  NONE,
  BASIC,
  ADV
};
typedef enum redirect redirect;

void run_pwd(char* args[], redirect type) {
  char dir[512];
  getcwd(dir, 512);
  switch (type) {
    case NONE:
      break;
    case BASIC:

      break;
    case ADV:
      break;
  };
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
  if (!strcmp(args[0], "pwd")) {
    run_pwd(args, NONE);
  } else if (!strcmp(args[0], "cd")) {
    run_cd(args);
  } else if (!strcmp(args[0], "exit")) {
    exit(0);
  } else {
    pid_t pid = fork();
    if (pid < 0) {
      raise_error();
    } else if (pid == 0) {
      if (execvp(args[0], args) < 0) {
        raise_error();
      }
    } else {
      wait(NULL);
    }
  }
}

/* Converts the input string to an array of arguments. */
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
  char buffer[1024];
  if (fd < 0) {
    raise_error();
    exit(1);
  }
  read(fd, buffer, 1024);
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
