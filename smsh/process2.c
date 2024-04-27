#include	<stdio.h>
#include	"smsh.h"
#include "stack.h"

/* process2.c   - version 2 - supports builtins
 * command processing layer
 * 
 * The process(char **arglist) function is called by the main loop
 * It sits in front of the execute() function.  This layer handles
 * two main classes of processing:
 * 	a) built-in functions (e.g. exit(), set, =, read, .. )
 * 	b) control structures (e.g. if, while, for)
 */

int is_control_command(char *);
int do_control_command(char **, struct StackFrame**);
int ok_to_execute(struct StackFrame**);
int builtin_command(char **, int *);

int process(char **args, struct StackFrame** stack)
/*
 * purpose: process user command
 * returns: result of processing command
 * details: if a built-in then call appropriate function, if not execute()
 *  errors: arise from subroutines, handled there
 */
{
	int		rv = 0;

	// printf("a0: %s, a1:%s, a2:%s\n", args[0], args[1], args[2]);

	if ( args[0] == NULL ){
		rv = 0;
	}
	else if ( is_control_command(args[0]) ){
		// printf("control command\n");

		if (stack_size(stack)){
			if (!(stack_top(stack)->if_state == ELSE_BLOCK && stack_top(stack)->if_result == SUCCESS)){
				rv = do_control_command(args, stack);
			} 
		}
		else {
			rv = do_control_command(args, stack);
		}	
	}
	else if ( ok_to_execute(stack) ){
		// printf("non-control command\n");
		if ( !builtin_command(args,&rv) )
			rv = execute(args);
	}
	return rv;
}

