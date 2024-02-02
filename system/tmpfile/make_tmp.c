#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
void make_temp(char * template);

int main(void)
{
	char      good_template[] = "/tmp/dirXXXXXXXXX";
	char      *bad_template = "/tmp/dirXXXXXXXXX";

	printf("trying to create first temp file...\n");
	make_temp(good_template);

	printf("trying to create second temp file...\n");
	make_temp(bad_template);
	exit(0);
}

void make_temp(char * template)
{
	int         fd;
	struct stat sbuf;

	if ((fd = mkstemp(template)) < 0)
		printf("can't create temp file");

	printf("temp name = %s \n", template);

	close(fd);

	if (stat(template, &sbuf) < 0){
		if ( errno == ENOENT)
			printf("file doesn't exit\n");
		else
			printf("stat failed");

	}else {

		printf("file exit\n");
		unlink(template);
	}
	
}
