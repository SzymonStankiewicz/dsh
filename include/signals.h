#ifndef  __DSH_SIGNALS_H__
#define  __DSH_SIGNALS_H__
#define BLOCK 0
#define UNBLOCK 1
#define DISABLE_SIGCHLD sigchld_interrupt(BLOCK)
#define ENABLE_SIGCHLD sigchld_interrupt(UNBLOCK)

void init_signals_handling();
void sigchld_interrupt(int x);
void wait_for_foreground();
void add_pid(int pid, int background);
void clean_foreground();
void default_actions();
void print_background();

#endif //__DSH_SIGNALS_H__
