#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#define TOK_ADD 5

#define MAXLEN 10

jmp_buf jumbuffer;
  
void do_line(char *);
void cmd_add(void);
int get_token(void);

int main(void)
{
	char line[MAXLEN];
	
	while (fgets(line, MAXLEN, stdin) != NULL)
		do_line(line);

	exit(0);
}

char * tok_ptr;


void do_line(char * ptr)
{
	int cmd;
	tok_ptr = ptr;

	while ((cmd = get_token()) > 0)
	{
		switch (cmd)
		case TOK_ADD:
			cmd_add();
			break;
	}
}

int get_token(void)
{


}
