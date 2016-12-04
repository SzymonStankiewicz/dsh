#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#include "signals.h"
#include "config.h"
#include "io.h"

sigset_t signal_wait;
sigset_t sigchld_block;
struct sigaction sigint_action[2];
struct sigaction sigchld_action[2];

int foreground_pids[MAX_PROCS];
int foreground_counter = 0, foreground_size = 0;

int background_pids[MAX_PROCS];
int background_stats[MAX_PROCS];
int background_counter = 0;

void print_background() {
	int i = 0;
	for(i = 0; i< background_counter; i++) {
		int pid = background_pids[i];
		int was_killed = WIFSIGNALED(background_stats[i]);
		int status = background_stats[i];
		if(was_killed) status = WTERMSIG(status);
		char* print_reason = was_killed ? "killed by signal " : "exited with status ";
		write_out("Background process ");
		write_out(itoa(pid));
		write_out_multi(5, " terminated. (", print_reason, " ", itoa(status), ")\n");
	}
	background_counter = 0;
}

void wait_for_foreground() {
	while(foreground_counter > 0) {
		sigsuspend(&signal_wait);
	}
}

void clean_foreground() {
	foreground_size = 0;
	foreground_counter = 0;
}

void add_pid(int pid, int background) {
	if(!background) {
		foreground_pids[foreground_size++] = pid;
		foreground_counter++;
	}
}

int is_foreground(int pid) {
	int i;
	for(i = 0; i< foreground_size; i++) {
		if(pid == foreground_pids[i])
			return 1;
	}
	return 0;
}

void sigchld_interrupt(int x) {
	if(x == BLOCK) {
		sigemptyset(&sigchld_block); 
		sigaddset(&sigchld_block, SIGCHLD); 
		sigprocmask(SIG_BLOCK, &sigchld_block, NULL); 
	}
	else {
		sigprocmask(SIG_UNBLOCK, &sigchld_block, NULL);
	}
}

void sigchld_handler(int sig) {
	pid_t child_pid;
	int child_status;

	while ((child_pid = waitpid(-1, &child_status, WNOHANG)) > 0) {
		if(is_foreground(child_pid))
			foreground_counter--;
		else {
			if(background_counter == MAX_PROCS) continue;
			background_pids[background_counter] = child_pid;
			background_stats[background_counter] = child_status;
			background_counter++;
		}
	}
}

void default_actions() {
	sigaction(SIGINT, &sigint_action[1], NULL);
	sigaction(SIGCHLD, &sigchld_action[1], NULL);
}

void init_signals_handling() {
	sigprocmask(SIG_BLOCK, NULL, &signal_wait);
	
	sigint_action[0].sa_handler = SIG_IGN;
	sigchld_action[0].sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sigchld_action[0].sa_handler = sigchld_handler;

	sigaction(SIGINT, &sigint_action[0], &sigint_action[1]); 
	sigfillset(&sigchld_action[0].sa_mask);
	sigaction(SIGCHLD, &sigchld_action[0], &sigchld_action[1]);
}
