# -Werror flag prevents compiling if there are warnings
myshell: myshell.c
	gcc -Wall -Werror -o myshell myshell.c

clean:
	rm -f myshell *~
