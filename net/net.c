/*
 * net.c
 *
 *  Created on: 2018年11月27日
 *      Author: jackiesjq
 */

#include "../net/net.h"

#include <stdlib.h>
#include <arpa/inet.h>
#include "../light_thread.h"
#include <fcntl.h>
#include <stdio.h>

int set_nonblock_mode(int fd);

int listener_init(Listener* plistener, const struct sockaddr * my_addr, const socklen_t addrlen) {
	int ret = 0;
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	ret = bind(listenfd, my_addr, addrlen);
	if (ret < 0) {
		return ret;
	}
	ret = listen(listenfd, 128);
	if (ret < 0) {
		return ret;
	}

	int pollfd = epoll_create(EPOLL_SIZE);

	struct epoll_event register_event;
	register_event.data.fd = listenfd;
	register_event.events = EPOLLIN;

	ret = epoll_ctl(pollfd, EPOLL_CTL_ADD, listenfd, &register_event);
	if (ret < 0) {
		return ret;
	}
	plistener->activity_event_array = (struct epoll_event *) malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	plistener->sockfd = listenfd;
	plistener->pollfd = pollfd;
	return ret;
}

Connection * listener_accept(Listener * plistener) {
	int listenfd = plistener->sockfd;
	int pollfd = plistener->pollfd;
	struct epoll_event * activity_event_array = plistener->activity_event_array;
	int newclientfd = -1;
	for (;;) {
		int events_num = epoll_wait(pollfd, activity_event_array, EPOLL_SIZE, 1000);
		if (events_num < 0) {
			break;
		} else if (events_num == 0) {
			continue;
		} else {
			int i;
			for (i = 0; i < events_num; i++) {
				//get one listenfd per call epoll_wait;
				if (activity_event_array[i].data.fd == listenfd) {
					struct sockaddr_in new_client_addr;
					socklen_t client_addr_len;
					newclientfd = accept(listenfd, (struct sockaddr *) &new_client_addr, &client_addr_len);
				} else {
					LightThreadTask * pLightThreadTask = (LightThreadTask *) activity_event_array[i].data.ptr;
					// 理论上不会有非空的pLightThreadTask。
					if(pLightThreadTask != NULL) {
						light_thread_resume(pLightThreadTask);
					} else {
						printf("maybe have error\n");
					}
				}

				/*
				 if (activity_event_array[i].events & EPOLLIN) {
				 // readable event
				 light_thread_resume(pLightThreadTask);

				 } else if (activity_event_array[i].events & EPOLLOUT) {
				 // writable event
				 light_thread_resume(pLightThreadTask);
				 }
				 */
			}

			if(newclientfd == -1) {
				continue;
			}

			int ret = set_nonblock_mode(newclientfd);
			if(ret < 0) {
				return NULL;
			}

			Connection * pconn = (Connection *) malloc(sizeof(Connection));
			pconn->fd = newclientfd;
			pconn->plistener = plistener;

			return pconn;
		}
	}

	return NULL;
}

int conn_read(Connection * pconn, char * buf, size_t count) {
	while (1) {
		int len = read(pconn->fd, buf, count);
		if (len >= 0) {
			int ret = epoll_ctl(pconn->plistener->pollfd, EPOLL_CTL_DEL, pconn->fd, NULL);
			if (ret < 0) {
				return ret;
			}

			return len;
		}

		// len < 0, can't read fd;
		// 关注可读事件
		struct epoll_event register_event;
		register_event.events = EPOLLIN | EPOLLET;
		register_event.data.ptr = PSchdule->p_curLightThreadTask;

		int ret = epoll_ctl(pconn->plistener->pollfd, EPOLL_CTL_ADD, pconn->fd, &register_event);
		if (ret < 0) {
			return ret;
		}

		// 等待可读恢复
		light_thread_wait();
	}
}

int conn_write(Connection * pconn, char * buf, size_t count) {
	while (1) {
		int len = write(pconn->fd, buf, count);
		if (len >= 0) {
			int ret = epoll_ctl(pconn->plistener->pollfd, EPOLL_CTL_DEL, pconn->fd, NULL);
			if (ret < 0) {
				return ret;
			}

			return len;
		}

		// len < 0, can't read fd;
		// 关注可读事件
		struct epoll_event register_event;

		register_event.events = EPOLLOUT | EPOLLET;
		register_event.data.ptr = PSchdule->p_curLightThreadTask;

		int ret = epoll_ctl(pconn->plistener->pollfd, EPOLL_CTL_ADD, pconn->fd, &register_event);
		if (ret < 0) {
			return ret;
		}

		// 等待可读恢复
		light_thread_wait();
	}
}

int conn_close_free(Connection * pconn) {
	int ret = close(pconn->fd);
	free(pconn);
	return ret;
}

int set_nonblock_mode(int fd) {
	int flag = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

