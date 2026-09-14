// Public API - server

#ifndef SERVER

#include <sys/socket.h>

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

#endif