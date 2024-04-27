#pragma once

#define	YES	1
#define	NO	0

char	*next_cmd();
char	**splitline(char *);
void	freelist(char **);
void	*emalloc(int);
void	*erealloc(void *, int);
int	execute(char **);
void	fatal(char *, char *, int );

enum states   { WANT_THEN, THEN_BLOCK, ELSE_BLOCK };
enum results  { SUCCESS, FAIL };

struct StackFrame {
	int if_state; 
	int if_result; 
	int last_stat;
};

