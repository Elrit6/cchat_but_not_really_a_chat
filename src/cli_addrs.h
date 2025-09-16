#pragma once

#include <WinSock2.h>

#include "globals.h"

typedef struct {
	char addr_str[addr_str_len];
	SOCKET sock;
	bool taken;
} Cli_Addr;

constexpr int max_cli_count = 256;

void init_cli_addrs(void);
bool add_cli_addr(const SOCKET sock, const char *addr_str);
bool get_cli_addr(const SOCKET sock, char *buf);
void rem_cli_addr(const SOCKET sock);
