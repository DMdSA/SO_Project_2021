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


#define LINE_SIZE 400										/* Tamanho pré-definido para as linhas lidas do cliente 				*/

FILTERS_FOLDER stored_filters;								/* Struct global com os filtros carregados								*/

#define N_EXECS 30											/* Nº pré-definido para o máximo de processos concorrentes				*/	/* Valores tento em conta o enunciado*/
#define N_FILTERS 30										/* Nº pré-definodo para o máximo de filtros que o programa tem 			*/	/* Valores tento em conta o enunciado*/

int pid_to_update[N_EXECS][N_FILTERS];						/* Array com os pids dos processos criados (+) nº de filtros que usaram	*/

int pids_index = 0;											/* Índex do array global com os pids 									*/

char** tasks;
int n_tasks = 0;
/*
void print_array(int array[N_EXECS][N_FILTERS]){

	printf("wow\n"); fflush(stdout);

	for(int i = 0; i < N_EXECS; i++){

		for(int a = 0; a < N_FILTERS; a++){
			printf("%d ", pid_to_update[i][a]);fflush(stdout);
		}
	}
	printf("\n");fflush(stdout);
}
*/




// /------------------------------------------------------------------------------------------------------------
void ctrl_c_handler(int signal){																				// CTRL-C handler

	char bye[] = "\n#> Leaving program...\n\n";						//aqui devia esperar que todos terminassem ne?..

	write(1, bye, strlen(bye));										//Antes sizeof

	free_filters_folder(stored_filters);																		// Free dos filtros
	unlink("tmp/c_to_s");																						// Retira ambos os fifos
	unlink("tmp/s_to_c");																						// Retira ambos os fifos
	free(tasks);
	exit(0);																									// Retorna 0
}
// /------------------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------------------// SIGCHILD handler
void sig_child_handler(int signum) {
	
	//print_array(pid_to_update);

	int status; pid_t pid;

	while ( (pid = waitpid(-1, &status, WNOHANG)) > 0) {														// Sempre que houver processos child a terminar
		
		int i = 0; int pid_line = 0;
		for(; i < N_EXECS; i++){

			if(pid_to_update[i][0] == pid){																		// Procura, no array de processos criados
				pid_line = i;																					// o pid do processo que terminou
				i = N_EXECS;
			}
		}

		if(pid_to_update[pid_line][0] != pid) return;															// Se o pid não tiver sido encontrado, provavelmente não vai
																												// interessar ao funcionamento do programa

		if(pid_to_update[pid_line][0] == pid && WEXITSTATUS(status) == 1){										// Se o processo tiver terminado com um código de saída 1
			kill(pid, SIGKILL);																					// Terminá-lo, se, por alguma razão, ainda não tiver terminado
			printf("\n#> Program could not execute as wished");
			return;
		}

		FILTER* filters = get_filters(stored_filters);															// Filtros guardados
		int size = get_number_of_filters(stored_filters);														// Número de filtros
		
		for(int a = 0; a < size; a++){																			// Percorrer os filtros

			int before = get_filter_em_uso(filters[a]);															// Número de filtros em uso, no momento
			
			if(before - pid_to_update[pid_line][a+1] >= 0){														// Se o processo tiver acrescentado algum uso de algum filtro
																												// Verificar que não dá nºs negativos

				set_used_filters(filters[a], (before - pid_to_update[pid_line][a+1]));							// Atualizar o nº de filtros em uso, para o valor antes de acrescentar
																												// (esse valor está guardado no array de pids, na mesma linha de cada pid)
			}
		}

		free(tasks[pid_line]); n_tasks--;
		
		pid_to_update[pid_line][0] = 0;
		for(int a = 1; a < N_FILTERS; a++){
			pid_to_update[pid_line][a] = 0;
		}
		pids_index--;
	}
	//print_array(pid_to_update);
}
// /------------------------------------------------------------------------------------------------------------





// /------------------------------------------------------------------------------------------------------------// Início do servidor
int start_server(){

	printf("\n#> A ligar servidor ...\n");
	
	mkfifo("tmp/c_to_s", 0644);																				// Criação de pipe para leitura do cliente
	mkfifo("tmp/s_to_c", 0644);																				// Criação de pipe para escrita para cliente

	int server_fd = open("tmp/c_to_s", O_RDONLY);																// Abrir pipe para leitura (cliente-servidor)

	simple_error_handler(server_fd, "\n#> [server_status]: Nao foi possivel iniciar o servidor\n");				// Aviso, caso dê erro

	printf("\n#> [online]: O servidor encontra-se agora online\n\n"); fflush(stdout);
	fflush(stdout);
	return server_fd;
}
// /------------------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------// Encerramento do servidor
int close_server(int fifo_fd){

	printf("\n#> [server_status]: A encerrar servidor ...\n"); fflush(stdout);
	int succ = close(fifo_fd);
	
	simple_error_handler(succ, "\n#> [server_status]: Nao foi possivel encerrar o servidor\n");

	printf("\n#> [offline]: O servidor foi encerrado com sucesso\n"); fflush(stdout);
	return 0;
}
// /------------------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------// Controlo dos argumentos passados à main
int controlo_main(int argc, char** argv){

	if(argc < 3 || argc > 3){
		printf("\n\n#> Utilizacao correta: %s config-filename filters-folder\n\n", argv[0]); fflush(stdout);
		return 1;
	}
	return 0;
}
// /------------------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------------------// Execução do aurrasd
int execute_aurras(char* executable_name, char* inputfile, char* outputfile, int control){						// control == 0, (input != output)
																												// control != 0, (input == output)
		int status;
		int input_fd;
		int output_fd;
		//fflush(stdout);
		char executable[strlen(executable_name+2)]; strcpy(executable, ".");									// Tratamento dos pedidos de cada filtro
		strcat(executable, executable_name); executable[strlen(executable_name)+1] = '\0';						// Para trabalhar com os seus executáveis
		write(1, executable, strlen(executable));
		write(1, "\n", 1);
		
		if(!fork()){

			if(control == 0){																					// Se for o 1º filtro, é aplicado ao "sample"
				
				input_fd = open(inputfile, O_RDONLY);															// Open do sample, para leitura
				if(input_fd == -1){
					write_to_cliente("Esse ficheiro nao parece estar disponivel");								// Tratamento em caso de erro
					return 1;
				}

				output_fd = open(outputfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);								// Open do output
				if(output_fd == -1){																			// Tratamento em caso de erro
					printf("\n#> erro a criar \"%s\"\n", outputfile); fflush(stdout);
					write_to_cliente("Nao foi possivel criar o ficheiro de output");
					return 1;
				}
			}

			else{																								// Se não for o 1º filtro, é aplicado ao próprio output
				
				input_fd = open(inputfile, O_RDONLY);															// Open do input, Leitura
				if(input_fd == -1){

					write_to_cliente("Esse ficheiro nao parece estar disponivel");
					return 1;																					// De notar que aqui o input == output
				}
				output_fd = open(inputfile, O_WRONLY);															// Open do input, Escrita
				if(output_fd == -1){

					write_to_cliente("Esse ficheiro nao parece estar disponivel");
					return 1;
				}
			}


			if(control == 0){																					// Há 2 files diferentes
				dup2(input_fd, 0); 																					// Alterar o stdin/stdout do exec
				close(input_fd);																					// Close dos fd que já não serão usados
				dup2(output_fd, 1);																					// Alterar o stdin/stdout do exec
				close(output_fd);}																					// Close dos fd que já não serão usados
			
			else{																								// Só há 1 file, mas o processo é idêntico
				dup2(input_fd, 0);
				close(input_fd);
				dup2(output_fd, 1);
				close(output_fd);
			}

			int success;

			if((success = execl(executable, executable_name, NULL)) == -1)
				_exit(1);																						//	Se o exec não funcionar, devolve código 1
			_exit(0);																							//	(Em sucesso, devolve 0..)
		}
		else{

			wait(&status);
		}

		printf("\n");fflush(stdout);
		return WEXITSTATUS(status);
	}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Automatiza o processo de execução do aurrasd
	int run(char** filters, char* inputfile, char*outputfile, int n_filters){


		fflush(stdout);
		int counter = 0;
		

		while(counter < n_filters){																	// Aplicar sucessivamente os N filtros pedidos
		
			int confirm = -1;
			if(counter == 0) {

				confirm = execute_aurras(filters[counter], inputfile, outputfile, 0);				// 1º aplicação
			}
			
			else{

				confirm = execute_aurras(filters[counter], outputfile, outputfile, 1);				// Seguintes aplicações, se houverem
			}
			
			if(confirm == 0) counter++;
				else{ printf("\n#> [Server_status] : Erro no aurras!\n"); fflush(stdout); return -1;}
		}
		return 0;
	}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Preparação das variáveis corretas para execução do aurrasd
int tratamento_execucao(int argc, char** argv, FILTERS_FOLDER ff){

	if(strcmp(argv[0], "transform")) return -1;

	int referencia_filtros = 3;																		// Índice onde começam os filtros
	char** executaveis = malloc(sizeof(char*) * (argc - referencia_filtros));						// Array onde se guardarão os executáveis de cada filtro
	int index = 0;
	fflush(stdout);
	

	for(int i = referencia_filtros; i < argc; i++){

		executaveis[index] = get_executableName(argv[i], ff);										// Guardar cada executável
		index++;
	}


	int confirm_success;
	confirm_success = run(executaveis, argv[1], argv[2], argc - referencia_filtros);



	for(int i = 0; i < (argc - referencia_filtros); i++){											// Free da memória alocada auxiliar
		free(executaveis[i]);
	} free(executaveis);

	return confirm_success;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Atualiza os filtros em uso, consoante o pedido do cliente
int update_filters(int argc, char** argv, FILTERS_FOLDER ff, int control){							// control==1, incrementa, control==2, decrementa

	int index_ref = 3;																				// Indice do primeiro filtro

	for(int i = index_ref; i < argc; i++){															// Enquanto houver filtros

		char* current_filter = argv[i];
		
		if(control == 1)
			update_used(current_filter, ff);														//  Atualizar em (+1) o filtro, no campo in_use
		
		else
			update_unused(current_filter, ff);
	}	
	return 0;
}
// /------------------------------------------------------------------------------------------------





// /------------------------------------------------------------------------------------------------// MAIN
int main(int argc, char** argv){	


    signal(SIGCHLD, sig_child_handler);																// Signals
    signal(SIGINT, ctrl_c_handler);																	// Signals

	if(controlo_main(argc, argv)) return 1;															// Argumentos inválidos


	for(int i = 0; i < N_EXECS; i++){																// Inicializar array
																									// Para ter a certeza de que não vai
		for(int a = 0; a < N_FILTERS; a++){															// ter lixo
			pid_to_update[i][a] = 0;
		}
	}

	char** aux_tasks = malloc(sizeof(char*) * N_EXECS);
	tasks = aux_tasks;


	char* filepath = argv[1];																		// Filepath do ficheiro de configuração
	FILTERS_FOLDER aux_folder = store_filters_folder(filepath);
	stored_filters = aux_folder;																	// Carregamento dos filtros para o sistema
	
	int n_filters = get_number_of_filters(stored_filters);


	char* line_buffer = malloc(sizeof(char) * LINE_SIZE);											// Alocação da memória para a linha que receberá os inputs do cliente
	strcpy(line_buffer, "\0");
	if(!line_buffer) simple_error_handler(-1, "\n#> Erro a alocar memoria para line_buffer: Server.c:main\n");
	

	int sv_fd = start_server();																		// Iniciar o servidor


	int read_bytes = 0;
	int current_pid;
	
	while(1){																						// Manter o serviço ativo
																									// Child Process code

			while( (read_bytes = read(sv_fd, line_buffer, LINE_SIZE)) > 0){							// Enquanto for possível a leitura do pipe

				ssize_t length = strcspn(line_buffer, "\n\r\0");

				line_buffer[length] = '\0';															// Definir o fim da string

				int n_args = 0;																		// Transformar a linha em argumentos
				char** my_argv = string_to_args(line_buffer, &n_args);


				int option = server_client_input(tasks, n_tasks, n_args, my_argv, stored_filters);	// Tratamento do input recebido

				int* used;

				if(option == 1){

					used = validate_availability(n_args, my_argv, stored_filters);					//check da disponibilidade de filtros
					if(used){


						update_filters(n_args, my_argv, stored_filters, 1);							// Atualiza os filtros no campo "in_use"


						switch((current_pid = fork())){


							case 0:																	// Executa o programa

								write_to_cliente("#> processing...");
								tratamento_execucao(n_args, my_argv, stored_filters);
								//write_to_cliente("#> file created");
								_exit(0);

								break;


							case -1:

								write(1, "Erro no fork\n", 13);										// Erro
								break;


							default:

								if(option == 1){
									
									char* current = malloc(sizeof(char) * strlen(line_buffer) + 10);
									strcpy(current, my_argv[0]);

									for(int i = 1; i < n_args; i++){

										strcat(current, " ");
										strcat(current, my_argv[i]);
									}
									strcat(current, "\0");
									tasks[pids_index] = current;
									n_tasks++;

									pid_to_update[pids_index][0] = current_pid;							// Guardar sempre em 1º o pid do processo
									for(int i = 0; i < n_filters; i++){
										pid_to_update[pids_index][i+1] = used[i];						// Atualizar a linha do pid c/ nº dos filtros pedidos
									}
									pids_index++;														// Atualizar o index do array de pids
								}

								free(used);
								break;
						}


					}

					else{
						free(used);
						printf("\n#> Nao ha recursos disponiveis ainda"); fflush(stdout);
						write_to_cliente("\n#> [server_status] : there are no resources available for your order");
					}
				}


	
				for(int i = 0; i < n_args; i++){													// Quando a tarefa terminar
					free(my_argv[i]);																// Free da transformação dos argumentos feita anteriormente
				}	free(my_argv);

			memset(line_buffer,'\0',length);
			}
	}



	close_server(sv_fd);																			// Encerramento do servidor


// /------
	free_filters_folder(stored_filters);															// Free da memória alocada para os filtros
// /------

	free(line_buffer);																				// Free memória alocada para a leitura do FIFO
	return 0;
}