#include "../include/global.h"
#include "../include/logger.h"

int server_socket_fd;
struct addrinfo server_hints;
struct addrinfo * server_addrinfo;
string server_ip;

int set_server_ip(){
    struct sockaddr_in server_sockaddr_in;
    memset(&server_sockaddr_in, 0, sizeof(sockaddr_in));
    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = inet_addr("8.8.8.8");
    server_sockaddr_in.sin_port = htons(53);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    connect(server_socket, (const sockaddr*)&server_sockaddr_in, sizeof(sockaddr_in));

    struct sockaddr_in server_sockaddr_in2;
    socklen_t length = sizeof(sockaddr_in);
    getsockname(server_socket, (sockaddr*)&server_sockaddr_in2, &length);
    server_ip = inet_ntop(AF_INET, &server_sockaddr_in2.sin_addr, (char*) malloc(INET_ADDRSTRLEN), INET_ADDRSTRLEN);

    close(server_socket);
    return 0;
}

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
    printf("Initialising Server\n");
    
    int server_initialization_status;
    server_initialization_status = server_initialization(port_number);

    if(server_initialization_status == -1){
        printf("Error in sever initialization. Closing application...\n");
        return -1;
    }
    else if(server_initialization_status == 0){
        int listen_status = listen(server_socket_fd, 10);

        if(listen_status == -1){
            printf("Server is unable to listen. Closing application...\n");
            return -1;
        }
        else if(listen_status == 0){
            printf("Server is Alive!\n");
            
            for(;;){
                printf("Please enter your command:\n");
                string command;
                cin >> command;

                if(command == "AUTHOR"){
                    printf("Author is not implemented yet\n");
                } 
                else if(command == "IP"){
                    int server_ip_status = set_server_ip();
                    if(server_ip_status == -1){
                        printf("Unable to setup server ip...\n");
                        return -1;
                    }
                    cout<<"Server IP: "<<server_ip<<endl;
                }
                else if(command == "LIST"){
                    printf("List is not implemented yet\n");
                }
            }
        }
    }

    // END
    return 0;
}