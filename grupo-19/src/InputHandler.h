#ifndef INPUTHANDLER_HEADER
#define INPUTHANDLER_HEADER



int confirm_transform_input(char* program_name, int argc, char** argv);


char** string_to_args(char* string_input, int* n_args);


int client_input(int argc, char** argv);





#endif