all:


clean:
	- find ./ -type f -name "*.out" -exec rm {} \;
	- find ./ -type f -executable -exec rm {} \;
