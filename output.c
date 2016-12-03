#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "output.h"
#include "config.h"

int write_to_file(int fd, char *str) {
  int len = strlen(str);
  int res = len;
  int written;
  while(len > 0) {
    written = write(fd, str, strlen(str));
    if (written < 0) return written;
    str += written;
    len -= written;
  }
  return res;
}

int write_out(char *str) {
  return write_to_file(STDOUT_FILENO, str);
}

int write_out_multi(int n, ...) {
	va_list ap;
	va_start(ap, n);
	int whole_write = 0;
	for(int i = 0; i<n; i++) {
		char* str = (char*) va_arg(ap, char*);
  	whole_write += write_out(str);
	}
	return whole_write;
}

int write_error(int n, ...) {
	va_list ap;
	va_start(ap, n);
	int whole_write = 0;
	for(int i = 0; i<n; i++) {
		char* str = (char*) va_arg(ap, char*);
  	whole_write += write_to_file(STDERR_FILENO, str);
	}
	return whole_write;
}

int write_syntax_error() {
  return write_error(2, SYNTAX_ERROR_STR, "\n");
}

void write_prompt() {
	struct stat stdout_stat;
	if (fstat(STDIN_FILENO, &stdout_stat) >= 0 && S_ISCHR(stdout_stat.st_mode)) {
		write_out(PROMPT_STR);
		fflush(stdout);
	}
}
