#include <stdio.h>

#include "io.h"
#include "config.h"
#include "siparse.h"
#include "utils.h"


int
main(int argc, char *argv[])
{
	char *line;
	while(1) {
		write_prompt();
		line = readline();
		if(line == NULL) break;
		write_out(line);
	}
	return 0;
}
