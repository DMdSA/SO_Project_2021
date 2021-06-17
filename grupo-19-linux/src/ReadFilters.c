#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ReadFilters.h"
#include "Communication.h"

#define BLOCK_SIZE 500


// /-----------------------------------//					*typedef struct filter*
typedef struct filter{				   //
									   //
	char* identifier;				   //					(+) Identificador do filtro (nome)
	char* executable;				   //					(+) Nome do executável associado
	unsigned int max_per;			   //					(+) Número máximo de usos concorrentes
	unsigned int in_use;			   //					(+) Número de usos correntes
} filter;							   //
// /-----------------------------------//


// /-----------------------------------//
typedef struct filters_folder{		   //					*typedef struct filters_folder*
									   //
	FILTER* filters;				   //					(+) Array de filtros guardados
	unsigned int number_of_filters;	   //					(+) Número de filtros
} filters_folder;					   //
// /-----------------------------------//


// /------------------------------------------------------------------------------------------------//	Receber o nome/identificador de um filtro
char* get_filter_id(FILTER f){

	int length = strlen(f->identifier);																//Tamanho da string
	char* copy = malloc(sizeof(char) * (length +1));												// Alocação da memória
	memcpy(copy, f->identifier, length); copy[length] = '\0';										// Cópia
	return copy;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------//	Receber o executável de um filtro
char* get_filter_exe(FILTER f){

	int length = strlen(f->executable);																// Tamanho da string
	char* copy = malloc(sizeof(char) * (length +1));												// Alocação
	memcpy(copy, f->executable, length); copy[length] = '\0';										// Cópia
	return copy;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber o nº máximo de usos concorrentes do filtro
unsigned int get_filter_max(FILTER f){

	unsigned int copy = f->max_per;
	return copy;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber o nº máximo de usos concorrentes
unsigned int get_folder_filter_max(char* filter, FILTERS_FOLDER ff){								// Através da string com o nome do filtro

	unsigned int aux = get_number_of_filters(ff);													// Nº de filtros guardados
	for(int i = 0; i < aux; i++){																	// Percorrer 1 a 1

		if(!strcmp(filter, (ff->filters[i])->identifier)){											// Quando encontrar o pedido

			int copy = (ff->filters[i])->max_per;													// Retornar o seu nº máx
			return copy;
		}
	}

	return -1;																						// Retorno -1 caso o filtro não exista/não esteja guardado
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber o nº de usos correntes de um filtro
unsigned int get_filter_em_uso(FILTER f){

	unsigned int copy = f->in_use;
	return copy;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Rreceber o nº de usos correntes de um filtro
unsigned int get_running_filter(char* filter, FILTERS_FOLDER ff){									// através do seu identificador/nome

	unsigned int aux = get_number_of_filters(ff);													// Nº filtros
	for(int i = 0; i < aux; i++){																	// Percorrer 1 a 1

		if(!strcmp(filter, (ff->filters[i])->identifier)){											// Quando encontrar

			int copy = (ff->filters[i])->in_use;													// Devolver
			return copy;
		}
	}

	return -1;																						// -1, caso erro
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Definir o nº de usos correntes
void set_used_filters(FILTER f, int n){																// de um filtro

	f->in_use = n;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Criação de uma struct do tipo filter
FILTER create_Filter(char* id, char* exe, unsigned int max){

	FILTER new = malloc(sizeof(struct filter));														// Alocação
	if(new != NULL){
		new->identifier = id;																		// Atribuição das variáveis
		new->executable = exe;
		new->max_per = max;
		new->in_use = 0;
	}
	return new;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Parser de uma linha do ficheiro config
FILTER parse_line(char* line){

	/** Uma linha é composta pelo formato: "<identificador de filtro> <nome executável> <máx de usos concorrentes>" */

	char id_[100]; char exe_[100]; unsigned int max = 0;											// Valores de 100 pré-definido, alterar caso se veja necessário


	int verify = 0;
	if( ( verify = sscanf(line, "%[^ ] %[^ ] %u", id_, exe_, &max)) == 3){							// Verifica se a linha tem o formato correto e se consegue,
																									// efetivamente, ler os 3 elementos
			int length = strlen(id_);
			char* id = malloc(sizeof(char) * length +1);
			memcpy(id, id_, length); id[length] = '\0';

			length = strlen(exe_);
			char* exe = malloc(sizeof(char) * length +1);
			memcpy(exe, exe_, length); exe[length] = '\0';

			return create_Filter(id, exe, max);														// A partir da informação retirada de uma linha
	}																								// devolve uma nova struct com a informação guardada

	else{																							// Caso não tenha o formato correto
		return NULL;																				// Devolve NULL
	}


/*													//Esta versão é sem o sscanf, baseia-se mais no controlo e manuseio da própria string (pode, ou nao, ser mais rapida)
	char* pointer = line;
	char* id; char* exe; unsigned int max = 0;
	char integer[10];

		size_t value_length = strcspn(pointer, " \n\0");
		id = malloc(sizeof(char) * (value_length +1));
		memcpy(id, pointer, value_length); id[value_length] = '\0';

		pointer = strchr(pointer, (int)' '); if(pointer) pointer++; else return NULL;
		value_length = strcspn(pointer, " \n\0");
		exe = malloc(sizeof(char) * (value_length +1));
		memcpy(exe, pointer, value_length); exe[value_length] = '\0';

		pointer = strchr(pointer, (int)' '); if(pointer) pointer++; else return NULL;
		value_length = strcspn(pointer, " \r\n\0");		//Se assumir-mos que o formato da file estará sempre correta, esta linha deve poder ser ignorada
		memcpy(integer, pointer, value_length); integer[value_length] = '\0';
		
		char* lixo;
		max = strtoul(pointer, &lixo, 10);
*/
		//return create_Filter(id_, exe_, max);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Print da informação de um filtro
void print_Filter(FILTER f){

	printf("\n\t#> id: \"%s\", exe: \"%s\", max: \"%u\", in_use: \"%u\"", f->identifier, f->executable, f->max_per, f->in_use);	//Mero debug
	fflush(stdout);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Print da informação de um filtro
void print_Filter_clients(FILTER f){																// Versão orientada para os clientes

	printf("\n\t#> filter_id: %s, max_usage: %u, in_use: %u", f->identifier, f-> max_per, f->in_use);					//Client version
	fflush(stdout);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Free de uma estrutura filter
void free_Filter(FILTER cf){

	if(cf != NULL){																					// Dá free à estrutura de dados que guarda a info de cada filtro

		free(cf->identifier);
		free(cf->executable);
		free(cf);
	}
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Ao ler um bloco de informação de uma file config
void confirm_block(char* line, int filters_fd){														// Verifica se lê corretamente "x" linhas, e não "corta" nenhuma

	int last = strlen(line);																		// Calcular o seu tamanho
		
		if(line[last-1] != '\n' && line[last-1] != '\0' && line[last-1] != '\r'){					// Se o último caracter não definir o fim de string

			int control = 1;																		// Int auxiliar de controlo											

			while(control == 1){																	// É preciso corrigir a string lida


				for(int i = 0; i < 130; i++){														// Até encontrar um caracter de fim de linha, a partir do fim da string

					if(line[last-1] == '\0' || line[last-1] == '\n' || line[last-1] == '\r'){		// Quando esse caracter é encontrado
						line[last-1] = '\n'; line[last] = '\0';										// Atualiza-se o final dessa string (dá jeito acabar em \n, para outra função)
						lseek(filters_fd, -(i+1), SEEK_CUR);										// E também se atualiza o pointer da file, movendo-o para trás
						i = 130;																	// Definir o fim do ciclo
						control = 0;
					}
					else{
						line[last-1] = '\0';														// Caso contrário eliminar o conteúdo que lá estava a mais
						last -= 1;																	// /Atualizar o novo tamanho da string
					}
				}

			}
		}
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Lê um bloco de informação do ficheiro config
char** read_block(char* line){																		// Ao ler um bloco de informação, agrupa-o de forma vantajosa
																									// em substrings, para posterior tratamento
	char* pointer = line;
	if(pointer == NULL) return NULL;																// Se não houver nada para ler, retornar NULL

	int size = 10;																					// Tamanho pré-definido para o nº de linhas de um bloco
	char** configs = malloc(sizeof(char*) * size);													// Alocação para as linhas

	unsigned int i = 0;

	while(pointer){																					// Enquanto houver texto

		if(i == size-1){																			// Atualizar o tamanho alocado para as linhas, se for necessário

			size = size * 2;
			char** aux = NULL;
			aux = realloc(configs, sizeof(char*) * size);
			if (aux != NULL) configs = aux;
		}

		size_t length = strcspn(pointer, "\r\n\0"); if(length == 0) break;							// Se a linha estiver vazia, sair do ciclo
		char* current = malloc(sizeof(char) * length+1);											// Caso contrário, guardar a linha lida
		memcpy(current, pointer, length); current[length] = '\0';


		if(current){																				// Se a linha lida contiver informação
			configs[i] = current;																	// Guardá-la no array
			i += 1;
		}

		pointer = strchr(pointer, (int)'\n'); if(pointer) pointer++; else break;					// Avançar na String
	}

	configs[i] = NULL;																				// Definir o final do array
	return configs;																					// Retorno do array com as linhas lidas
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Free da memória alocada para um bloco de informação
void free_block(char** block){

	if (block != NULL)
	for(unsigned int i = 0; block[i] != NULL; i++){
		free(block[i]);
	}

	free(block);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Automatização do processo de leitura, parse, criação e armazenamento dos filtros
FILTER* storeFilters(char* filepath, unsigned int* N){												// a partir de um ficheiro config


	int filters_fd = open(filepath, O_RDONLY);														// Abertura do ficheiro, para leitura
	size_t bytes_read = 0;

	char* line_buff = malloc(sizeof(char) * BLOCK_SIZE);											// Alocação para o tamanho de um bloco de linhas lidas

	int store_size = 15;																			// Tamanho pré-definido para o nº de filtros que serão guardados
	FILTER* store = malloc(sizeof(struct filter) * store_size);										// Alocação da memória para esse efeito
	int store_index = 0;																			// Índex auxiliar

	if(!store){ simple_error_handler(-1, "\n#> Error: nao foi possivel alocar memoria para os filtros\n"); return NULL;}
																							

	while( (bytes_read = read(filters_fd, line_buff, BLOCK_SIZE)) > 0){								// Enquanto houver informação para ser lida

		line_buff[bytes_read] = '\0';																// Definir o fim da linha lida

		confirm_block(line_buff, filters_fd);														// Confirma se o bloco de leitura efetuado tem um formato "decente" e atualiza-o, caso seja
																									// necessário
		char** block = read_block(line_buff);														// Parse do bloco para diferentes linhas

		if(store_index == store_size -1){															//Se for necessário, alocar mais espaço para o array

			store_size *= 2;
			FILTER* aux = realloc(store, sizeof(struct filter) * store_size);
			if(aux != NULL){
				store = aux;
			}
		}

		for(int i = 0; block[i] != NULL; i++){														// Enquanto houver linhas para ler
			
			FILTER this = parse_line(block[i]);														// Dar o respetivo parse à linha e criar o respetivo FILTER
			if(this == NULL) {
				printf("\n#> Error: Nao foi possivel criar um config_file\n");						// debug
				fflush(stdout);
			}

			else{

				store[store_index] = this;															// Guarda o filtro criado no array que lhe é destinado
				store_index++;
			}
		}
	}

	*N = store_index;																				// Guarda o nº de filtros carregados
	store[store_index] = NULL;																		// Atualiza o fim do array de filtros
	close(filters_fd);																				// Close do ficheiro aberto para leitura
	free(line_buff);																				// Free do buffer criado para ler cada bloco
	return store;
}
// /------------------------------------------------------------------------------------------------



// /------------------------------------------------------------------------------------------------// Free de um array de filtros
void free_stored_filters(FILTER* stored){

	if(stored != NULL){

		for(int i = 0; stored[i] != NULL; i++)
			free_Filter(stored[i]);
	}

	free(stored);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Print dos filtros pedidos por um cliente
void print_asked_filters(int argc, char** argv){

	int start_index = 3;
	printf("\n#> Filtros pedidos: "); fflush(stdout);
	
	for(int i = start_index; i < argc; i++){
		printf("%s ", argv[i]);
		fflush(stdout);
	}
	printf("\n\n");
	fflush(stdout);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Criação e armazenamento de uma struct filters_folder
FILTERS_FOLDER store_filters_folder(char* filepath){


	unsigned int n = 0;
	FILTER* fs = storeFilters(filepath, &n);														// Contém um array de filtros

	FILTERS_FOLDER store = malloc(sizeof(struct filters_folder));
	store->filters = fs;
	store->number_of_filters = n;
	return store;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Free de um filters_folder
void free_filters_folder(FILTERS_FOLDER stored){

	free_stored_filters(stored->filters);
	free(stored);
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Print do array de filtros guardados no sistema
void print_filters_list(FILTER* stored_filters){

	if(!stored_filters){ simple_error_handler(-1, "\n#> Neste momento nao estao disponiveis quaisquer filtros\n"); return;}


	printf("\n#> Filtros atualmente disponiveis:\n"); fflush(stdout);
	for(int i = 0; stored_filters[i] != NULL; i++){

		print_Filter_clients(stored_filters[i]);
	}
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber o array de filtos guardados no sistema
FILTER* get_filters(FILTERS_FOLDER ff){

	if(!ff){ simple_error_handler(-1, "\n#> Erro no get_filters: Struct filters_folder vazia\n"); return NULL;}
	return ff->filters;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber o nº de filtros guardados
int get_number_of_filters(FILTERS_FOLDER ff){

	if(!ff){ simple_error_handler(-1, "\n#> Erro no get_filters: Struct filters_folder vazia\n"); return 0;}
	int nfilters = ff->number_of_filters;
	return nfilters;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Confirma a existência dos filtros pedidos por um utilizador
int confirm_filters(int argc, char** argv, FILTERS_FOLDER stored_filters){

	if(strcmp(argv[0], "transform")) return -1;														// Se o comando não for "transform", retorna -1

	int start_index = 3;																			// Os filtros começam no índex 3
	int found_flag = 0;																				// flag auxiliar de controlo
	int n_filters = get_number_of_filters(stored_filters);											// nº filtros
	FILTER* filters = get_filters(stored_filters);													// filtros

	for(int i = start_index; i < argc; i++){														// Enquanto houver filtros

		found_flag = 0;

		for(int aux = 0; aux < n_filters; aux++){													// Percorrer os filtros guardados

			char* current_folder_filter = get_filter_id(filters[aux]);								// 1 a 1
			
			if(!strcmp(current_folder_filter, argv[i])){ 											// Se for igual a algum dos filtros pedidos, ativar a flag
				
				free(current_folder_filter);
				found_flag = 1;
				aux = n_filters;
			}
			else free(current_folder_filter);
		}

		if(found_flag == 0){																		// Se em algum filtro pedido a flag não for ativa

			printf("\n#> Os filtros que pediste nao estao todos disponiveis"); fflush(stdout);		// é porque o filtro não existe no sistema
			print_filters_list(filters);
			return 1;
		}
	}
	return 0;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber uma lista com os nomes dos filtros existentes (strings)
char** get_filters_for_user(FILTERS_FOLDER ff, unsigned int* n){

	FILTER* filters = get_filters(ff);																// Filtros
	unsigned int n_filters = get_number_of_filters(ff);												// Nº filtros

	char** filters_ids = malloc(sizeof(char*) * n_filters);											// Alocação do array
	
	for(unsigned int i = 0; i < n_filters; i++){

		char* filter = filters[i]->identifier;
		int length = strlen(filter);
		char* current = malloc(sizeof(char) * (length+1));
		memcpy(current, filter, length); current[length] = '\0';

		filters_ids[i] = current;
	}

	*n = n_filters;																					// Devolve também o nº de filtros
	return filters_ids;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Receber o nome do executável de um filtro
char* get_executableName(char* filter_id, FILTERS_FOLDER ff){


	char* path = malloc(sizeof(char)*70);
	strcpy(path, "/bin/aurrasd-filters/");


	FILTER* filters = get_filters(ff);
	unsigned int aux = get_number_of_filters(ff);
	for(unsigned int i = 0; i < aux; i++){
		if(!strcmp(get_filter_id(filters[i]), filter_id)){

			strcat(path, get_filter_exe(filters[i]));
			path[strlen(path)] = '\0';
			return path;
		}
	}
	free(path);
	return NULL;

}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Valida a disponibilidade dos filtros pedidos num "transform"
int* validate_availability(int argc, char** argv, FILTERS_FOLDER ff){

	if(strcmp(argv[0], "transform")) return NULL;
	
	FILTER* filters = get_filters(ff);																// filtros disponiveis
	int index_ref = 3;																				// os filtros começam no indice 3
	int size = get_number_of_filters(ff);															// numero de filtros guardados
	
	int* num_of_used = malloc(sizeof(int) * size);													// array com a contagem dos filtros que se vao usar
	for(int i = 0; i < size; i++) num_of_used[i] = 0;												// inicializar a 0

	
	char** seen_filters = malloc(sizeof(char*) * size);												// filtros já percorridos
	int seen_index = 0;																				// index auxiliar do array de filtros ja percorridos
	int flag_found = 0;																				// flag de controlo

	for(int i = index_ref; i < argc; i++){															// percorrer os filtros dos argumentos

		char* current_filter = argv[i];
		int current_counter = 1;																	// Inicio da contagem de cada filtro
		flag_found = 0;																				// Para já, assumir que não está no array auxiliar

		for(int a = 0; a < seen_index; a++){														// Percorrer o array auxiliar
			if(!strcmp(current_filter, seen_filters[a])){											// Se encontrar o mesmo filtro, ativar flag
				flag_found = 1;																		// Ou seja, a sua contagem já foi feita
				a = seen_index;
			}
		}


		if(!flag_found){																			// se for a primeira vez a ser lido															

			for(int aux = i+1; aux < argc; aux++){													// Verifica todos os filtros iguais no pedido
				if(!strcmp(argv[aux], current_filter)){
					current_counter++;																// Faz a respetiva contagem
				}
			}


			seen_filters[seen_index] = current_filter;												// Procede à sua inserção no array de filtros já vistos
			seen_index++;																			// Atualiza o índice desse array

			write(1, &current_counter, 1);
			int max = get_folder_filter_max(current_filter, ff);									//	Verifica-se o máx de usos que esse filtro pode ter
			int in_use = get_running_filter(current_filter, ff);									//	Verifica-se o nº de usos correntes que o filtro tem

			
			if(current_counter > (max - in_use)) return NULL;										// Se algum dos filtros pedidos nao estiver disponivel, devolver NULL

			for(int aux = 0; aux < size; aux++){													// Proceder à inserção da contagem, caso seja possível

				char* ordered_filter = get_filter_id(filters[aux]);
				if(!strcmp(ordered_filter, current_filter)){
					num_of_used[aux] = current_counter;
					aux = size;
				}
				free(ordered_filter);
			}
		}
	}
	free(seen_filters);
	return num_of_used;
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Atualiza, em (+1), o nº de usos correntes de um filtro
void update_used(char* filtername, FILTERS_FOLDER ff){

	FILTER* filters = get_filters(ff);
	int size = get_number_of_filters(ff);
	for(int i = 0; i < size; i++){

		char* id = get_filter_id(filters[i]);
		if(!strcmp(id, filtername)){

			filters[i]->in_use++;
			free(id);
			return;
		}
		else free(id);
	}
}
// /------------------------------------------------------------------------------------------------


// /------------------------------------------------------------------------------------------------// Atualiza, em (-1), o nº de usos correntes de um filtro
void update_unused(char* filtername, FILTERS_FOLDER ff){

	FILTER* filters = get_filters(ff);
	int size = get_number_of_filters(ff);
	for(int i = 0; i < size; i++){

		char* id = get_filter_id(filters[i]);

		if(!strcmp(id, filtername)){
			if((filters[i])->in_use > 0){
				filters[i]->in_use--;
				free(id);
				return;
			}
		}
		else free(id);
	}
}
// /------------------------------------------------------------------------------------------------