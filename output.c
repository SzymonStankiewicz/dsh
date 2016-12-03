#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

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

int write_error(char *str) {
  return write_to_file(STDERR_FILENO, str);
}

int write_syntax_error() {
  return write_error(SYNTAX_ERROR_STR) + write_error("\n");
}

void write_prompt() {
	struct stat stdout_stat;
	if (fstat(STDIN_FILENO, &stdout_stat) >= 0 && S_ISCHR(stdout_stat.st_mode)) {
		write_out(PROMPT_STR);
		fflush(stdout);
	}
}
