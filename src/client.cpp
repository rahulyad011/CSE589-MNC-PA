#include "../include/global.h"
#include "../include/logger.h"

int client_socket_fd;
struct addrinfo client_hints;
struct addrinfo * client_addrinfo;
string client_ip;
int client_port;

int client_initialization(string port_number){
    const char *client_port_number_pointer = port_number.c_str(); 
    
    memset(&client_hints, 0, sizeof(client_addrinfo));
    client_hints.ai_family = AF_INET;
    client_hints.ai_socktype = SOCK_STREAM;
    client_hints.ai_flags = AI_PASSIVE;

    int getaddrinfo_status = getaddrinfo(NULL, client_port_number_pointer, &client_hints, &client_addrinfo);
    if(getaddrinfo_status != 0){
        printf("Error in executing getaddrinfo_status()...\n");
        return -1;
    }

    client_socket_fd = socket(client_addrinfo->ai_family, client_addrinfo->ai_socktype, client_addrinfo->ai_protocol);
    if(client_socket_fd < 0){
        printf("Error in executing socket()...\n");
        return -1;
    }

    int bind_status = bind(client_socket_fd, client_addrinfo->ai_addr, client_addrinfo->ai_addrlen);
    if(bind_status < 0){
        printf("Error in executing bind()...\n");
        return -1;
    }

    freeaddrinfo(client_addrinfo);
    return 0;
}


int client(string port_number){
    printf("Initialising client\n");
    
    int client_initialization_status;
    client_initialization_status = client_initialization(port_number);

    if(client_initialization_status == -1){
        printf("Error in client initialization. Closing application...\n");
        return -1;
    }
    else if(client_initialization_status == 0){
        printf("client is Alive\n");
        fd_set client_readfds;

        for(;;){
            printf("[PA1-Client@CSE489/589]$ ");

            FD_ZERO(&client_readfds);
            fflush(stdout);
            FD_SET(STDIN, &client_readfds);
            select(STDIN+1, &client_readfds, NULL, NULL, NULL);
            
            if(FD_ISSET(STDIN, &client_readfds)){
                char *command = (char*) malloc(sizeof(char)*MAX_INPUT_SIZE);
                memset(command, '\0', MAX_INPUT_SIZE);
                if(fgets(command, MAX_INPUT_SIZE, stdin) == NULL){
                    return -1;
                }
                string command_str = command;
                if(command[strlen(command) - 1] == '\n')
                {
                    command_str = command_str.substr(0, command_str.length() - 1);
                }
                vector<string> split_cmd = split_string(command_str, " ");
                fflush(stdin);

                if (split_cmd[0] == "AUTHOR"){
                    printf("Author is not implemented yet\n");
                } 
                else if (split_cmd[0] == "IP"){
                    client_ip = set_ip();
                    // if(client_ip == NULL){
                    //     perror("Unable to set IP...");
                    //     return -1;
                    // }
                    print_log_success("IP");
                    cse4589_print_and_log("IP:%s\n", client_ip.c_str());
                    print_log_end("IP");
                }
                else if (split_cmd[0] == "PORT"){
                    client_port = string_to_int(port_number);
                    cout << "Client port: " << client_port << endl;
                    printf("Port is not implemented yet\n");
                }
                else if (split_cmd[0] == "LIST"){
                    printf("List is not implemented yet\n");
                }
            }
            else
                printf("Timed out...\n");
        }
    }

    // END
    return 0;
}