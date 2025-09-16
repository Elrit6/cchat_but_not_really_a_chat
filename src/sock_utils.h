#pragma once

#include <WinSock2.h>

bool sock_init(SOCKET *sock);
int sock_recv(const SOCKET sock, char *buf, const int buf_len);
bool sock_send(const SOCKET sock, char *buf, const int buf_len);
