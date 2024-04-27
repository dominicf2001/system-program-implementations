#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	"smsh.h"
#include	"varlib.h"
#include "stack.h"

/**
 **	small-shell version 4
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 **/

#define	DFL_PROMPT	"> "

int main(int argc, char** argv)
{
	char	*cmdline, *prompt, **arglist;
	int	result, process(char **, struct StackFrame**);
	void	setup();

	prompt = DFL_PROMPT ;
	setup();

	FILE* cmd_src = stdin;
	if (argc > 1){
			cmd_src = fopen(argv[1], "r");	
	}

	struct StackFrame** stack = emalloc(MAX_STACK_SIZE * sizeof(struct StackFrame*));
	stack[0] = 0;

	while ( (cmdline = next_cmd(prompt, cmd_src)) != NULL ){
		if ( (arglist = splitline(cmdline)) != NULL  ){
			result = process(arglist, stack);
			freelist(arglist);
		}
		free(cmdline);
	}
	free(stack);
	return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
	extern char **environ;

	VLenviron2table(environ);
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}
