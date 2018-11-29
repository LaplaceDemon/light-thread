/*
 * net.h
 *
 *  Created on: 2018年11月27日
 *      Author: jackiesjq
 */

#ifndef NET_H_
#define NET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#define EPOLL_SIZE 4096

typedef struct listener {
	int sockfd;
	int pollfd;
	struct epoll_event * activity_event_array;
} Listener;

typedef struct connection {
	int fd;
	Listener * plistener;
} Connection;

int listener_init(Listener* plistener,const struct sockaddr *my_addr, const socklen_t addrlen);

Connection * listener_accept(Listener * plistener);

int conn_read(Connection * pconn, char * buf, size_t count);

int conn_write(Connection * pconn, char * buf, size_t count);

int conn_close_free(Connection * pconn);

int listener_close();



#endif /* NET_H_ */
