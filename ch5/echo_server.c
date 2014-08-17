#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define SOCKET_BACKLOG 	100
#define SERVER_PORT 	1234
#define BUF_SIZE 	256

void echo(int fd);

void setup_signal_handler();

int main() {
	/* setup SIGCHLD handler */
	setup_signal_handler();
  
	/* define socket address */
	struct sockaddr_in server = {0};
	server.sin_family = AF_INET;
	server.sin_port = htons( SERVER_PORT );
	

	/* define socket file descriptor */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* bind socket file descriptor to the socket address */
	bind(server_fd, (struct sockaddr *)&server, sizeof(server));
	
	/* listen on this socket file descriptor */
	listen( server_fd, SOCKET_BACKLOG );	

	/* define socket struct/file descriptor used to present remote peer(client) */
	struct sockaddr_in client = {0};	
	int client_fd;
	int client_sockaddr_len = 0;
	
	/* application send buffer */
	char buffer[BUF_SIZE];

	while (1) {
		printf("server ready to accept\n");
		client_fd = accept(server_fd, (struct sockaddr *)&client, &client_sockaddr_len);
		if (client_fd < 0) {
			/* if SA_RESTART is not set in setup_signal_handler and
			 * then when process is interrupted by SIGCHLD 
			 * the accept() will return EINTR instead of a valid socket fd
			 */
			printf("server accept error!\n");
			continue;
		}
		if (fork() == 0) {
			close(server_fd);
			printf("child process start pid(%d)\n", getpid());

			echo(client_fd);
			
			printf("child process exit  pid(%d)\n", getpid());
			exit(0);
		}	
		close(client_fd);	
	}

	return 0;
}

void echo(int fd) {
	int n;
	char buffer[BUF_SIZE];
	while ((n = read(fd, buffer, BUF_SIZE)) > 0) {
		write(fd, buffer, n);
	}
}

void signal_child_handler(int signo) {
	pid_t pid;
	int stat;

	/* pid = wait(&stat); 
	 * signal is not queued, many child process exits
	 * may just cause one signal handle process
	 * so we should use waitpid() in a row instead of a single wait()
	 * to collect child process information
	 */
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child process pid(%d) terminated\n", pid);
	}
}

void setup_signal_handler() {
	struct sigaction act, old_act;

	act.sa_handler = signal_child_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART;
	printf("SA_RESTART\n");
#endif
	if (sigaction(SIGCHLD, &act, &old_act) < 0) {
		printf("setup SIGCHLD Failed.");
	}
}
