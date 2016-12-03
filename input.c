#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include "input.h"
#include "output.h"
#include "config.h"

static char out[MAX_LINE_LENGTH + 1];
static char line[MAX_LINE_LENGTH + 1];
int position = 0;
int last_read = 0;

//Read from stdin to buffer, returns number of read bytes. 
//Terminates process when error other than EINTR occur.
int errorless_read(char* buff, int len) {
	int size;
	while((size = read(STDIN_FILENO, buff, len)) == -1) {
		if(errno != EINTR) exit(1);
	}
	return size;
}


//Fill line with data from STDIN.
//Stops if:
// - read stopped at EOL or
// - line buffor is full or
// - reached EOF
// Returns number of read bytes.
int fill_line() {
	int size = 0, old_pos = position;
	do {
		position += size = errorless_read(line + position, MAX_LINE_LENGTH - position);
	} 
	while (size && position < MAX_LINE_LENGTH && line[position-1] != '\n');
	return position - old_pos;
}

//Move first _pos_ characters from line to out
void move(int pos) {
	last_read = 0;
	memcpy(out, line, pos+1);
	memmove(line, line+pos+1, MAX_LINE_LENGTH-pos);
	out[pos+1] = '\0';
	position -= (pos+1);
}


//Copy first line from line to out
int copy_line() {
	int i;
	for(i = last_read; i<position; i++) {
		if(line[i] == '\n') {
			move(i);
			return i;
		}
	}
	return -1;
}

void append_new_line() {
	line[position] = '\n';
	position++;
}

int returned_last_line = 0;
char* readline() {
	int skip = 0;
	do {
		skip = 0;
		if(returned_last_line) return NULL;
		while(copy_line() == -1) {
			if(position == MAX_LINE_LENGTH) {
				last_read = 0;
				position = 0;
				skip = 1;
			}
			int read = fill_line();
			if(read == 0) {
				returned_last_line = 1;
				append_new_line();
			}
		}
		if(skip) write_syntax_error();
	}
	while(skip);
	if(returned_last_line && out[0] == '\n') return NULL;
	return out;
}
