#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_ARGS 200

/* Prints error message but does not kill the process. */
void raise_error() {
  char* error_message = "An error has occurred\n";
  write(STDOUT_FILENO, error_message, strlen(error_message));
}

void myPrint(char *msg) {
  write(STDOUT_FILENO, msg, strlen(msg));
}

void basic_redirect(char* msg, char* filename) {
  int fd;
  fd = open(filename, O_RDONLY);
  if (fd > 0) {
    printf("Error 1\n");
    raise_error();
  }
  fd = open(filename, O_CREAT | O_WRONLY);
  if (fd < 0) {
    printf("Error 2\n");
    raise_error();
  }
  ssize_t num_bytes = write(fd, msg, strlen(msg));
  if (num_bytes < 0) {
    printf("Error 3\n");
    raise_error();
  }
  close(fd);
}

void adv_redirect(char* msg, char* filename) {
  int fd;
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    basic_redirect(msg, filename);
    return;
  }

}

void run_pwd(char* args[]) {
  char dir[512];
  getcwd(dir, 512);
  for (int i = 0; args[i]; i++) {
    if (!strcmp(args[i], ">")) {
      printf("Basic redirect\n");
      basic_redirect(dir, args[i+1]);
      return;
    } else if (!strcmp(args[i], ">+")) {
      adv_redirect(dir, args[i+1]);
      return;
    }
  }
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

/* Runs all commands other than exit, cd, and pwd. */
void run_cmd(char* args[]) {
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

/* Converts the input string to an array of arguments. */
void parse_input(char* input, char* args[]) {
  char *token;
  int i = 0;
  while ((token = strsep(&input, " \n\t")) != NULL) {
    if (*token == '\0') continue;
    args[i++] = token;
    if (i >= MAX_ARGS - 1) break;
  }
  args[i] = NULL;
  // i = 0;
  // while (args[i]) {
  //   printf("arg[%d]: %s\n", i, args[i]);
  //   i++;
  // }
}

// void parse_input2(char* input, char* args[]) {
//   char* token;
//   int i = 0;
//   while ((token = strsep(&input, " ")) != NULL) {
//     if (*token == '\0') {
//       continue;
//     }
//     if (strcmp(token, ";") == 0 || strcmp(token, ">") == 0) {
//       args[i++] = token;
//     } else {
//       char *subtoken;
//       while ((subtoken = strsep(&token, " ;>")) != NULL) {
//         if (*subtoken == '\0') {
//           continue;
//         }
//         args[i++] = subtoken;
//       }
//     }
//     if (i >= MAX_ARGS - 1) {
//       break;
//     }
//   }
//   args[i] = NULL;
//   i = 0;
//   while (args[i]) {
//     printf("arg[%d]: %s\n", i, args[i]);
//     i++;
//   }
// }

void run_multiple_cmds(char* argv) {

}

int main(int argc, char *argv[]) {
  char cmd_buff[514];
  char *pinput, *args[MAX_ARGS];
  while (true) {
    myPrint("myshell> ");
    pinput = fgets(cmd_buff, 514, stdin);
    if (!pinput) {
      exit(0);
    }
    parse_input(cmd_buff, args);
    if (!strcmp(args[0], "pwd")) {
      run_pwd(args);
    } else if (!strcmp(args[0], "cd")) {
      run_cd(args);
    } else if (!strcmp(args[0], "exit")) {
      exit(0);
    } else {
      run_cmd(args);
    }
  }
  return 0;
}
