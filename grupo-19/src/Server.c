#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#include "ReadFilters.h"
#include "Communication.h"
#include "InputHandler.h"



#define LINE_SIZE 400

FILTERS_FOLDER stored_filters;

#define N_EXECS 30
#define N_FILTERS 30

int pid_to_update[N_EXECS][N_FILTERS];

int pids_index = 0;




// /------------------------------------------------------------------------------------------------------------
void sig_child_handler(int signum) {
	
	//(void) signum;

	int status; pid_t pid;

	while ( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
		
		int i = 0; int pid_line = 0;
		for(; i < N_EXECS; i++){

			if(pid_to_update[i][0] == pid){
				pid_line = i;
				i = N_EXECS;
			}
		}

		if(pid_to_update[pid_line][0] != pid) return;


		if(pid_to_update[pid_line][0] == pid && WEXITSTATUS(status) == 1){
			kill(pid, SIGKILL);
			printf("\n#> Program could not execute as wished");
			return;
		}

		FILTER* filters = get_filters(stored_filters);
		int size = get_number_of_filters(stored_filters);
		
		for(int a = 0; a < size; a++){

			int before = get_filter_em_uso(filters[a]);
			
			if(before - pid_to_update[pid_line][a+1] >= 0){
				

				set_used_filters(filters[a], (before - pid_to_update[pid_line][a+1]));

			}
		}


		for(; i < N_EXECS; i++){

			for(int aux = 0; aux < size; aux++){

				pid_to_update[i][aux] = pid_to_update[i+1][aux];
			}
		}
		pids_index--;
	}
}
// /------------------------------------------------------------------------------------------------------------





// /------------------------------------------------------------------------------------------------------------
int start_server(){																									// Início do servidor

	printf("\n#> A ligar servidor ...\n");
	
	mkfifo(Client_Server, 0644);																					// Criação de pipe para leitura do cliente
	mkfifo(Server_Client, 0644);																					// Criação de pipe para escrita para cliente
	//simple_error_handler(fifo_confirm, "\n#> Erro a criar fifo : Server.c:start_server()");

	int server_fd = open(Client_Server, O_RDONLY);																	// Abrir pipe para leitura
	simple_error_handler(server_fd, "\n#> [server_status]: Nao foi possivel iniciar o servidor\n");

	printf("\n#> [online]: O servidor encontra-se agora online\n\n"); 
	fflush(stdout);
	return server_fd;
}
// /------------------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------
int close_server(int fifo_fd){

	printf("\n#> [server_status]: A encerrar servidor ...\n"); fflush(stdout);
	int succ = close(fifo_fd);
	
	simple_error_handler(succ, "\n#> [server_status]: Nao foi possivel encerrar o servidor\n");

	printf("\n#> [offline]: O servidor foi encerrado com sucesso\n"); fflush(stdout);
	return 0;
}
// /------------------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------
int controlo_main(int argc, char** argv){

	if(argc < 3 || argc > 3){
		printf("\n\n#> Utilizacao correta: %s config-filename filters-folder\n\n", argv[0]); fflush(stdout);
		return 1;
	}
	return 0;
}
// /------------------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------
int execute_aurras(char* executable_name, char* inputfile, char* outputfile, int control){

		int status;
		int input_fd;
		int output_fd;
		fflush(stdout);
		char executable[strlen(executable_name+2)]; strcpy(executable, ".");			
			strcat(executable, executable_name); executable[strlen(executable_name)+1] = '\0';
			write(1, executable, strlen(executable));
		if(!fork()){

			if(control == 0){																//Se for o 1º filtro, é aplicado ao "sample"
				
				input_fd = open(inputfile, O_RDONLY);										//Open do sample
				if(input_fd == -1){
					//printf("\n#> erro a abrir \"%s\"\n\n", inputfile); fflush(stdout);
					write_to_cliente("Esse ficheiro nao parece estar disponivel");
					return 1;
				}

				output_fd = open(outputfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);			//Open do output
				if(output_fd == -1){
					printf("\n#> erro a criar \"%s\"\n", outputfile); fflush(stdout);
					write_to_cliente("Nao foi possivel criar o ficheiro de output");
					return 1;
				}
			}

			else{																			//Se não for o 1º filtro, é aplicado ao próprio output
				
				input_fd = open(inputfile, O_RDONLY);											//Open do input/output
				if(input_fd == -1){
					//printf("\n#> Erro ao abrir \"%s\"", inputfile); fflush(stdout);
					write_to_cliente("Esse ficheiro nao parece estar disponivel");
					return 1;
				}
				output_fd = open(inputfile, O_WRONLY);											//Open do input/output
				if(output_fd == -1){
					//printf("\n#> Erro ao abrir \"%s\"", inputfile); fflush(stdout);
					write_to_cliente("Esse ficheiro nao parece estar disponivel");
					return 1;
				}
			}


			if(control == 0){																//se for o 1º filtro, há 2 files diferentes
				dup2(input_fd, 0); 																//Alterar o stdin/stdout do exec
				close(input_fd);																//Close dos fd que já não serão usados
				dup2(output_fd, 1);																//Alterar o stdin/stdout do exec
				close(output_fd);}																//Close dos fd que já não serão usados
			
			else{																			//se não for o 1ºfiltro, só há uma file
				dup2(input_fd, 0);
				close(input_fd);
				dup2(output_fd, 1);
				close(output_fd);
			}

			int success;

			if((success = execl(executable, executable_name, NULL)) == -1)
				_exit(1);																	//Se o exec não funcionar, devolve código 1
			_exit(0);																		//Em sucesso, devolve 0
		}
		else{

			wait(&status);
		}

		return WEXITSTATUS(status);
	}
// /------------------------------------------------------------------------------------------------




// /------------------------------------------------------------------------------------------------
	int run(char** filters, char* inputfile, char*outputfile, int n_filters){


		fflush(stdout);
		int counter = 0;

		while(counter < n_filters){															//Aplicar sucessivamente os N filtros pedidos
		
			int confirm = -1;
			if(counter == 0) {

				confirm = execute_aurras(filters[counter], inputfile, outputfile, 0);
			}
			
			else{

				confirm = execute_aurras(filters[counter], outputfile, outputfile, 1);
			}
			
			if(confirm == 0) counter++;
				else{ printf("\n#> [Server_status] : Erro no aurras!\n"); fflush(stdout); return -1;}

		}


		return 0;
	}
// /------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------
int tratamento_execucao(int argc, char** argv, FILTERS_FOLDER ff){

	if(strcmp(argv[0], "transform")) return -1;

	int referencia_filtros = 3;																			// Índice onde começam os filtros
	char** executaveis = malloc(sizeof(char*) * (argc - referencia_filtros));
	int index = 0;
	fflush(stdout);
	

	for(int i = referencia_filtros; i < argc; i++){

		executaveis[index] = get_executableName(argv[i], ff);

		index++;
	}


	int confirm_success;
	if( (confirm_success = run(executaveis, argv[1], argv[2], argc - referencia_filtros)) == 0)



	for(int i = 0; i < (argc - referencia_filtros); i++){
		free(executaveis[i]);
	} free(executaveis);

	return confirm_success;
}
// /------------------------------------------------------------------------------------------------






// /------------------------------------------------------------------------------------------------
int update_filters(int argc, char** argv, FILTERS_FOLDER ff, int control){								//control==1, incrementa, control==2, decrementa

	FILTER* filters = get_filters(ff);																	//Filtros guardados
	int size = get_number_of_filters(ff);																//Numero de filtros

	int index_ref = 3;																					//Indice do primeiro filtro

	
	for(int i = index_ref; i < argc; i++){																//Enquanto houver filtros

		char* current_filter = argv[i];
		
		if(control == 1)
			update_used(current_filter, ff);
		
		else
			update_unused(current_filter, ff);
	}	


	for(int i = 0; i < size; i++){

		char* id = get_filter_id(filters[i]);

		//printf("\n#> Em uso : %d, max: %d", get_running_filter(id, ff), get_maximum_filter(id, ff)); fflush(stdout);
		free(id);
	}

	return 0;
}
// /------------------------------------------------------------------------------------------------








int main(int argc, char** argv){


    signal(SIGCHLD, sig_child_handler);
    signal(SIGINT, ctrl_c_handler);

	if(controlo_main(argc, argv)) return 1;																			// Argumentos inválidos


	for(int i = 0; i < N_EXECS; i++){																				//Inicializar array
																													//Para ter a certeza de que não vai
		for(int a = 0; a < N_FILTERS; a++){																			//Ter lixo
			pid_to_update[i][a] = 0;
		}
	}


	char* filepath = argv[1];																						// Filepath do ficheiro de configuração
	FILTERS_FOLDER aux_folder = store_filters_folder(filepath);
	stored_filters = aux_folder;													// Carregamento dos filtros para o sistema
	int n_filters = get_number_of_filters(stored_filters);


	char* line_buffer = malloc(sizeof(char) * LINE_SIZE);
	if(!line_buffer) simple_error_handler(-1, "\n#> Erro a alocar memoria para line_buffer: Server.c:main\n");
	
	int sv_fd = start_server();																						// Iniciar o servidor


	int read_bytes = 0;
	int current_pid;
	
	while(1){																										// Manter o serviço ativo
	//if(!fork()){																									// Child Process code

			while( (read_bytes = read(sv_fd, line_buffer, LINE_SIZE)) > 0){											// Enquanto for possível a leitura do pipe

				ssize_t length = strcspn(line_buffer, "\n\0\r");

				line_buffer[length] = '\0';																		// Definir o fim da string
				//line_buffer[strcspn(line_buffer, "\n\0")] = '\0';
				//char* s = "#> [server_status] : ";
				//write(1, s, strlen(s));
				//write(1, line_buffer, length);
				//write(1, "\n", 1);


				int n_args = 0;																						// Transformar a linha em argumentos
				char** my_argv = string_to_args(line_buffer, &n_args);


				int option = server_client_input(argv[0], n_args, my_argv, stored_filters, sv_fd);								// Tratamento do input recebido

				int* used;
				used = validate_availability(n_args, my_argv, stored_filters);	//check da disponibilidade de filtros

				if(option == 1){

					used = validate_availability(n_args, my_argv, stored_filters);	//check da disponibilidade de filtros
					if(used){



						update_filters(n_args, my_argv, stored_filters, 1);
						write_to_cliente("processing...");
						//int success;

						//aq deve claramente haver um fork
						switch((current_pid = fork())){


							case 0:

								//printf("\n#> vou entrar no process");fflush(stdout);
								tratamento_execucao(n_args, my_argv, stored_filters);
								_exit(0);

								break;


							case -1:

								write(1, "Erro no fork\n", 13);
								//printf("\n#> Erro no fork");fflush(stdout);
								//while(wait(NULL) > 0); 					//???
								break;


							default:

								pid_to_update[pids_index][0] = current_pid;				//Guardar sempre em 1º o pid do processo

								for(int i = 0; i < n_filters; i++){
									pid_to_update[pids_index][i+1] = used[i];
								}
								pids_index++;
								//printf("\n#> adicionei %d %d %d %d %d %d", pid_to_update[pids_index-1][0], pid_to_update[pids_index-1][1], pid_to_update[pids_index-1][2], pid_to_update[pids_index-1][3], pid_to_update[pids_index-1][4], pid_to_update[pids_index-1][5]);
								//fflush(stdout);
								free(used);

								break;
						}


					}

					else{
						free(used);
						printf("\n#> Nao ha recursos disponiveis ainda"); fflush(stdout);
					}

				}


		
				for(int i = 0; i < n_args; i++){																// Quando a tarefa terminar
					free(my_argv[i]);																			// Free da transformação dos argumentos feita anteriormente
				}	free(my_argv);

			}
			//_exit(0);																								// Código de terminação c/sucesso
	}



	close_server(sv_fd);																							// Encerramento do servidor


// /------
	free_filters_folder(stored_filters);																			// Free da memória alocada para os filtros
// /------
	int unlink_confirm = unlink("/tmp/so_project");																	// Unlink do FIFO (Named Pipe)
	simple_error_handler(unlink_confirm, "\n#> Erro a dar unlink ao fifo\n");

	free(line_buffer);																								// Free memória alocada para a leitura do FIFO
	return 0;
}