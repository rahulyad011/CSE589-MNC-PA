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
vector<SocketObject> client_socketlist;

void print_client_List(){
    sort(client_socketlist.begin(), client_socketlist.end());
    print_log_success("LIST");
    int i = 0;
    for (vector<SocketObject>::iterator it = client_socketlist.begin(); it != client_socketlist.end();i++,it++)
    {
        if(it->status == "logged-in")
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, it->hostname.c_str(), it->ip.c_str(), it->port_num);
    }
    print_log_end("LIST");
}

int client_initialization(string port_number){
    const char *client_port_number_pointer = port_number.c_str(); 
    struct addrinfo * p;
    int yes = 1;

    memset(&client_hints, 0, sizeof(client_addrinfo));
    client_hints.ai_family = AF_INET;
    client_hints.ai_socktype = SOCK_STREAM;
    client_hints.ai_flags = AI_PASSIVE;

    int client_getaddrinfo_status = getaddrinfo(NULL, client_port_number_pointer, &client_hints, &client_addrinfo);
    if(client_getaddrinfo_status != 0){
        printf("Error in executing client_getaddrinfo_status()...\n");
        return -1;
    }

    for(p = client_addrinfo; p != NULL; p = p->ai_next) {
        client_socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (client_socket_fd < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(client_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(client_socket_fd, p->ai_addr, p->ai_addrlen) < 0) {
            close(client_socket_fd);
            continue;
        }

        break;
    }

    // client_socket_fd = socket(client_addrinfo->ai_family, client_addrinfo->ai_socktype, client_addrinfo->ai_protocol);
    // if(client_socket_fd < 0){
    //     printf("Error in executing socket()...\n");
    //     return -1;
    // }

    // int bind_status = bind(client_socket_fd, client_addrinfo->ai_addr, client_addrinfo->ai_addrlen);
    // if(bind_status < 0){
    //     printf("Error in executing bind()...\n");
    //     return -1;
    // }
    if (p == NULL) {
        fprintf(stderr, "selectclient: failed to bind\n");
        return -1;
    }

    freeaddrinfo(client_addrinfo);
    return 0;
}

int client(string port_number){
    printf("Initialising client\n");

    client_ip = set_ip();
    
    int client_initialization_status;
    fd_set client_masterfds;
    fd_set client_readfds;
    FD_ZERO( &client_masterfds);
    FD_ZERO( &client_readfds);
    int fdmax, newfd;
    socklen_t addrlen;
    struct sockaddr_storage remoteaddr; // client address
    client_initialization_status = client_initialization(port_number);
    char remoteIP[INET6_ADDRSTRLEN];
    char buf[256];    // buffer for client data
    int nbytes;
    int i, j;
    client_initialization_status = client_initialization(port_number);

    if(client_initialization_status == -1){
        printf("Error in client initialization. Closing application...\n");
        return -1;
    }
    else if(client_initialization_status == 0){
        printf("Client is Alive!\n");
        FD_SET(client_socket_fd, &client_masterfds);
        fdmax = client_socket_fd;
        string command;
        bool client_login_status = false;
        bool client_login_status_local = false;

        for(;;){
            printf("\n[PA1-Client@CSE489/589]$ ");
            fflush(stdout);
            char *incomming_msg = (char*) malloc(sizeof(char)*MESSAGE_LEN);
   	        memset(incomming_msg, '\0', MESSAGE_LEN);

            client_readfds = client_masterfds;
            if(!client_login_status_local){
                FD_SET(STDIN, &client_readfds);
            }
            else{
                FD_SET(client_socket_fd, &client_readfds);
                FD_SET(STDIN, &client_readfds);
            }

            int fdmax = client_login_status_local ?  client_socket_fd : 0;
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
                else if (split_command[0] == "LIST" && client_login_status){
                    print_client_List();
                }
                else if (split_command[0] == "LOGIN" && !client_login_status){
                    cout<<"split_command[0]= "<<split_command[0]<<endl;
                    cout<<"split_command[1]= "<<split_command[1]<<endl;
                    cout<<"split_command[2]= "<<split_command[2]<<endl;

                    string incoming_ip = split_command[1];
                    string incoming_port = split_command[2];

                    bool ip_authenticity = ip_exception_check(incoming_ip);
                    bool port_authenticity = port_exception_check(incoming_port);
                    bool command_length_authenticity = split_command.size() == 3;

                    if(ip_authenticity && port_authenticity && command_length_authenticity){
                        int client_getaddrinfo_status = getaddrinfo(split_command[1].c_str(), split_command[2].c_str(), &client_hints, &client_addrinfo);
                        if(client_getaddrinfo_status != 0){
                            perror("Error in executing client_getaddrinfo_status() for client from client...\n");
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
                            continue;
                        }
                        cout<<"send_status: " <<send_status << endl;
                        cout<<"updating login status of client: "<<endl;
                        client_login_status_local = true;
                        printf("LOGIN Successful!!\n");
                    }
                    else{
                        print_log_error(split_command[0]);
                    }
                }
                else if (split_command[0] == "REFRESH" && client_login_status){
                    printf("REFRESH is selected by the client\n");
                    string message = "REFRESH " + client_ip + " " + port_number;
                    int send_status = send(client_socket_fd, (const char*)message.c_str(), message.length(), 0);

                    printf("Initiating Client Refresh...\n");

                    if(send_status < 0)
                    {
                        perror("Error in sending the Refresh message to server\n");
                    } 
                    else
                    {
                        print_log_success("REFRESH");
                        print_log_end("REFRESH");
                    }
                }
                else if (split_command[0] == "EXIT"){
                    string message = "EXIT " + client_ip + " " + port_number;
                    int send_status = send(client_socket_fd, (const char*)message.c_str(), message.length(), 0);

                    printf("Initiating Client Exit...\n");

                    if(send_status < 0)
                    {
                        perror("Error in sending the exit message to server\n");
                    } 
                    else
                    {
                        client_login_status = false;
                        close(client_socket_fd);
                        print_log_success("EXIT");
                        print_log_end("EXIT");
                    }

                    return -1;
                }
            }

            // Handle connecting request & listen
            if(FD_ISSET(client_socket_fd, &client_readfds)){
                printf("Inside Client Listener\n");

                memset(incomming_msg, '\0', MESSAGE_LEN);
                int im_recv_status = recv(client_socket_fd, incomming_msg, MESSAGE_LEN, 0);

                if(im_recv_status == 0){
                    printf("Not Receiving anything Inside Client Listener so closing connection \n");
                    close(client_socket_fd);
                    client_socket_fd = 0;
                    client_login_status = false;
                }
                else if(im_recv_status < 0){
                    perror("receive");
                    printf("error in receiving \n");
                }
                else{
                    string inmessage_str = incomming_msg;
                    cout<<"message from ser_ver: "<<inmessage_str<<"\n"<<endl;

                    // split each line sent by the ser_ver, it means the detail of each live client
                    vector<string> master_msgs = split_string(inmessage_str, "\n");
                    cout<<"master_msgs size:"<<master_msgs.size()<<endl;

                    for (vector<string>::iterator master_msg = master_msgs.begin(); master_msg != master_msgs.end(); ++master_msg) {
                        //now we need to break the each line corresponding to one live client into hostname, ip and port
                        vector<string> msg_params = split_string(*master_msg, " ");
                        cout<<"msg_params first element: "<<msg_params[0]<<endl;
                        // msg_params[0] = trimString(msg_params[0]);
                        cout<<"msg_param size:"<<msg_params.size()<<endl;
                        if(msg_params.size() == 0){
                            continue;
                        }
                        else{
                            cout << "what is the function" << msg_params[0] << "\n";
                            if (msg_params[0] == "LIST_LOGIN") {
                                //server send the list of all logged-in
                                cout << "recv get LIST_LOGIN\n";
                                if (client_socketlist.size() != 0) client_socketlist.clear();

                                for (vector<string>::size_type j = 1; j + 2 < msg_params.size(); j += 3) {
                                    const string& hostname_t = msg_params[j];
                                    const string& ip_t = msg_params[j + 1];
                                    const string& port_t = msg_params[j + 2];
                                    cout << hostname_t << ", " << ip_t << ", " << port_t << "\n";
                                    cout << "client_socketlist size = " << client_socketlist.size() << "\n";
                                    SocketObject* tmp_head = newSocketObject(-2, hostname_t, ip_t, port_t);
                                    if (tmp_head == NULL) cout << " tmp_head == NULL !  \n";
                                    client_socketlist.push_back(*tmp_head);
                                    cout << "client_socketlist size = " << client_socketlist.size() << "\n";
                                }
                                cout << "printing client_socketlist client side = "<< endl;
                                printSocketList(client_socketlist);
                                if(!client_login_status)
                                {
                                    client_login_status = true;
                                    print_log_success("LOGIN");
                                    print_log_end("LOGIN");
                                }
                            }
                            else{

                            }
                        }
                    }
                }
                // return -1;
            }

            fflush(stdout);
        }
    }

    // END
    return 0;
}