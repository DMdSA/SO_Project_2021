#ifndef INPUTHANDLER_HEADER
#define INPUTHANDLER_HEADER


/**
 * Verifica o formato de um pedido do cliente
 * @param argc Número de argumentos
 * @param argv Argumentos
 * @returns inteiro associado a cada situação
 * 			0 -> 'status', 'filters'
 * 			1 -> caso possível de 'transform'
 */
int client_input(int argc, char** argv);


/**
 * Confirmação do formato de um pedido do cliente
 * @param program_name Nome do executável
 * @param argc Número de argumentos passados
 * @param argv Argumentos
 * @returns 0 em caso de sucesso
 */
int confirm_transform_input(char* program_name, int argc, char** argv);


/**
 * Transforma uma linha de texto de comandos em argumentos devidamente separados
 * @param string_input Linha de texto
 * @param *n_args Endereço de inteiro onde ficará guardado o nº de argumentos encontrados
 * @returns Array com os argumentos encontrados
 */
char** string_to_args(char* string_input, int* n_args);


#endif