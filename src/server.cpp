#include "../include/global.h"
#include "../include/logger.h"

#define STDIN 0
#define MAX_INPUT_SIZE 65535

int server_socket_fd;
struct addrinfo server_hints;
struct addrinfo * server_addrinfo;
string server_ip;
int server_port;

int server_initialization(string port_number){
    const char *server_port_number_pointer = port_number.c_str(); 

    memset(&server_hints, 0, sizeof(server_addrinfo));
    server_hints.ai_family = AF_INET;
    server_hints.ai_socktype = SOCK_STREAM;
    server_hints.ai_flags = AI_PASSIVE;

    int getaddrinfo_status = getaddrinfo(NULL, server_port_number_pointer, &server_hints, &server_addrinfo);
    if(getaddrinfo_status != 0){
        printf("Error in executing getaddrinfo_status()...\n");
        return -1;
    }

    server_socket_fd = socket(server_addrinfo->ai_family, server_addrinfo->ai_socktype, server_addrinfo->ai_protocol);
    if(server_socket_fd < 0){
        printf("Error in executing socket()...\n");
        return -1;
    }

    int bind_status = bind(server_socket_fd, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen);
    if(bind_status < 0){
        printf("Error in executing bind()...\n");
        return -1;
    }

    freeaddrinfo(server_addrinfo);
    return 0;
}

int server(string port_number){
    printf("Initialising server\n");
    
    int server_initialization_status;
    server_initialization_status = server_initialization(port_number);

    if(server_initialization_status == -1){
        printf("Error in sever initialization. Closing application...\n");
        return -1;
    }
    else if(server_initialization_status == 0){
        int listen_status = listen(server_socket_fd, 10);
        string command;

        if(listen_status == -1){
            printf("Server is unable to listen. Closing application...\n");
            return -1;
        }
        else if(listen_status == 0){
            printf("Server is Alive!\n");
            fd_set server_readfds;
            
            for(;;){
                printf("\n[PA1-Server@CSE489/589]$ ");

                FD_ZERO(&server_readfds);
                fflush(stdout);
                FD_SET(STDIN, &server_readfds);
                select(STDIN+1, &server_readfds, NULL, NULL, NULL);

                if(FD_ISSET(STDIN, &server_readfds)){
                    // char *command = (char*) malloc(sizeof(char)*MAX_INPUT_SIZE);
                    // memset(command, '\0', MAX_INPUT_SIZE);
                    // if(fgets(command, MAX_INPUT_SIZE, stdin) == NULL){
                    //     return -1;
                    // }
                    // string command_str = command;
                    // if(command[strlen(command) - 1] == '\n')
                    // {
                    //     command_str = command_str.substr(0, command_str.length() - 1);
                    // }
                    // vector<string> split_cmd = split_string(command_str, " ");
                    // fflush(stdin);
                    getline(cin, command);
                    cout << "You entered: " << command << endl;

                    if(command == "AUTHOR"){
                        print_log_success(command);
                        string ubitname = "rahulyad";
                        cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", ubitname.c_str());
                        print_log_end(command);
                    }
                    else if(command == "IP"){
                        server_ip = set_ip();
                        print_log_success("IP");
                        cse4589_print_and_log("IP:%s\n", server_ip.c_str());
                        print_log_end("IP");
                    }
                    else if (command == "PORT"){
                        server_port = string_to_int(port_number);
                        print_log_success("PORT");
                        cse4589_print_and_log("PORT:%d\n", server_port);
                        print_log_end("PORT");
                    }
                    else if(command == "LIST"){
                        printf("List is not implemented yet\n");
                    }
                }
            }
        }
    }

    // END
    return 0;
}