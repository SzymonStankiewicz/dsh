#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

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

int run_builtin_command(command *com) {
	if(com == NULL || com->argv[0] == NULL) return 1;
	for(int i = 0; builtins_table[i].name != NULL; i++) {
		if(strcmp(builtins_table[i].name, com->argv[0]) == 0) {
			if(builtins_table[i].fun(com->argv) == -1) {
				write_error(3, "Builtin ", builtins_table[i].name ," error.\n");
			}
			return 1;
		}
	}
	return 0;
}

void run_command(command *com) {
	if(com == NULL || com->argv[0] == NULL) return;
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
		if(run_builtin_command(pickfirstcommand(ln))) return;
		com = pickfirstcommand(ln);
		run_command(com);
	}
}
