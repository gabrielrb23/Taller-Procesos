/**************************************************************
        Pontificia Universidad Javeriana
        Autores: Gabriel Riaño y Dary Palacios
        Fecha: 7 de Abril 2024
        Materia: Sistemas Operativos
        Tema: Taller de procesos y arreglos
        Fichero: Programa principal para la suma de elementes
		 de arreglos de enteros
        Objetivo: A partir de los ficheros dados se realiza la
		  la suma de sus elementos mediante el uso de 
		  procesos con la funcion fork() y pipe()
****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Función para cargar los enteros desde un archivo en un arreglo
void cargarArreglo(char *nombreArchivo, int *arreglo){

	FILE *file = fopen(nombreArchivo, "r");
	if (file == NULL) {
        	perror("Error al abrir el archivo");
        	return;
    	}

	char linea[256];
	int pos = 0;

	// Imprime los elementos leídos del archivo
	printf("Arreglo de %s: ", nombreArchivo);

	// Lee línea por línea y extrae los números
	while(fgets(linea, sizeof(linea), file)){
		char *token = strtok(linea, " \n");
		while(token != NULL){
			int numero = (int) atoi(token);
			printf("%d ", numero);
			arreglo[pos++] = numero;
			token = strtok(NULL, " \n");
		}
	}
	printf("\n\n");
	fclose(file);
}

// Función para calcular la suma de los elementos de un arreglo
int sumaArreglo(int *arreglo, int numElementos){
	int suma = 0;
	while(numElementos-- > 0){
		suma += arreglo[numElementos];
	}
	return suma;
}

int main(int argc, char *argv[]){

	// Verifica que haya suficientes argumentos en la línea de comandos
	if (argc < 5) {
        	printf("Uso: %s <#_elementos_archivo1> <nombre_archivo1> <#_elementos_archiv2> <nombre_archivo2> \n", argv[0]);
        	return 1;
    	}

	int pipefd[2];          // Pipe principal para enviar datos al padre
	pipe(pipefd);           // Se crea el pipe

	// Proceso padre crea al primer hijo
	pid_t primerHijo = fork();
	if(primerHijo < 0){
		perror("Error creando primerHijo");
		return 1;
	}else if(primerHijo == 0){
		// Proceso primer hijo

		int pipefd_aux[2];  // Pipe auxiliar para comunicación entre nieto/segundo hijo y primer hijo
		pipe(pipefd_aux);   // Se crea el pipe auxiliar

		// Primer hijo crea al nieto
		pid_t grandHijo = fork();

		if(grandHijo < 0){
			perror("Error creando grandHijo");
			exit(1);
		}else if(grandHijo == 0){
			// Proceso nieto

			int n1 = (int) atoi(argv[1]);
			int *vA = (int *) malloc(n1*sizeof(int));     // Reserva espacio para el arreglo
			cargarArreglo(argv[2], vA);                   // Carga datos desde el archivo
			int suma_vA = sumaArreglo(vA, n1);            // Calcula la suma del arreglo

			// Escribe la suma en el pipe principal hacia el padre
			close(pipefd[0]);
			write(pipefd[1], &suma_vA, sizeof(int));			
			close(pipefd[1]);

			// También escribe la suma al pipe auxiliar para el primer hijo
			close(pipefd_aux[0]);
			write(pipefd_aux[1], &suma_vA, sizeof(int));			
			close(pipefd_aux[1]);

			free(vA);
			exit(0);

		}else{
			// Proceso primer hijo (después de crear al nieto)
			wait(NULL); // Espera a que el nieto termine

			// Primer hijo crea al segundo hijo
			pid_t segundoHijo = fork();
			if(segundoHijo < 0){
				perror("Error creando grandHijo");
				exit(1);
			}else if(segundoHijo == 0){
				// Proceso segundo hijo

				int n2 = (int) atoi(argv[3]);
				int *vB = (int *) malloc(n2*sizeof(int));   // Reserva espacio para el arreglo
				cargarArreglo(argv[4], vB);                 // Carga datos desde el archivo
				int suma_vB = sumaArreglo(vB, n2);          // Calcula la suma del arreglo

				// Escribe la suma en el pipe principal hacia el padre
				close(pipefd[0]);
				write(pipefd[1], &suma_vB, sizeof(int));
				close(pipefd[1]);

				// También escribe la suma al pipe auxiliar para el primer hijo
				close(pipefd_aux[0]);
				write(pipefd_aux[1], &suma_vB, sizeof(int));
				close(pipefd_aux[1]);

				free(vB);
				exit(0);
			}

			wait(NULL); // Espera al segundo hijo

			// El primer hijo ahora lee las dos sumas desde el pipe auxiliar
			close(pipefd_aux[1]); // Cierra escritura del auxiliar
			close(pipefd[0]);     // Cierra lectura del pipe principal

			int suma_vA, suma_vB;
			read(pipefd_aux[0], &suma_vA, sizeof(int));
			read(pipefd_aux[0], &suma_vB, sizeof(int));
			close(pipefd_aux[0]);

			// Calcula la suma total
			int sumaArreglos = suma_vA + suma_vB;

			// Escribe la suma total en el pipe principal hacia el padre
			write(pipefd[1], &sumaArreglos, sizeof(int));
			close(pipefd[1]);

			exit(0);
		}
	}else{
		// Proceso padre
		wait(NULL); // Espera al primer hijo

		int sumaArreglos, suma_vA, suma_vB;

		// Lee la suma del nieto
		read(pipefd[0], &suma_vA, sizeof(int));
		printf("Suma del arregloA: %d\n", suma_vA);

		// Lee la suma del segundo hijo
		read(pipefd[0], &suma_vB, sizeof(int));
		printf("Suma del arregloB: %d\n", suma_vB);

		// Lee la suma total calculada por el primer hijo
		read(pipefd[0], &sumaArreglos, sizeof(int));
		printf("Suma de los arreglos: %d\n", sumaArreglos);

		close(pipefd[0]);
		close(pipefd[1]);
	}
	return 0;
}

