#include "io.h"
#include "execute.h"


int main(int argc, char *argv[])
{
  init_signals_handling();
	char *line;
	while(1) {
		write_prompt();
		line = readline();
		if(!line) break;
		execute(line);
	}
	return 0;
}
