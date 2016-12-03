#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "builtins.h"
#include "io.h"

#include "execute.h"

void validate_error(char* prog_name) {
	char* error_type = NULL;
	if (errno == EACCES || errno == EPERM) {
		error_type = "permission denied";
	}
	else if (errno == ENOENT) {
		error_type = "no such file or directory";
	}
	else {
		error_type = "exec error";
	}
	write_error(4, prog_name, ": ", error_type, "\n");
	exit(127);
}

void run_command(command *com) {
	pid_t pid = fork();

	if (pid == 0) {
		if(execvp(com->argv[0], com->argv) < 0) validate_error(com->argv[0]);
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
