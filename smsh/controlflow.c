/* controlflow.c
 *
 * "if" processing is done with two state variables
 *    if_state and if_result
 */
#include	<stdio.h>
#include <stdlib.h>
#include	"smsh.h"
#include	"varlib.h"
#include "stack.h"

int	process(char **, struct StackFrame**);

int	syn_err(char *, struct StackFrame**);

int ok_to_execute(struct StackFrame** stack)
/*
 * purpose: determine the shell should execute a command
 * returns: 1 for yes, 0 for no
 * details: if in THEN_BLOCK and if_result was SUCCESS then yes
 *          if in THEN_BLOCK and if_result was FAIL    then no
 *          if in WANT_THEN  then syntax error (sh is different)
 */
{
	int	rv = 1;		/* default is positive */

	//printf("checking if_state: %d\n", stack_frame.if_state);
	if ( stack_size(stack) && stack_top(stack)->if_state == WANT_THEN ){
		syn_err("then expected", stack);
		rv = 0;
	}
	else if ( stack_size(stack) && stack_top(stack)->if_state == THEN_BLOCK && stack_top(stack)->if_result == SUCCESS ){
		// printf("successful if result. If state is then_block\n");
		rv = 1;
	}
	else if ( stack_size(stack) && stack_top(stack)->if_state == THEN_BLOCK && stack_top(stack)->if_result == FAIL ){
		// printf("failed if result. If state is then_block\n");
		rv = 0;
	}
	else if ( stack_size(stack) && stack_top(stack)->if_state == ELSE_BLOCK && stack_top(stack)->if_result == FAIL ){
		// printf("failed if result. If state is then_block\n");
		rv = 1;
	}
	else if ( stack_size(stack) && stack_top(stack)->if_state == ELSE_BLOCK && stack_top(stack)->if_result == SUCCESS){
		// printf("failed if result. If state is then_block\n");
		rv = 0;
	}

	// printf("rv: %d\n", rv);
	return rv;
}

int is_control_command(char *s)
/*
 * purpose: boolean to report if the command is a shell control command
 * returns: 0 or 1
 */
{
    return (strcmp(s,"if")==0 || 
						strcmp(s,"then")==0 || 
						strcmp(s,"fi")==0 ||
						strcmp(s,"else")==0);

}


int do_control_command(char **args, struct StackFrame** stack)
/*
 * purpose: Process "if", "then", "fi" - change state or detect error
 * returns: 0 if ok, -1 for syntax error
 *   notes: I would have put returns all over the place, Barry says "no"
 */
{
	char	*cmd = args[0];
	int	rv = -1;

	if( strcmp(cmd,"if")==0 ){
		// printf("in if\n");		
		// init new stack frame
		struct StackFrame* stack_frame = malloc(sizeof(struct StackFrame));
		*stack_frame = (struct StackFrame){
			.if_state = WANT_THEN,
			.if_result = SUCCESS,
			.last_stat = 0
		};

		stack_frame->last_stat = process(args+1, stack);
		stack_frame->if_result = (stack_frame->last_stat == 0 ? SUCCESS : FAIL );
		stack_frame->if_state = WANT_THEN;
		stack_push(stack, stack_frame);
		rv = 0;
	}
	else if ( strcmp(cmd,"then")==0 ){
		// printf("in then\n");
		if ( stack_top(stack)->if_state != WANT_THEN )
			rv = syn_err("then unexpected", stack);
		else {
			stack_top(stack)->if_state = THEN_BLOCK;
			rv = 0;
		}
	}
	else if ( strcmp(cmd,"else")==0 ){
		// printf("in else\n");
		if ( stack_top(stack)->if_state != THEN_BLOCK )
			rv = syn_err("else unexpected", stack);
		else {
			stack_top(stack)->if_state = ELSE_BLOCK;
			rv = 0;
		}
	}
	else if ( strcmp(cmd,"fi")==0 ){
		// printf("in fi\n");
		if ( stack_top(stack)->if_state != THEN_BLOCK && stack_top(stack)->if_state != ELSE_BLOCK)
			rv = syn_err("fi unexpected", stack);
		else {
			struct StackFrame* stack_frame = stack_pop(stack);
			free(stack_frame);
			rv = 0;
		}
	}
	else 
		fatal("internal error processing:", cmd, 2);
	return rv;
}

int syn_err(char *msg, struct StackFrame** stack)
/* purpose: handles syntax errors in control structures
 * details: resets state to NEUTRAL
 * returns: -1 in interactive mode. Should call fatal in scripts
 */
{
	if (stack_size(stack)){
		struct StackFrame* stack_frame = stack_pop(stack);
		free(stack_frame);
	}
	fprintf(stderr,"syntax error: %s\n", msg);
	return -1;
}
