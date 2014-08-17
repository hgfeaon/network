#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <termio.h>

#include <sys/time.h>
#include <sys/types.h>

#define BUF_SIZE 256



void setup_terminal(struct termios* old_args);

void reset_terminal(struct termios* old_args); 

int main() {
	struct termios old_args;

	setup_terminal(&old_args);

	fd_set rfds;
	int sres = 0;

	char buffer[BUF_SIZE];
	int nread = 0;

	for (;;) {
		/* reset read file descriptor set */
		FD_ZERO(&rfds);
		/* add standard input file descriptor to the set */
		FD_SET(0, &rfds);
		
		/* do blocking select */
		printf("\nselect begin\n");
		sres = select(0 + 1, &rfds, NULL, NULL, NULL);
		printf("\nselect return\n");

		if (sres < 0) {
			perror("select error");
			break;
		} else if (sres == 0) {
			/* should not be here for blocked select without timeout*/
			printf("select timeout\n");
		} else {
			if (FD_ISSET(0, &rfds)) {
				nread = read(0, buffer, sizeof(buffer)-1);
				if (nread <= 0) {
					perror("read error");
					break;
				}
				buffer[nread] = '\0';
				printf("read string(%d): %s\n", nread, buffer);
			}
		}
	
	}
	reset_terminal(&old_args);
	return 0;
}

void setup_terminal(struct termios* old_args) {
	tcgetattr(0, old_args);

	struct termios new_args = *old_args;
	new_args.c_lflag &= ~ICANON;
	new_args.c_cc[VMIN] = 1;

	tcsetattr(0, TCSANOW, &new_args);
}

void reset_terminal(struct termios* old_args) {
	tcsetattr(0, TCSANOW, old_args);
}
