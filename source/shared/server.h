// Public API - server

#ifndef SERVER

#include <sys/socket.h>
#include <stdio.h>

#define SERVER

/**
 * @brief Создание, настройка и перевод в режим прослушки сокета
 *
 * @param[in]  addr Указатель на структуру сетевого адреса
 * @param[in]  backlog Размер очереди подключений
 * 
 * @return 
 * - -1 в случае ошибки
 * @return 
 * - файловый дескриптор в случае успеха
 *
 * @warning Указатель addr должен быть заранее определённым
 */
int socket_create(const struct sockaddr *addr, int backlog);

/**
 * @brief Полная отправка данных из буфера клиенту
 * 
 * @param[in] client_fd Клиентский дескриптор
 * @param[in] buffer Буфер с данными
 * @param[in] buffsize Размер буфера с данными
 * 
 * @return кол-во отправленных байт | -1
 */
ssize_t socket_send_all(int client_fd, const char *buffer, size_t buffsize);
#endif