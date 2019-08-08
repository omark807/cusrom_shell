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

/**
 * Gets input from the user, executes it
 *
 * 1. Reads command from standard input
 * 2. Separates command string into a program and arguments
 * 3. Run parsed command
 */
void lsh_loop(void) {
  char* line;
  char** args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);


}
int main(int argc, char** argv) {

  // Load any config files, run command loop
  lsh_loop();

  // Shutdown/cleanup
  return EXIT_SUCCESS;
}
