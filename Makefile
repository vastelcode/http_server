# Инициализируем переменные

CC = gcc
TARGET = main
FLAGS = -c -Wall -Wextra -Werror

PREF_SRC = ./source/
PREF_OBJ = ./objects/

# Создаём папку для объектных файлов
$(shell mkdir -p ${PREF_OBJ})

# Рекурсивно находим все .c файлы в PREF_SRC и подкаталогах
SRC = $(shell find $(PREF_SRC) -name "*.c")

# Преобразование путей
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

# Сборка исполняемого файла
$(TARGET) : $(OBJ)
	$(CC) -pthread -g -O0 $(OBJ) -o $(TARGET)

$(PREF_OBJ)%.o : $(PREF_SRC)%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $< -o $@


# Очистка: удаляем исполняемый файл и все объектные файлы
clean :
	rm $(TARGET)
	rm -rf $(PREF_OBJ)