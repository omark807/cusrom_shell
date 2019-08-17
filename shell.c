/**************************************************************************
  Custom C shell, working on making this a viable daily driver

  Author: Omar Khan

 **************************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 1024


/**
 * Function declarations for buit-in commands
 */
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

/**
 *
 * List of builtin commands, followed by their corresponding functions.
 *
 **/
char *builtin_ops[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};

int shell_num_ops() {
  return sizeof(builtin_ops) / sizeof(char *);
}

/*
 *  The shell's built-in functionalities and implementations.
*/

/**
 * Change directories (cd)
 * @param args List of args.  args[0] is "cd".  args[1] is the directory.
 * @return Always returns 1 to continue executing.
 */
int shell_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("shell");
    }
  }
  return 1;
}


/**
 * Help manu.
 * @param args List of args
 * @return Always returns 1 to continue executing.
 */
int shell_help(char **args)
{
  int i;
  printf("My shell\n");
  printf("Type program names and arguments, and press enter.\n");
  printf("The following commands are built in:\n");

  for (i = 0; i < shell_num_ops(); i++) {
    printf("  %s\n", builtin_ops[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int shell_exit(char **args)
{
  return 0;
}

/**
 * Launch a program and wait for it to terminate.
 *
 * @param args Null terminated list of arguments
 * @return Returns 1 to continue execution.
 */
int shell_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("shell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("shell");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
 * Execute shell built-in or launch program.
 * @param args Null terminated list of arguments.
 * @return 1 if the shell should continue running, 0 if it should terminate
 */
int shell_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < shell_num_ops(); i++) {
    if (strcmp(args[0], builtin_ops[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return shell_launch(args);
}

/**
 *  Read a line of input from stdin.
 *  @return The line from stdin.
 */
char *shell_read_line(void)
{
  int bufsize = BUFFER_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "shell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += BUFFER_SIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define shell_TOK_BUFSIZE 64
#define shell_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **shell_split_line(char *line)
{
  int bufsize = shell_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "shell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, shell_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += shell_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, shell_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void shell_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = shell_read_line();
    args = shell_split_line(line);
    status = shell_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  shell_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
