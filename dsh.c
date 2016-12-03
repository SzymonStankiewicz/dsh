#include <stdio.h>

#include "io.h"
#include "config.h"
#include "siparse.h"
#include "utils.h"


int
main(int argc, char *argv[])
{
	char* m;
	while(m = read_line()) {
		write_out(m);
	}
}
