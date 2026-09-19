// Модуль, предоставляющий функции для создания и настроки сокетов

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../shared/logger.h"
#include "../shared/types.h"
#include "../shared/constants.h"

int socket_create(const struct sockaddr *addr, int backlog)
{
	// 1. Открываем сокет
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось открыть сокет",__func__);
		return -1;
	}

	// 2. Настраиваем сокет на переиспользование порта
	int reuse = 1;

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось установить опциональный параметр", __func__);
		close(sockfd);
		return -1;
	}

	// 3. Привязываем сокет к локальному адресу
	if(bind(sockfd, addr, sizeof(*addr)) == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось привязать сокет",__func__);
		close(sockfd);
		return -1;
	}

	// 4. Переводим сокет в режим прослушки
	if(listen(sockfd, backlog) == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось перевести сокет в режим прослушки",__func__);
		close(sockfd);
		return -1;
	}

	return sockfd;
}

ssize_t socket_send_all(int client_fd, const char *buffer, size_t buffsize)
{
	if(buffer == NULL) return -1;

	/* кол-во отправленных байт */
	size_t total = 0;

	while(total < buffsize) {
		/* Отправляем порцию данных клиенту */
		ssize_t n = send(client_fd, buffer + total, buffsize - total , 0);

		if(n == 0) {
			logger(INFO, stdout, NULL, "%s: Клиент закрыл соединение",__func__);
			break;
		}
		if(n < 0) {
			logger(ERROR, stdout, NULL, "%s: Произошла ошибка при отправке данных клиенту",__func__);
			return -1;
		}

		total += n;
	}

	return total;
}