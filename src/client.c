#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "globals.h"
#include "sock_utils.h"

SOCKET cli_sock = INVALID_SOCKET;

void close_cli(void) {
	if (cli_sock != INVALID_SOCKET) {
		closesocket(cli_sock);
	}
	WSACleanup();
}

bool init_cli(const char *ip, const int port) {
	if (!sock_init(&cli_sock)) {
		goto err;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)port);
	addr.sin_addr.s_addr = inet_addr(ip);

	if (connect(cli_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		fprintf(stderr, "connect failed, code: %d.\n", WSAGetLastError());
		goto err;
	}

	return true;

	err:
		close_cli();
		return false;
}

bool msg_loop(void) {
	char buf[msg_buf_len];
	while (true) {
		printf(">");
		scanf("%511[^\n]", buf);
		getchar(); // remove last \n
		if (!sock_send(cli_sock, buf, msg_buf_len)) {
			return false;
		}
	}
	return true;
}

int main(const int argc, const char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Address and/or port not given.\n");
		return 1;
	}

	const char *ip = argv[1];
	const int port = atoi(argv[2]);
	if (port <= 0 || port == INT_MAX) {
		fprintf(stderr, "Invalid port given.\n");
		return 1;
	}
	if (!init_cli(ip, port)) {
		return 1;
	}

	if (!msg_loop()) {
		return 1;
	}

	return 0;
}
