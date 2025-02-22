#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_ARGS 2000
#define BUFFER_SIZE 16384

/* Indicates what redirection type (if any) is being used. */
enum redirect {
  NONE,
  BASIC,
  ADV
};
typedef enum redirect redirect;

/* Prints the input message to stdout. */
void myPrint(char* msg);

/* Raises an error but does not exit the program. */
void raise_error();

/* Splits the input string by `delimiter` and stores the resulting
 * substrings in `args`. */
void parse_input(char* input, char* args[], char* delimiter);

/* Returns true if file descriptor is successfully changed. Returns false if
 * there is an error. */
bool basic_redirect(char* args[]);

/* */
bool adv_redirect(char* args[]);

/* Prints the current working directory. The output cannot be redirected
 * to a file. */
void run_pwd(char* args[]);

/* Changes the current working directory. */
void run_cd(char* args[]);

/* */
void run_cmd(char* args[]);

/* */
void run_multiple_cmds(char* input);

/* Runs commands from the specified file, rather than in interactive mode. */
void run_batch_file(char* args[]);
