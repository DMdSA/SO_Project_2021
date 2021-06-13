#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ReadFilters.h"
#include "InputHandler.h"



// /------------------------------------------------------------------------------------------------
int confirm_transform_input(char* program_name, int argc, char** argv){


	if(argc < 4){															//Lembrar que este argc é sem o nome do programa!!!!
		printf("\n#> Nao especificaste nenhum filtro a aplicar."); fflush(stdout);
		printf("\n#> Uso correto do comando:\n\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n", program_name); fflush(stdout);
		return 1;
	}


	if(strcmp(argv[0], "transform")){

		printf("\n#> O comando pedido nao foi reconhecido\n"); fflush(stdout);
		return 1;
	}


	char* pointer_input = argv[1];
	char* pointer_output = argv[2];

	pointer_input = strchr(pointer_input, (int)'.'); if(!pointer_input){ printf("\n#> O ficheiro de entrada nao tem o formato correto\n"); fflush(stdout); return 2;}
	pointer_output = strchr(pointer_output, (int)'.'); if(!pointer_output){ printf("\n#> O ficheiro de saida nao tem o formato correto (.mp3)\n"); fflush(stdout); return 3;}

	//printf("\n#> in: \"%s\", out: \"%s\"", pointer_input, pointer_output);
	pointer_output++;
	if(!strcmp(pointer_output, "mp3")){

		return 0;
	}
	else{

		printf("\n\n#> O ficheiro de saida tem de ter o formato .mp3 ...\n\n"); fflush(stdout);
	}

	return 4;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------
char** string_to_args(char* string_input, int* n_args){

	*n_args = 0;
	int size = 10;
	char** argv = malloc(sizeof(char*) * 10);
	int argv_index = 0;

	char* pointer = string_input;
	
	while(pointer[0] != '\0' && pointer[0] != '\n'){

		if(argv_index == size){

			size *= 2;
			char** aux = realloc(argv, sizeof(char*) * size);
			if(aux) argv = aux;
		}

		size_t length = strcspn(pointer, " \0");
		char* current = malloc(sizeof(char) * length+1);
		memcpy(current, pointer, length); current[length] = '\0';
		argv[argv_index] = current;
		argv_index++;
		if(length != strcspn(pointer, "\0")) {
			
			pointer = strchr(pointer, (int)' ');
			if(pointer) pointer++;
		}
		else pointer[0] = '\0';
	}

	*n_args = argv_index;

	return argv;
}
// /------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------
int client_input(int argc, char** argv){

	
	switch(argc){

		case 1:

			printf("\n\n#> Comandos que podes utilizar:\n\n\t%s status\n\t%s filters\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\n", argv[0], argv[0], argv[0]);
			fflush(stdout);
			return 2;													// *2* caso não tenha nada para fazer

		case 2:
			
			if (!strcmp(argv[1], "status") || !strcmp(argv[1], "filters")){

				return 0;												// *0* associado ao pedido Status
			}
			else if(!strcmp(argv[1], "transform")){

					printf("\n#> Uso correto do comando transform:\n\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\n", argv[0]); fflush(stdout);
					return 2;											// *2* caso não tenha nada para fazer
				}
				
			else{
					printf("\n#> Esse comando nao foi reconhecido"); fflush(stdout);
					printf("\n\n#> Comandos que podes utilizar:\n\n\t%s status\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\t%s filters\n\n\n", argv[0], argv[0], argv[0]);
					return 2;											// *2* caso não tenha nada para fazer
				}

		case 3:
		case 4:
			printf("\n#> Uso correto do comando transform:\n\n\t%s transform input-filename output-filename filter-id-1 filter-id-2 ...\n\n\n", argv[0]); fflush(stdout);
			return 5;


		default:

			if(!strcmp(argv[1], "transform"))
				return 1;												// *1* se for um comando valido, em principio
	}

	return 2;															// *2* caso esteja errado
}
// /------------------------------------------------------------------------------------------------