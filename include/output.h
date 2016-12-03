#ifndef  __DSH_OUT_H__
#define  __DSH_OUT_H__

int write_file(int fd, char *str);
int write_out(char *str);
int write_out_multi(int n, ...);
int write_error(int n, ...);
int write_syntax_error();
void write_prompt();

#endif //__DSH_OUT_H__
