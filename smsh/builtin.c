/* builtin.c
 * contains the switch and the functions for builtin commands
 */

#include <stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include <unistd.h>
#include	"smsh.h"
#include	"varlib.h"

int assign(char *);
int okname(char *);

int builtin_command(char **args, int *resultp)
/*
 * purpose: run a builtin command 
 * returns: 1 if args[0] is builtin, 0 if not
 * details: test args[0] against all known builtins.  Call functions
 */
{
	int rv = 0;

	if ( strcmp(args[0],"set") == 0 ){	     /* 'set' command? */
		VLlist();
		*resultp = 0;
		rv = 1;
	}
	else if ( strchr(args[0], '=') != NULL ){   /* assignment cmd */
		*resultp = assign(args[0]);
		if ( *resultp != -1 )		    /* x-y=123 not ok */
			rv = 1;
	}
	else if ( strcmp(args[0], "cd") == 0 ){   /* cd command */
		*resultp = chdir(args[1]);
		if ( *resultp != -1 )
			rv = 1;
	}
	else if ( strcmp(args[0], "read") == 0 ){   /* read command */
		char* varname = args[1];	

		char	*buf;
		int	bufspace = 0;
		int	pos = 0;
		int	c;
		while( ( c = getc(stdin)) != '\n' ) {
			if ( c == '\n' )
				break;
			if( pos+1 >= bufspace ){
				if ( bufspace == 0 )
					buf = emalloc(BUFSIZ);
				else
					buf = erealloc(buf,bufspace+BUFSIZ);
				bufspace += BUFSIZ;
			}

			buf[pos++] = c;
		}
		buf[pos] = '\0';

		VLstore(varname, buf);
	}
	else if ( strcmp(args[0], "exit") == 0 ){   /* exit command */
		int status = 1;
		if (args[1] != NULL){
			status = atoi(args[1]);
		}
		exit(status);
	}
	else if ( strcmp(args[0], "export") == 0 ){
		if ( args[1] != NULL && okname(args[1]) )
			*resultp = VLexport(args[1]);
		else
			*resultp = 1;
		rv = 1;
	}
	return rv;
}

int assign(char *str)
/*
 * purpose: execute name=val AND ensure that name is legal
 * returns: -1 for illegal lval, or result of VLstore 
 * warning: modifies the string, but retores it to normal
 */
{
	char	*cp;
	int	rv ;

	cp = strchr(str,'=');
	*cp = '\0';
	rv = ( okname(str) ? VLstore(str,cp+1) : -1 );
	*cp = '=';
	return rv;
}
int okname(char *str)
/*
 * purpose: determines if a string is a legal variable name
 * returns: 0 for no, 1 for yes
 */
{
	char	*cp;

	for(cp = str; *cp; cp++ ){
		if ( (isdigit(*cp) && cp==str) || !(isalnum(*cp) || *cp=='_' ))
			return 0;
	}
	return ( cp != str );	/* no empty strings, either */
}
