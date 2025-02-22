# Unix-like Shell

## How to Use

Run the following commands in the terminal:

```Unix
git clone https://github.com/keyaloding/shell.git
make myshell
./myshell
```

### Batch Files

To run commands from a file, rather than in interactive mode,
run `./myshell [batchFile]`.

### Multiple Commands

To run multiple commands with a single input, separate the commands with
semicolons (;). Example: `cd directory; ls; ps;`.

### Redirection

To redirect the output of a command to a file, run `[cmd] > [filename]`. If the
file already exists, an error will be printed and the command will be skipped.

### Built-in commands

The `cd`, `pwd`, and `exit` commands are built-in. All other commands are
executed by running `execvp()` in child process. Because `pwd` is built-in, its
output cannot be redirected to an output file.
