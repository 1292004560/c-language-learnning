all: sds-test
sds-test: sds.c sds.h testhelp.h
	$(CC) -o sds-test sds-test.c sds.c -Wall -std=c99 -pedantic -O2 -DSDS_TEST_MAIN
	@echo ">>> Type ./sds-test to run the sds.c unit tests."


gdb:
	gdb ./sds-test

clean:
	rm -f sds-test