#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "builtins.h"
#include "io.h"
#include "signals.h"

#include "execute.h"

int isBackground;

void handle_execvp_error(char* prog_name) {
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

void handle_open_error(char* filename) {
	char* error_type = NULL;
	if (errno == EACCES) {
		error_type = "permission denied";
	}
	else if (errno == ENOENT) {
		error_type = "no such file or directory";
	}
	write_error(4, filename, ": ", error_type, "\n");
}

int run_builtin_command(command *com) {
	if(com == NULL || com->argv == NULL || com->argv[0] == NULL) return 1;
	int i;
	for(i = 0; builtins_table[i].name != NULL; i++) {
		if(strcmp(builtins_table[i].name, com->argv[0]) == 0) {
			if(builtins_table[i].fun(com->argv) == -1) {
				write_error(3, "Builtin ", builtins_table[i].name ," error.\n");
			}
			return 1;
		}
	}
	return 0;
}

void redirect_in(int* in) {
	if(!in) return;
	dup2(in[0], STDIN_FILENO);
	close(in[0]);
	close(in[1]);
}

void redirect_out(int *out) {
	if(!out) return;
	dup2(out[1], STDOUT_FILENO);
	close(out[0]);
	close(out[1]);
}

void handle_redirect(int fp, int dup2fileno, char* file) {
	if(fp == -1) {
		handle_open_error(file);
		exit(127);
	}
	dup2(fp, dup2fileno);
	close(fp);
}

void file_redirect(command *com) {
	redirection** redi = com->redirs;
	int flags;
	char *file;
	while(*redi) {
		flags = (*redi)->flags;
		file = (*redi)->filename;
		if(IS_RIN(flags)) {
			handle_redirect(open(file, O_RDONLY), STDIN_FILENO, file);
		}
		if(IS_ROUT(flags)) {
			handle_redirect(open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666), STDOUT_FILENO, file);
		}
		if(IS_RAPPEND(flags)) {
			handle_redirect(open(file, O_WRONLY | O_CREAT | O_APPEND, 0666), STDOUT_FILENO, file);
		}
		redi++;
	}
}

void run_command(command *com, int *read_pipe, int *write_pipe) {
	if(run_builtin_command(com)) return;
	pid_t pid = fork();

	if (pid == 0) {
		if (isBackground)
			setsid(); 
	
		default_actions();
		ENABLE_SIGCHLD;

		redirect_in(read_pipe);
		redirect_out(write_pipe);
		file_redirect(com);
		if(execvp(com->argv[0], com->argv) < 0) handle_execvp_error(com->argv[0]);
		exit(0);
	}
	else if(pid < 0) {
		exit(2);
	}
	else {
		add_pid(pid, isBackground);
	}
}

int* init_pipe() {
	int* pipes = malloc(2);
	pipe(pipes);
	return pipes;
}

void close_pipe(int* pipes) {
	if(!pipes) return;
	close(pipes[0]);
	close(pipes[1]);
	free(pipes);
}

void run_pipeseq(pipeline pipe) {
	clean_foreground();
	DISABLE_SIGCHLD;
	int coms = 0;
	command **com = pipe;
	int* pipes[2];
	pipes[0] = NULL;
	while(*com) {
		pipes[1] = NULL;
		if(*(com+1)) {
			pipes[1] = init_pipe();
		}
		run_command(*com, pipes[0], pipes[1]);
		close_pipe(pipes[0]);
		pipes[0] = pipes[1];
		com++;
		coms++;
	}
	close_pipe(pipes[0]);
	if(!isBackground) {
		wait_for_foreground();
	}
	ENABLE_SIGCHLD;
}

void execute(char* in) {
	line* ln;
	command *com;
	ln = parseline(in);
	isBackground = ln->flags;
	if(ln == NULL) {
		write_syntax_error();
	}
	else {
		pipeline* pipe = ln->pipelines;
		while(*pipe) {
			run_pipeseq(*pipe);
			pipe++;
		}
	}
}
