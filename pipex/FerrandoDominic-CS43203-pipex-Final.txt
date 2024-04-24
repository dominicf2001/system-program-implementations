#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int program_count = 0;
char** program_names;

void pipeAndFork(const int program_index) {
  if (program_index >= program_count - 1){
    char* args[] = {program_names[program_index], NULL};
    execvp(program_names[program_index], args);
    return;
  }

  int pipes[2];
  if (pipe(pipes) == -1){
    perror("Error opening pipe\n");
    exit(1);
  }

  pid_t pid;
  if ((pid = fork()) == -1){
    perror("Error when forking\n");
    exit(1);
  }
  if (pid != 0){ // PARENT
    close(pipes[1]);
    if (dup2(pipes[0], 0) == -1){
      perror("Error when duping\n");
      exit(1);
    }
    close(pipes[0]);
    
    pipeAndFork(program_index + 1);
  }
  else { // CHILD
    close(pipes[0]);
    if (dup2(pipes[1], 1) == -1){
      perror("Error when duping\n");
      exit(1);
    }
    close(pipes[1]);

    char* args[] = {program_names[program_index], NULL};
    execvp(program_names[program_index], args);
  }
}

int main(int argc, char* argv[]){
  program_count = argc - 1;
  program_names = argv + 1;

  if (program_count == 0){
    printf("Not enough arguments\n"); 
    return 1;
  }
    
  pipeAndFork(0);

  return 0;
}
