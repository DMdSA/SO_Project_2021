#ifndef READFILTERS_HEADER
#define READFILTERS_HEADER


/**
 * Struct que armazena a informação de cada filtro
 */
typedef struct filter* FILTER;


/**
 * Struct que armazena todos os filtros carregados para o sistema
 */
typedef struct filters_folder* FILTERS_FOLDER;



/**
 * Devolve o id de um filtro
 * @param f filtro
 * @returns id
 */
char* get_filter_id(FILTER f);


/**
 * Devolve o nome do executável de um filtro
 * @param f filtro
 * @returns nome executável
 */
char* get_filter_exe(FILTER f);


/**
 * Devolve o nº máximo de usos concorrentes de um filtro
 * @param f filtro
 * @returns nº de usos
 */
unsigned int get_filter_max(FILTER f);


/**
 * Devolve o nº de usos correntes de um filtro, no momento
 * @param f filtro
 * @returns nº de usos
 */
unsigned int get_filter_em_uso(FILTER f);


/**
 * Modifica diretamente o nº de filtros em uso corrente de um filtro
 * @param f filtro
 * @param n novo nº de filtros em uso
 */
void set_used_filters(FILTER f, int n);

/**
 * Criação de uma struct que guarda um filtro
 * @param id identificador do filtro
 * @param exe nome do executável
 * @param max nº máximo de usos concorrentes
 * @returns filtro
 */
FILTER create_Filter(char* id, char* exe, unsigned int max);


/**
 * Efetua o parse de uma linha de um ficheiro config, contendo informação sobre os filtros disponíveis
 * @param line linha de texto
 * @returns filtro
 */
FILTER parse_line(char* line);


/**
 * Impressão da informação de um filtro
 */
void print_Filter(FILTER f);


/**
 * Impressão da informação de um filtro, versão direcionada para o cliente
 */
void print_Filter_clients(FILTER cf);


/**
 * Liberta a memória utilizada por um filtro
 */
void free_Filter(FILTER f);


/**
 * Confirma se um bloco de informação tem um formato legível
 * @param line bloco
 * @param filters_fd file descriptor do ficheiro que está a ser lido
 */
void confirm_block(char* line, int filters_fd);


/**
 * Lê um bloco de informação do ficheiro e subdivide-os em strings com formato proveitoso
 * @param line bloco de texto
 * returns array com substrings retiradas do bloco
 */
char** read_block(char* line);


/**
 *  Free das substrings criadas a partir de um bloco
 * @param block_substrings strings a serem libertadas
 */
void free_block(char** block_substrings);


/**
 * Função de leitura, respetivo parse e armazenamento de dados
 * @param filepath path do ficheiro com os filtros
 * @param *N inteiro onde ficará guardado o nº de filtros carregados
 */
FILTER* storeFilters(char* filepath, unsigned int* N);


/**
 * Free de um array de filtros
 * @param *stored array de filtros
 */
void free_stored_filters(FILTER* stored);


/**
 * \brief Confirma a existência dos filtros pedidos por um utilizador
 * @param argc nº de argumentos passados
 * @param argv argumentos
 * @param stored_filters filtros armazenados
 * @return 0, em caso de sucesso
 */
int confirm_filters(int argc, char** argv, FILTERS_FOLDER stored_filters);


/**
 * Print dos filtros pedidos por um cliente
 * @param argc nº argumentos
 * @param argv argumentos
 */
void print_asked_filters(int argc, char** argv);


/**
 * Armazenamento de todos os filtros carregados a partir de um ficheiro
 * @param filepath filepath do ficheiro
 * @returns Struct com a lista dos filtros carregados e o seu nº
 */
FILTERS_FOLDER store_filters_folder(char* filepath);


/**
 * Free de uma struct que contém todos os filtros do sistema
 * @param stored struct
 */
void free_filters_folder(FILTERS_FOLDER stored);


/**
 * Devolve um array com todos os filtros armazenados
 * @param ff struct com lista de filtros
 * @returns lista de filtros
 */
FILTER* get_filters(FILTERS_FOLDER ff);


/**
 * Devolve o nº de filtros guardados no sistema
 * @param ff struct com os filtros armazenados
 * @returns nº de filtros
 */
int get_number_of_filters(FILTERS_FOLDER ff);


/**
 * Devolve uma lista dos nomes dos filtros armazenados
 * @param ff struct com os filtros guardados
 * @param *n guarda o nº de filtros encontrados
 * @returns array com os identificadores de todos os filtros
 */
char** get_filters_for_user(FILTERS_FOLDER ff, unsigned int* n);


/**
 * Devolve o nº máximo de usos concorrentes de um filtro
 * @param filter identificador de filtro
 * @param ff struct com os filtros guardados
 * @returns nº máximo de usos
 */
unsigned int get_folder_filter_max(char* filter, FILTERS_FOLDER ff);


/**
 * Devolve o nº de usos correntes de um filtro, no momento
 * @param filter identificador de filtro
 * @param ff struct com os filtros guardados
 * @returns nº de usos
 */
unsigned int get_running_filter(char* filter, FILTERS_FOLDER ff);


/**
 * Devolve o nome do executável de um filtro
 * @param filter_id identificador do filtro
 * @param ff struct com os filtros guardados
 * @returns nome do executável
 */
char* get_executableName(char* filter_id, FILTERS_FOLDER ff);


/**
 * Atualiza, em (+1), o nº de filtros em uso corrente, no momento
 * @param filtername identificador do filtro
 * @param ff filtros guardados
 */
void update_used(char* filtername, FILTERS_FOLDER ff);


/**
 * Atualiza, em (-1), o nº de filtros em uso corrente, no momento
 * @param filtername identificador do filtro
 * @param ff filtros guardados
 */
void update_unused(char* filtername, FILTERS_FOLDER ff);


/**
 * Valida a disponibilidade dos filtros pedidos pelo cliente
 * @param argc nº de argumentos
 * @param argv argumentos
 * @param ff filtros guardados
 * returns *int array com a quantidade de filtros que o utilizador pediu
 */
int* validate_availability(int argc, char** argv, FILTERS_FOLDER ff);


#endif