#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ReadFilters.h"
#include "InputHandler.h"



// /------------------------------------------------------------------------------------------------
int client_input(int argc, char** argv){																// Tratamento dos inputs iniciais do cliente

	
	switch(argc){

		case 1:																							// Só 1 argumento, avisa o formato correto de utilização

			printf("\n\n#> Comandos que podes utilizar:\n\n\t%s status\n\t%s filters\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\n", argv[0], argv[0], argv[0]);
			fflush(stdout);
			return 2;																					// *2* caso não tenha nada para fazer

		case 2:
			
			if (!strcmp(argv[1], "status") || !strcmp(argv[1], "filters")){

				return 0;																				// *0* associado ao pedido status e filter
			}
			else if(!strcmp(argv[1], "transform")){														// Nº de argumentos errado

					printf("\n#> Uso correto do comando transform:\n\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\n", argv[0]); fflush(stdout);
					return 2;																			// *2* caso não tenha nada para fazer
				}
				
			else{
					printf("\n#> Esse comando nao foi reconhecido"); fflush(stdout);					// Comando não reconhecido
					printf("\n\n#> Comandos que podes utilizar:\n\n\t%s status\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\t%s filters\n\n\n", argv[0], argv[0], argv[0]);
					return 2;																			// *2* caso não tenha nada para fazer
				}

		case 3:																							// Aviso do formato correto de utilização
		case 4:
			printf("\n#> Uso correto do comando transform:\n\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\n", argv[0]); fflush(stdout);
			return 2;																					// *2* caso não tenha nada para fazer


		default:

			if(!strcmp(argv[1], "transform"))															// Em princípio, será um comando transform
				return 1;																				// *1* se for um comando valido, em principio
	}

	return 2;																							// *2* caso esteja errado
}
// /------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------
int confirm_transform_input(char* program_name, int argc, char** argv){									// Verifica o formato do input do cliente


	if(argc < 4){																						// (Lembrar que este argc é sem o nome do programa)
		printf("\n#> Nao especificaste nenhum filtro a aplicar."); fflush(stdout);
		printf("\n#> Uso correto do comando:\n\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n", program_name); fflush(stdout);
		return 1;
	}


	if(strcmp(argv[0], "transform")){																	// Se o comando não for do tipo "transform",
																										// Avisar o erro
		printf("\n#> O comando pedido nao foi reconhecido\n"); fflush(stdout);
		return 1;																						// Retorno de 1
	}


	char* pointer_input = argv[1];																		// Ficheiro de input
	char* pointer_output = argv[2];																		// Ficheiro de output
																										// Pretende-se verificar a extensão dos mesmos
	pointer_input = strchr(pointer_input, (int)'.'); if(!pointer_input){ printf("\n#> O ficheiro de entrada nao tem o formato correto\n"); fflush(stdout); return 2;}
																										// Apenas se verifica se tem um "."
	pointer_output = strchr(pointer_output, (int)'.'); if(!pointer_output){ printf("\n#> O ficheiro de saida nao tem o formato correto (.mp3)\n"); fflush(stdout); return 3;}

	pointer_output++;																					// Avança-se na string, para a extensão do ficheiro
	if(!strcmp(pointer_output, "mp3")){																	// Se o output não tiver a extensão "mp3", retornar o erro

		return 0;																						// Retorno 0 => sucesso
	}
	else{

		printf("\n\n#> O ficheiro de saida tem de ter o formato .mp3 ...\n\n"); fflush(stdout);
	}

	return 4;																							// Retorno 4 => insucesso
}
// /------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------
char** string_to_args(char* string_input, int* n_args){													// Parse de uma string para os seus argumentos constituintes
																										// (Transforma-se no formato dos argc/argv na main)
	*n_args = 0;																						// O número de argumentos é inicializado
	int size = 10;																						// Tamanho pré-definido para o nº de argumentps
	char** argv = malloc(sizeof(char*) * size);															// Array onde ficarão os argumentos separados
	int argv_index = 0;																					// Índice auxiliar
	char* current;


	char* pointer = string_input;																		// Pointer auxiliar para a string do input
	
	while(pointer[0] != '\0' && pointer[0] != '\n'){													// Enquanto essa string não estiver vazia

		if(argv_index == size){																			// Se o nº de argumentos sobrepuser o tamanho
																										// pré-definido de 10, duplicar o mesmo
			size *= 2;
			char** aux = realloc(argv, sizeof(char*) * size);
			if(aux) argv = aux;
		}

		size_t length = strcspn(pointer, " \n\r\0");														// Verificar o tamanho da string até ao próximo "espaço"

		current = malloc(sizeof(char) * (length+1));												// Alocar o tamanho para essa substring
		strncpy(current, pointer, length); current[length] = '\0';										// Cópia e encerramento da mesma
			
		argv[argv_index] = current;																		// Guardar no array
		argv_index++;																					// Incrementar o seu índice
		
		if(length != strcspn(pointer, "\0")) {															// Se o tamanho da string inicial for diferente do tamanho até ao fim da string
																										// Quer dizer que só há mais argumentos
			pointer = strchr(pointer, (int)' ');														// Avança-se na string para o próximo
			if(pointer) pointer++;
		}
		else pointer[0] = '\0';																			// Caso contrário, só havia 1 argumento, então encerra-se o ciclo
	}

	*n_args = argv_index;																				// O nº de argumentos coincidirá com o nº do índice
	return argv;																						// Retorno dos argumentos
}
// /------------------------------------------------------------------------------------------------



