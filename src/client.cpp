#include "../include/global.h"
#include "../include/logger.h"

#define STDIN 0
#define MAX_INPUT_SIZE 65535

int client_socket_fd;
struct addrinfo client_hints;
struct addrinfo * client_addrinfo;
string client_ip;
int client_port;
char client_hostname[256];

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

        string command;
        bool client_login_status = false;

        for(;;){
            printf("[PA1-Client@CSE489/589]$ ");

            fd_set client_readfds;
            FD_ZERO( & client_readfds);
            if(!client_login_status){
                FD_SET(STDIN, & client_readfds);
            }
            else{
                FD_SET(client_socket_fd, & client_readfds);
                FD_SET(STDIN, & client_readfds);
            }

            int fdmax = client_login_status ?  client_socket_fd : 0;
            int select_status = select(fdmax+1, &client_readfds, NULL, NULL, NULL);

            if(select_status == -1){
                perror("Error in executing select()...\n");
                return -1;
            }

            // Handle Standard Input
            if(FD_ISSET(STDIN, &client_readfds)){
                getline(cin, command);
                cout << "You Entered: " << command << endl;
                vector<string> split_command = split_string(command, " ");

                if (split_command[0] == "AUTHOR"){
                    print_log_success(command);
                    string ubitname = "rahulyad";
                    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", ubitname.c_str());
                    print_log_end(command);
                } 
                else if (split_command[0] == "IP"){
                    client_ip = set_ip();
                    // if(client_ip == NULL){
                    //     perror("Unable to set IP...");
                    //     return -1;
                    // }
                    print_log_success("IP");
                    cse4589_print_and_log("IP:%s\n", client_ip.c_str());
                    print_log_end("IP");
                }
                else if (split_command[0] == "PORT"){
                    client_port = string_to_int(port_number);
                    print_log_success("PORT");
                    cse4589_print_and_log("PORT:%d\n", client_port);
                    print_log_end("PORT");
                }
                else if (split_command[0] == "LIST"){
                    printf("List is not implemented yet\n");
                }
                else if (split_command[0] == "LOGIN" && !client_login_status){
                    cout<<"split_command[0]= "<<split_command[0]<<endl;
                    cout<<"split_command[1]= "<<split_command[1]<<endl;
                    cout<<"split_command[2]= "<<split_command[2]<<endl;
                    int server_getaddrinfo_status = getaddrinfo(split_command[1].c_str(), split_command[2].c_str(), &client_hints, &client_addrinfo);
                    if(server_getaddrinfo_status != 0){
                        perror("Error in executing getaddrinfo_status() for server from client...\n");
                        return -1;
                    }

                    int connect_status = connect(client_socket_fd, client_addrinfo->ai_addr, client_addrinfo->ai_addrlen);
                    if(connect_status == -1){
                        perror("Unable to connect()...\n");
                        close(client_socket_fd);
                        return -1;
                    }

                    freeaddrinfo(client_addrinfo);

                    gethostname(client_hostname, sizeof(client_hostname) - 1);
                    string message = "LOGIN " + string(client_hostname) + " " + client_ip + " " + port_number;
                    cout<<"message prepared by client= "<<message<<endl;
                    int send_status = send(client_socket_fd, (const char*)message.c_str(), message.length(), 0);
                    if(send_status <= 0){
                        perror("Unable to send() message");
                        return -1;
                    }
                    cout<< send_status << endl;

                    client_login_status = true;
                    printf("LOGIN Successful!!\n");
                }
            }

            // Handle connecting request & listen
            if(FD_ISSET(client_socket_fd, &client_readfds)){
                printf("Inside Client Listener\n");
                return -1;
            }

            fflush(stdout);
        }
    }

    // END
    return 0;
}