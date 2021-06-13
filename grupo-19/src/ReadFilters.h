#ifndef READFILTERS_HEADER
#define READFILTERS_HEADER


typedef struct filter* FILTER;

typedef struct filters_folder* FILTERS_FOLDER;

/**
 * \brief Imprime struct |debug 
 */
char* get_filter_id(FILTER cf);


/**
 * \brief Imprime struct |debug 
 */
char* get_filter_exe(FILTER cf);


/**
 * \brief Imprime struct |debug 
 */
unsigned int get_filter_max(FILTER cf);

unsigned int get_filter_em_uso(FILTER f);

void set_used_filters(FILTER f, int n);

/**
 * \brief Criação de struct com dados de filtros
 *	@param id
 *	@param exe
 *	@param max
 */
FILTER create_Filter(char* id, char* exe, unsigned int max);


/**
 * \brief Dá parse de uma string para uma struct com a respetiva informação
 * @param line
 */
FILTER parse_line(char* line);


/**
 * \brief Imprime struct |debug 
 */
void print_Filter(FILTER cf);


/**
 * \brief Imprime struct |client
 */
void print_Filter_clients(FILTER cf);


/**
 * \brief Free da memória alocada para uma estrutura
 */
void free_Filter(FILTER cf);


/**
 * \brief Lê um bloco de informação do ficheiro e subdivide-os em strings com formato proveitoso
 * @param line
 */
char** read_block(char* line);


/**
 * \brief Dá free do bloco criado aquando da leitura do ficheiro
 * @param block
 */
void free_block(char** block);


/**
 * \brief Confirma que o bloco de informação lida está completo, i.e., não corta informação
 */
void confirm_block(char* line, int filters_fd);


/**
 * \brief Função de leitura, respetivo parse e armazenamento de dados
 * @param filepath
 */
FILTER* storeFilters(char* filepath, unsigned int* N);


/**
 * \brief 
 * @param 
 */
void free_stored_filters(FILTER* stored);


/**
 * \brief 
 * @param 
 */
void free_stored_filters(FILTER* stored);


/**
 * \brief 
 * @param 
 */
int confirm_filters(int argc, char** argv, FILTERS_FOLDER stored_filters);


/**
 * \brief 
 * @param 
 */
void get_filters_list(FILTER* stored_filters);


/**
 * \brief 
 * @param 
 */
void print_asked_filters(int argc, char** argv);



FILTERS_FOLDER store_filters_folder(char* filepath);
void free_filters_folder(FILTERS_FOLDER stored);
FILTER* get_filters(FILTERS_FOLDER ff);
int get_number_of_filters(FILTERS_FOLDER ff);
char** get_filters_for_user(FILTERS_FOLDER ff, unsigned int* n);

unsigned int get_maximum_filter(char* filter, FILTERS_FOLDER ff);
unsigned int get_running_filter(char* filter, FILTERS_FOLDER ff);

char* get_executableName(char* filter_id, FILTERS_FOLDER ff);

void update_used(char* filtername, FILTERS_FOLDER ff);
void update_unused(char* filtername, FILTERS_FOLDER ff);
int* validate_availability(int argc, char** argv, FILTERS_FOLDER ff);

#endif