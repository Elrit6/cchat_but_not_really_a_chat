#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "cli_addrs.h"
#include "globals.h"
#include "sock_utils.h"

SOCKET svr_sock = INVALID_SOCKET;
FD_SET socks, read_socks;

void close_svr(void) {
	for (int i = 0; i < socks.fd_count; ++i) {
		const SOCKET sock = socks.fd_array[i];
		if (sock != INVALID_SOCKET) {
			closesocket(sock);
		}
	}
	WSACleanup();
}

bool init_svr(const int port) {
	if (!sock_init(&svr_sock)) {
		return false;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(svr_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		fprintf(stderr, "bind failed: %d.\n", WSAGetLastError());
		goto err;
	}
	if (listen(svr_sock, SOMAXCONN) == SOCKET_ERROR) {
		fprintf(stderr, "listen failed: %d.\n", WSAGetLastError());
		goto err;
	}

	FD_ZERO(&socks);
	FD_SET(svr_sock, &socks);

	init_cli_addrs();

	return true;

	err:
		close_svr();
		return false;
}

bool handle_new_conn(void) {
	struct sockaddr_in cli_addr;
	int cli_addr_size = sizeof(cli_addr);
	const SOCKET cli_sock = accept(svr_sock, (struct sockaddr*)&cli_addr, &cli_addr_size);
	if (cli_sock == INVALID_SOCKET) {
		fprintf(stderr, "Invalid connection recieved.\n");
		return false;
	}

	FD_SET(cli_sock, &socks);

	char cli_addr_str[addr_str_len];
	snprintf(
		cli_addr_str, addr_str_len, "%s:%u",
		inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port)
	);

	add_cli_addr(cli_sock, cli_addr_str);
	printf("+ <%s>\n", cli_addr_str);

	return true;
}

void rem_cli(const SOCKET cli_sock) {
	FD_CLR(cli_sock, &socks);
	closesocket(cli_sock);
	
	char cli_addr[addr_str_len];
	if (get_cli_addr(cli_sock, cli_addr)) {
		printf("- <%s>\n", cli_addr);
	}
	rem_cli_addr(cli_sock);
}

void rem_new_lines(char *str, const int str_len) {
	for (int i = 0; i < str_len; ++i) {
		if (str[i] == '\n') {
			str[i] = ' ';
		}
	}
}

bool recv_msg_from_cli(const SOCKET cli_sock) {
	char buf[msg_buf_len];
	const int bytes_recv = sock_recv(cli_sock, buf, msg_buf_len);
	if (bytes_recv < 0) {
		goto err;
	}
	buf[msg_buf_len - 1] = '\0';
	rem_new_lines(buf, bytes_recv);

	char cli_addr_str[addr_str_len];
	if (!get_cli_addr(cli_sock, cli_addr_str)) {
		fprintf(stderr, "Couldn't find the client's address.\n");
		goto err;
	}

	char msg[msg_len];
	snprintf(msg, (size_t)msg_len, "<%s> %s", cli_addr_str, buf);
	puts(msg);
	
	return true;

	err:
		rem_cli(cli_sock);
		return false;
}

bool handle_socks() {
	read_socks = socks;
	
	if (select(0, &read_socks, nullptr, nullptr, nullptr) == SOCKET_ERROR) {
		fprintf(stderr, "select failed: %d.\n", WSAGetLastError());
		return false;
	}

	for (int i = 0; i < read_socks.fd_count; ++i) {
		const SOCKET sock = read_socks.fd_array[i];
		if (sock == svr_sock) {
			handle_new_conn();
		} else {
			recv_msg_from_cli(sock);
		}
	}

	return true;
}

int main(const int argc, const char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Port was not given.\n");
		return 1;
	}
	const int port = atoi(argv[1]);
	if (port <= 0 || port == INT_MAX) {
		fprintf(stderr, "Invalid port given.\n");
		return 1;
	}

	if (!init_svr(port)) {
		return 1;
	}
	while (true) {
		if (!handle_socks()) {
			return 1;
		}
	}

	close_svr();
	return 0;
}
