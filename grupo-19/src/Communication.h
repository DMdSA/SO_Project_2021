#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#define Client_Server "/tmp/ClientToServer"								//Isto tem de ser alterado para temp/ (!!!)
#define Server_Client "/tmp/ServerToClient"								//Isto tem de ser alterado para temp/ (!!!)



void read_from_server();

int write_to_cliente(char* something);

int server_client_input(char* program_name, int argc, char** argv, FILTERS_FOLDER ff, int fifo);

void simple_error_handler(int error, char* msg);
void ctrl_c_handler(int signal);



#endif