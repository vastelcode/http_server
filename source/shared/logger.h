// Public API - infra/logger.c
#ifndef LOGGER

#include <pthread.h>
#include <stdio.h>
#include "constants.h"

#define LOGGER

/**
 * @brief Выводит форматированное сообщение со статусом в файл с захватом мьютекса
 * 
 * @param[in]  status  Статус вывода
 * @param[out]  logfile  Указатель на файл, куда будет записан результат
 * @param[in,out] mutex  Указатель на мьютекс, который будет заблокирован
 * @param[in] format Форматная строка
 *
 *
 * @note Параметр mutex может иметь значение NULL
 */
void logger(status_log status ,FILE *logfile ,pthread_mutex_t *mutex ,char *format , ...);

#endif