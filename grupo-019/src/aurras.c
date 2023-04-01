#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "ReadFilters.h"
#include "Communication.h"
#include "InputHandler.h"


#define ARGV_LENGTH 400



// /------------------------------------------------------------------------------------------------------------
void ctrl_c_handler(int signal){																				// CTRL-C handler

	char bye[] = "\n#> Leaving...\n\n";
	write(1, bye, strlen(bye));
	
	exit(0);																									// Retorna 0
}
// /------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv){

	
	signal(SIGINT, ctrl_c_handler);																			// CTRL-C handler

	int communication_fd = open("tmp/c_to_s", O_WRONLY);

	simple_error_handler(communication_fd, "\n#> [offline]: the server seems to be offline\n\n\n");			// Se o servidor estiver offline, retorna 1

	int control = 0;																						// Auxiliar de controlo

	if( (control = client_input(argc, argv)) <= 1){															// O control define qual a tarefa a ser realizada

		if(control == 1){																					// ==1, Confirma o formato de um pedido de transform
																											// Nomeadamente a extensão dos ficheiros de input/output passados
			if(!confirm_transform_input(argv[0], argc-1, (argv+1)));
			else return 1;																					// Caso não esteja no formato correto, retorna 1
		}


			char* argv_line = malloc(sizeof(char) * ARGV_LENGTH);											// Tamanho pre-definido para concatenar os argumentos da main
   			
   			strcpy(argv_line, argv[1]); strcat(argv_line, " ");												// Processo de concatenação
   			for(int i = 2; i < argc; i++){
   				strcat(argv_line, argv[i]);
   				if(i < argc-1) strcat(argv_line, " ");														// O resultado é uma única linha de texto
   			}
			
			write(communication_fd, argv_line, strlen(argv_line));											// A linha com os argumentos é passada ao servidor
			free(argv_line);																				// Free da memória alocada

			if(!strcmp(argv[1], "status") || !strcmp(argv[1], "filters")){									// Se o pedido tiver sido "status" ou "filters", fica à
				
				read_from_server();																			// espera de uma resposta do servidor
				return 0;																					// Retorna 0
			}

			if(control == 1){																				// Se o pedido tiver sido um "transform"

				printf("\n#> pending...\n");																// Fica à espera da disponibilidade
				fflush(stdout);
				read_from_server();																			// Fica à espera que o servidor inicie a execução do pedido
				//wait_before_leaving();
				//wait_before_leaving();

			}

	}

	close(communication_fd);																				// Término da comunicação c/servidor
	return 0;																								// Retorna 0
}