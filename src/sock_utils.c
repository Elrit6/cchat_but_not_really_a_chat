#include "sock_utils.h"

#include <stdio.h>

bool sock_init(SOCKET *sock) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		fprintf(stderr, "WSAStartup failed: %d.\n", WSAGetLastError());
		return false;
	}

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*sock == INVALID_SOCKET) {
		fprintf(stderr, "socket failed: %d.\n", WSAGetLastError());
		return false;
	}

	return true;
}

int sock_recv(const SOCKET sock, char *buf, const int buf_len) {
	const int bytes_recv = recv(sock, buf, buf_len, 0);
	if (bytes_recv == SOCKET_ERROR) {
		fprintf(stderr, "recv failed: %d.\n", WSAGetLastError());
		return -1;
	}
	return bytes_recv;
}

bool sock_send(const SOCKET sock, char *buf, const int buf_len) {
	const int bytes_sent = send(sock, buf, buf_len, 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "send failed: %d.\n", WSAGetLastError());
		return false;
	} if (bytes_sent < buf_len) {
		fprintf(stderr, "Failed to send all the data.\n");
		return false;
	}
	return true;
}
