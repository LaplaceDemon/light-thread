/*
 * test_net.c
 *
 *  Created on: 2018年11月28日
 *      Author: jackiesjq
 */

#include "net/net.h"
#include "light_thread.h"
#include <arpa/inet.h>
#include <strings.h>
#include <stdio.h>

#define PORT 9090

void handler(void * arg);

int main(int argc, char **argv) {
	schedule_init(8192);
	schedule_start_in_thread();

	struct sockaddr_in serv_sockaddr;
	bzero(&serv_sockaddr, sizeof(serv_sockaddr));
	serv_sockaddr.sin_family = AF_INET;
	serv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_sockaddr.sin_port = htons(PORT);

	Listener listener;
	listener_init(&(listener), (struct sockaddr *) &(serv_sockaddr), sizeof(serv_sockaddr));
	printf("running...\n");
	for (;;) {
		Connection * pConnection = listener_accept(&(listener));
		if(pConnection != NULL) {
			printf("a new connection has been accepted...\n");
			light_thread_create(handler, (void *) pConnection);
		}

	}

	return 0;
}

void handler(void * arg) {
	Connection * pConnection = (Connection *) arg;
	char buf[1024];
	for (;;) {
		int len = conn_read(pConnection, buf, sizeof(buf));
		if (len == 0) {
			conn_close_free(pConnection);
			printf("closed connection");
			return ;
		} else if (len > 0) {
			conn_write(pConnection, buf, len);
		}
	}

}

