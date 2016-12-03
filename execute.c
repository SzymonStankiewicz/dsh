#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "builtins.h"
#include "io.h"

#include "execute.h"

void run_command(command *com) {
	pid_t pid = fork();

	if (pid == 0) {
		execvp(com->argv[0], com->argv);
		exit(0);
	}
	else if(pid < 0) {
		exit(2);
	}
	else {
		int returnStatus;    
		waitpid(pid, &returnStatus, 0);
	}
}

void execute(char* in) {
	line* ln;
	command *com;
	ln = parseline(in);
	if(ln == NULL) {
		write_syntax_error();
	}
	else {
		com = pickfirstcommand(ln);
		run_command(com);
	}
}
