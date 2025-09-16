#include "cli_addrs.h"
#include <string.h>

static Cli_Addr cli_addrs[max_cli_count];

static void reset_cli_addr(Cli_Addr *cli_addr) {
	cli_addr->addr_str[0] = '\0';
	cli_addr->sock = INVALID_SOCKET;
	cli_addr->taken = false;
}

void init_cli_addrs(void) {
	for (int i = 0; i < max_cli_count; ++i) {
		reset_cli_addr(&cli_addrs[i]);
	}
}

bool add_cli_addr(const SOCKET sock, const char *addr_str) {
	for (int i = 0; i < max_cli_count; ++i) {
		Cli_Addr *cli_addr = &cli_addrs[i];
		if (cli_addr->taken) {
			continue;
		}
		memcpy(cli_addr->addr_str, addr_str, addr_str_len);
		cli_addr->sock = sock;
		cli_addr->taken = true;
		return true;
	}
	return false;
}

bool get_cli_addr(const SOCKET sock, char *buf) {
	for (int i = 0; i < max_cli_count; ++i) {
		Cli_Addr *cli_addr = &cli_addrs[i];
		if (cli_addr->sock == sock) {
			memcpy(buf, cli_addr->addr_str, addr_str_len);
			return true;
		}
	}
	return false;
}

void rem_cli_addr(const SOCKET sock) {
	for (int i = 0; i < max_cli_count; ++i) {
		Cli_Addr *cli_addr = &cli_addrs[i];
		if (cli_addr->sock == sock) {
			reset_cli_addr(cli_addr);
		}
	}
}
