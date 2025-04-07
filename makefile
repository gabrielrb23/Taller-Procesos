# Makefile para el Taller de procesos y arreglos
# Autor: Gabriel Riaño
# Fecha: Abril 2024

# Nombre del ejecutable
TARGET = taller_procesos

# Archivos fuente
SRC = tallerProcesos.c

# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -Wextra -std=c11

# Regla principal
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Limpieza de archivos generados
clean:
	rm -f $(TARGET)
