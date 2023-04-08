#include "../include/global.h"
#include "../include/logger.h"

#define STDIN 0
#define MAX_INPUT_SIZE 65535

int server_socket_fd;
struct addrinfo server_hints;
struct addrinfo * server_addrinfo;
string server_ip;
int server_port;

extern vector<SocketObject> socketlist;

int server_initialization(string port_number){
    const char *server_port_number_pointer = port_number.c_str(); 
    struct addrinfo * p;
    int yes = 1;

    memset(&server_hints, 0, sizeof(server_addrinfo));
    server_hints.ai_family = AF_INET;
    server_hints.ai_socktype = SOCK_STREAM;
    server_hints.ai_flags = AI_PASSIVE;

    int server_getaddrinfo_status = getaddrinfo(NULL, server_port_number_pointer, &server_hints, &server_addrinfo);
    if(server_getaddrinfo_status != 0){
        printf("Error in executing server_getaddrinfo_status()...\n");
        return -1;
    }

    for(p = server_addrinfo; p != NULL; p = p->ai_next) {
        server_socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket_fd < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(server_socket_fd, p->ai_addr, p->ai_addrlen) < 0) {
            close(server_socket_fd);
            continue;
        }

        break;
    }

    // server_socket_fd = socket(server_addrinfo->ai_family, server_addrinfo->ai_socktype, server_addrinfo->ai_protocol);
    // if(server_socket_fd < 0){
    //     printf("Error in executing socket()...\n");
    //     return -1;
    // }

    // int bind_status = bind(server_socket_fd, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen);
    // if(bind_status < 0){
    //     printf("Error in executing bind()...\n");
    //     return -1;
    // }

    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        return -1;
    }

    freeaddrinfo(server_addrinfo);
    return 0;
}

int server(string port_number){
    printf("Initialising server\n");

    server_ip = set_ip();
    
    int server_initialization_status;
    fd_set server_masterfds;
    fd_set server_readfds;
    FD_ZERO( &server_masterfds);
    FD_ZERO( &server_readfds);
    int fdmax, newfd;
    socklen_t addrlen;
    struct sockaddr_storage remoteaddr; // client address
    server_initialization_status = server_initialization(port_number);
    char remoteIP[INET6_ADDRSTRLEN];
    char buf[256];    // buffer for client data
    int nbytes;
    int i, j;

    if(server_initialization_status == -1){
        printf("Error in sever initialization. Closing application...\n");
        return -1;
    }
    else if(server_initialization_status == 0){
        int listen_status = listen(server_socket_fd, 5);
        string command;

        if(listen_status == -1){
            perror("Server is unable to listen. Closing application...\n");
            perror("listen");
            return -1;
        }
        else if(listen_status == 0){
            printf("Server is Alive!\n");
            FD_SET(server_socket_fd, &server_masterfds);
            FD_SET(STDIN, &server_masterfds);
            int fdmax = server_socket_fd > STDIN ? server_socket_fd : STDIN;
            // fdmax = server_socket_fd;
            for(;;){
                printf("\n[PA1-Server@CSE489/589]$ ");
                fflush(stdout);

                server_readfds = server_masterfds;
                // cout<<"server_masterfds = ";
                // print_fd_set(server_masterfds);
                // cout<<endl;

                // cout<<"server_readfds = ";
                // print_fd_set(server_readfds);
                // cout<<endl;

                int select_status = select(fdmax+1, &server_readfds, NULL, NULL, NULL);

                if(select_status == -1){
                    perror("Error in executing select()...\n");
                    return -1;
                }

                int index;
                for(index=0; index <= fdmax; index++){
                    if(FD_ISSET(index, &server_readfds)){

                        cout<<"server_readfds = ";
                        print_fd_set(server_readfds);
                        cout<<endl;

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
                            addrlen = sizeof remoteaddr;
                            newfd = accept(server_socket_fd, (struct sockaddr *)&remoteaddr, &addrlen);

                            if (newfd == -1) {
                                perror("accept");
                            } else {
                                FD_SET(newfd, &server_masterfds); // add to master set
                                if (newfd > fdmax) {    // keep track of the max
                                    fdmax = newfd;
                                }
                                printf("selectserver: new connection from %s on socket %d\n", 
                                        inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP, INET6_ADDRSTRLEN),
                                        newfd);
                            }

                            // return -1;
                        } 
                        else
                        {
                            // handle data from a client
                            if ((nbytes = recv(index, buf, sizeof buf, 0)) <= 0) {
                                // got error or connection closed by client
                                if (nbytes == 0) {
                                    // connection closed
                                    printf("selectserver: socket %d hung up\n", index);
                                } else {
                                    perror("recv");
                                }
                                close(index); // bye!
                                FD_CLR(index, &server_masterfds); // remove from master set
                            } 
                            else 
                            {
                                // // we got some data from a client
                                // for(j = 0; j <= fdmax; j++) {
                                //     // send to everyone!
                                //     if (FD_ISSET(j, &server_masterfds)) {
                                //         // except the listener and ourselves
                                //         if (j != server_socket_fd && j != index) {
                                //             if (send(j, buf, nbytes, 0) == -1) {
                                //                 perror("send");
                                //                 printf("send error");
                                //             }
                                //         }
                                //     }
                                // }

                                string client_command = buf;
                                cout<<"client_command= "<<client_command<<endl;
                                vector<string> split_client_command = split_string(client_command, " ");

                                if(split_client_command[0] == "LOGIN"){
                                    string incomming_ip = split_client_command[2];
                                    SocketObject* currentSocketObject = is_exist_Socket(incomming_ip);

                                    // Existing Client
                                    if(currentSocketObject != NULL){
                                        currentSocketObject->status = "logged-in";
                                    }
                                    // New Client, Add it in Socket List
                                    else{
                                        currentSocketObject = newSocketObject(newfd, split_client_command[1], split_client_command[2], split_client_command[3]);
                                        socketlist.push_back(*currentSocketObject);
                                        cout<<"Client with HOSTNAME= "<< split_client_command[1]<<", IP= "<< split_client_command[2]<<", PORT= "<<split_client_command[3]<<" Logged In Successfully"<<endl;
                                    }

                                    // Inform the client about existing live conections
                                    string message = "LIST_LOGIN ";
                                    for (vector<SocketObject>::iterator it = socketlist.begin(); it != socketlist.end(); ++it) {
                                        if (it->status == "logged-in") {
                                            message += it->hostname + " " + it->ip + " " + it->port + " ";
                                        }
                                        message += "\n";
                                    }
                                    

                                    int ll_send_status = send(currentSocketObject->cfd, message.c_str(), strlen(message.c_str()), 0);
                                    if(ll_send_status <= 0){
                                        perror("send");
                                    }
                                    else{
                                        printf("List Login Info Successfully Sent\n");
                                    }
                                }
                                else if(split_client_command[0] == "REFRESH"){

                                }
                                else if(split_client_command[0] == "EXIT"){

                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            perror("Server listner not in any category. Closing application...\n");
            return -1;
        }
    }
    // END
    else{
        printf("not in any category so close the server..\n");
        return -1;
    }
    return 0;
}