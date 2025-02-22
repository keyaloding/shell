# -Werror flag prevents compiling if there are warnings
myshell: myshell.c
	gcc -Wall -Werror -o myshell myshell.c

clean:
	rm -f myshell *~

test: myshell.h myshell.c test_myshell.c
	gcc -Wall -Werror -o test myshell.c test_myshell.c -lcriterion