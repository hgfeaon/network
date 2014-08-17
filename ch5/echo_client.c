#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_PORT 1234
#define SERVER_IP "127.0.0.1"

#define BUF_SIZE 256

void send_echo(FILE* fp, int fd);

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	
	inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

	connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	send_echo(stdin, sockfd);

	return 0;
}

void send_echo(FILE* fp, int fd) {
	char send_buf[BUF_SIZE] = {0};
	char recv_buf[BUF_SIZE] = {0};

	int readn = 0;
	int writen = 0;

	while(fgets(send_buf, BUF_SIZE, fp) != NULL) {
		if ((writen = write(fd, send_buf, strlen(send_buf) + 1)) < 0) {
			printf("1st write error\n");
			break;
		} else {
			printf("1st write ok\n");
		}

		sleep(1);

		if ((writen = write(fd, "(test)", strlen("(test)") + 1)) < 0) {
			printf("2nd write error\n");
			break;
		} else {
			printf("2nd write ok\n");
		}

		sleep(1);
		
		if ((readn = read(fd, recv_buf, BUF_SIZE)) < 0) {
			printf("read error\n");
			break;
		} else if (readn == 0) {
			printf("read EOF\n");
			break;
		}
		fputs(recv_buf, stdout);
	}
	printf("client exit\n");
}
