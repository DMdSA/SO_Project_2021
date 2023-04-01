#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "ReadFilters.h"
#include "Communication.h"

#define CLIENT_LINE 1200


// /------------------------------------------------------------------------------------------------------------
void simple_error_handler(int error, char* msg){																// Envia uma mensagem "de erro" para o STDOUT

	if(error < 0){

		write(1, msg, strlen(msg));
		exit(1);																								// E retorna 1
	}
}
// /------------------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------
void read_from_server(){																						// Lê uma mensagem enviada pelo servidor

	char* read_text = malloc(sizeof(char) * CLIENT_LINE);
	if(!read_text) {simple_error_handler(-1, "\n#> Erro a alocar memoria : Communication");}					// Mensagem de erro para debug 

	ssize_t read_bytes = -1;																					// Número de bytes que serão registados aquando da leitura

	int communication_fd = open("tmp/s_to_c", O_RDONLY);														// Abrir o "terminal" de comunicação
	simple_error_handler(communication_fd, "\n#> [server_status] : O servidor provavelmente esta offline\n");	// Se não for possível comunicar c/ servidor

	//write(1, "\n", 1);

	if(!fork()){																								// Leitura
		while( (read_bytes = read(communication_fd, read_text, CLIENT_LINE)) > 0){

			read_text[strlen(read_text)] = '\0';
			simple_error_handler(read_bytes, "\n#> Erro na leitura do servidor : Communication");				// Se houver erro na leitura, erro e termina
			int write_status = write(1, read_text, strlen(read_text));
			simple_error_handler(write_status, "\n#> Erro na escrita do cliente : Communication");				// Se houver erro na escrita, erro e termina
		}
	}
	else{
		wait(NULL);																								// Esperar que a leitura termine
	}
	

	free(read_text);																							// Free da memória alocada para ler a info do servidor
	close(communication_fd);																					// Fechar o terminal de comunicação
}
// /------------------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------------------
int write_to_cliente(char* something){																			// Automatização no processo de comunicação
																												// com o cliente, por parte do servidor

	int server_fd = open("tmp/s_to_c", O_WRONLY);																// Abrir o terminal de comunicação
	simple_error_handler(server_fd, "\n#> [server_error]: Nao foi possivel criar ligacao com cliente\n");		// Em caso de erro, avisar e sair
	if(server_fd < 0) return -1;
	
	//write(server_fd, "\n", 1);
	
	char* aux = malloc(sizeof(char) * strlen(something) + 2);
	sprintf(aux, "%s\n", something);

	int write_status = write(server_fd, aux, strlen(aux)+1);											// Escrita para o cliente
	
	free(aux);
	close(server_fd);																							// Fechar o terminal de comunicação
	
	simple_error_handler(write_status, "\n#> [server_error]: Nao foi possivel escrever para o cliente\n");		// Em caso de erro, avisar
	if(write_status < 0) return -1;

	return 0;
}
// /------------------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------------------
int server_client_input(char** tasks, int n_tasks, int argc, char** argv, FILTERS_FOLDER ff){					// "Interpretador" do servidor
																												// Aqui os argc/argv são diferentes dos passados na main
	
	if(!strcmp(argv[0], "status")){																				// Caso o cliente tenha pedido o status

		char* line; char*aux;
		int size = get_number_of_filters(ff);																	// Número de filtros carregados no sistema
		FILTER* filters = get_filters(ff);																		// Filtros carregados no sistema

		line = malloc(sizeof(char) * CLIENT_LINE);																// Alocação de memória para a mensagem a retornar
		aux = malloc(sizeof(char) * 500);
		strcpy(line, ""); strcpy(aux, "");

		if(tasks && n_tasks > 0){																								
			for(int i = 0; i < n_tasks; i++){
			
				sprintf(aux, "#> [task #%d] - [%s]\n", i+1, tasks[i]);
				strcat(line, aux);
				strcpy(aux, "");
				//write_to_cliente(line);
			}
			//write_to_cliente(line);
		}

		for(int i = 0; i < size; i++){																			// Enquanto houver filtros
			
			char* current_filter = get_filter_id(filters[i]);													// Percorrer 1 a um
			sprintf(aux, "#> [server_status] - Filter [%s] : %d/%d (running/max)\n", current_filter, get_filter_em_uso(filters[i]), get_filter_max(filters[i]));
			strcat(line, aux);
			strcpy(aux, "");
																												// Definir o formato da informação a ser passada
			free(current_filter);																				// Free da memória alocada para receebr o nome do filtro
		}

		pid_t server_pid = getpid();																			// PID do processo atual, para
		sprintf(aux, "#> [server_status] - pid : %d\n", server_pid);											// também retornar ao cliente
		strcat(line, aux);
		free(aux);
		strcat(line, "\0");
		write_to_cliente(line);																					// Envio da mensagem
		free(line);																								// Free da memória alocada para as mensagens acima expostas

		return 0;																								// Retorno de 0, em sucesso
	}


	else if (!strcmp(argv[0], "filters")){																		// Se o pedido for para ver os filtros disponiveis

		char* line = malloc(sizeof(char) * CLIENT_LINE);														// Alocação de memória para a mensagem a retornar
		char* aux = malloc(sizeof(char) * 500); strcpy(line, "");
		int size = get_number_of_filters(ff);																	// Número de filtros
		FILTER* filters = get_filters(ff);																		// Filtros

		for(int i = 0; i < size; i++){																			// Enquanto há filtros

			char* current_filter = get_filter_id(filters[i]);
			sprintf(aux, "#> %s\n", current_filter);
			strcat(line, aux);
			free(current_filter);
		}
		write_to_cliente(line);																					// Enviar os seus nomes para o cliente

		free(aux);free(line);
		return 0;																								// Retorno de 0, sucesso
	}


	else if(!confirm_filters(argc, argv, ff) && !(strcmp(argv[0], "transform"))){								// Se o pedido for uma transformação

			char* succ = "#> User_input recognized!";															// Se os filtros pedidos forem encontrados
			write(1, succ, strlen(succ));																		// Apenas é impresso no servidor
			return 1;																							// *1* success
	}

	else if(!strcmp(argv[0], "transform")){																		// Filtros não encontrados

			for(int i = 0; i < argc; i++){
				printf("\"%s\" ", argv[i]); fflush(stdout);
			}

			write_to_cliente("\n#> [server_status] : Not all filters you ordered are recognized\n");			// Avisa o cliente do seu erro
			return -1;
	}

	return -1;																									// Retorna -1, caso insucesso
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------// Função de leitura do servidor auxiliar
void wait_before_leaving(){																						// necessária para controlar o término do comando
																												// do utilizador
	char disposable[10];
	ssize_t read_bytes = -1;

	int communication_fd = open("tmp/s_to_c", O_RDONLY);
	simple_error_handler(communication_fd, "\n#> [server_status] : O servidor provavelmente esta offline\n");

	while( (read_bytes = read(communication_fd, disposable, 10)) > 0){

		simple_error_handler(read_bytes, "\n#> Erro na leitura do servidor : Communication");					// Se houver erro na leitura, erro e termina
		int write_status = write(1, disposable, read_bytes);
		simple_error_handler(write_status, "\n#> Erro na escrita do cliente : Communication");					// Se houver erro na escrita, erro e termina
	}	
	
	write(1, "\n", 1);
	close(communication_fd);
}
// /------------------------------------------------------------------------------------------------------------