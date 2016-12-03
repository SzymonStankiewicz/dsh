#include <stdio.h>

#include "io.h"
#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "execute.h"


int main(int argc, char *argv[])
{
	char *line;
	while(1) {
		write_prompt();
		line = readline();
		if(line == NULL) break;
		execute(line);
	}
	return 0;
}
