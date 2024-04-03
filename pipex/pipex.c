#include <stdio.h>
#include <unistd.h>

int program_count = 0;
char** program_names;

void pipeAndFork(const int program_index) {
  if (program_index >= program_count - 1){
    return;
  }

  int pipes[2];
  if (pipe(pipes) == -1){
    perror("Error opening pipe\n");
    return;
  }

  pid_t pid;
  if ((pid = fork()) == -1){
    perror("Error when forking\n");
    return;
  }
  if (pid != 0){
    printf("In parent: %s\n", program_names[program_index]);
    close(pipes[1]);
    
    // do some parent stuff
  }
  else {
    printf("In child: %s\n", program_names[program_index + 1]);
    close(pipes[0]);

    // do some child stuff

    pipeAndFork(program_index + 1);
  }
}

int main(int argc, char* argv[]){
  program_count = argc - 1;
  program_names = argv + 1;

  pipeAndFork(0);

  return 0;
}
