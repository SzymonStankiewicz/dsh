#ifndef  __DSH_OUT_H__
#define  __DSH_OUT_H__

int write_file(int fd, char *str);
int write_out(char *str);
int write_error(char *str);
int write_syntax_error();

#endif //__DSH_OUT_H__
