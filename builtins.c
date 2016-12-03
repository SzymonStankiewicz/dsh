#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>

#include "config.h"
#include "builtins.h"
#include "io.h"

int builtin_echo(char*[]);
int undefined(char *[]);
int builtin_exit(char* argv[]);
int builtin_cd(char* argv[]);
int builtin_ls(char* argv[]);
int builtin_kill(char* argv[]);

builtin_pair builtins_table[]={
	{"exit",	&builtin_exit},
	{"lecho",	&builtin_echo},
	{"lcd",		&builtin_cd},
	{"lkill",	&builtin_kill},
	{"lls",		&builtin_ls},
	{NULL,NULL}
};

int builtin_echo( char * argv[]) {
	int i =1;
	if (argv[i]) write_out(argv[i++]);
	while  (argv[i])
		write_out_multi(2, argv[i++], " ");
	write_out("\n");
	fflush(stdout);
	return 0;
}

int builtin_kill(char* argv[])
{
    if(argv[1] == NULL) return -1;
		
		int defined_id = argv[1][0] == '-';
    int signal_id = defined_id ? atoi(argv[1] + 1) : SIGTERM;
    int pid = atoi(defined_id ? argv[2] : argv[1]);
    
    return kill(pid, signal_id);
}

int builtin_exit(char* argv[]) {
	exit(0);
}

int builtin_cd(char* argv[])
{
	if(argv[2]) return -1;
	char* path = argv[1] ? argv[1] : getenv("HOME");
	return chdir(path);
}

int builtin_ls(char* argv[]) {
	char _path[MAX_LINE_LENGTH];
	if(argv[1] == NULL && getcwd(_path, MAX_LINE_LENGTH) == NULL) return -1;
	char* path = argv[1] == NULL ? _path : argv[1];
	DIR* files = opendir(path);
	if (files == NULL) {
		return -1;
	}
	struct dirent* entry;
	while ((entry = readdir(files)) != NULL) {
		if(entry->d_name[0] != '.') {
			write_out_multi(2, entry->d_name, "\n");
		}
	}
	closedir(files);

	return 0;
}

int undefined(char * argv[])
{
	fprintf(stderr, "Command %s undefined.\n", argv[0]);
	return BUILTIN_ERROR;
}
