#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#define Client_Server "/tmp/ClientToServer"								//Isto tem de ser alterado para temp/ (!!!)
#define Server_Client "/tmp/ServerToClient"								//Isto tem de ser alterado para temp/ (!!!)



/**
 * Simulação do tratamento de um erro
 * @param error valor de retorno
 * @param msg aviso a ser impresso
 */
void simple_error_handler(int error, char* msg);


/**
 * Leitura de mensagens processadas pelo Servidor ao
 */
void read_from_server();


/**
 * Processo de escrita de uma qualquer mensagem para o cliente
 * @param something Mensagem
 * @returns 0, em caso de envio com sucesso
 */
int write_to_cliente(char* something);


/**
 * Tratamento de todos os inputs realizados pelo cliente para o servidor
 * @param tasks Tarefas em execução, no momento
 * @param n_tasks Número de tarefas em execução, no momento
 * @param argc Número de argumentos
 * @param argv Argumentos
 * @param ff Conjunto dos filtros guardados pelo servidor
 * @returns inteiro associado a cada tarefa específica
 *          0 -> pedido 'filters' e 'status'
 *          1 -> pedido 'transform'
 *         -1 -> erro
 */
int server_client_input(char** tasks, int n_tasks, int argc, char** argv, FILTERS_FOLDER ff);


/**
 * Processo de leitura de mensagens do servidor [versão simples]
 * (apenas recebe uma mensagem de cada vez)
 */
void wait_before_leaving();



#endif