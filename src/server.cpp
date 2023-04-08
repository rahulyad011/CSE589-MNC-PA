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
        int listen_status = listen(server_socket_fd, 5);
        string command;

        if(listen_status == -1){
            printf("Server is unable to listen. Closing application...\n");
            return -1;
        }
        else if(listen_status == 0){
            printf("Server is Alive!\n");
            
            fd_set server_readfds;
            FD_ZERO( & server_readfds);
            FD_SET(server_socket_fd, & server_readfds);
            FD_SET(STDIN, & server_readfds);
            int fdmax = server_socket_fd > STDIN ? server_socket_fd : STDIN;

            for(;;){
                printf("\n[PA1-Server@CSE489/589]$ ");

                fflush(stdout);
                int select_status = select(fdmax+1, &server_readfds, NULL, NULL, NULL);
                if(select_status == -1){
                    perror("Error in executing select()...\n");
                    return -1;
                }

                int index;
                for(index=0; index <= fdmax; index++){
                    if(FD_ISSET(index, &server_readfds)){
                        // Handle Standard Input
                        if(index == STDIN){
                            getline(cin, command);
                            cout << "You Entered: " << command << endl;

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
                        // listening from connections
                        else if(index == server_socket_fd){
                            printf("Inside Server Listener\n");
                            return -1;
                        }
                    }
                }
            }
        }
    }

    // END
    return 0;
}