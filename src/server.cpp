#include "../include/global.h"
#include "../include/logger.h"

#define STDIN 0
#define MAX_INPUT_SIZE 65535

int server_socket_fd;
struct addrinfo server_hints;
struct addrinfo * server_addrinfo;
string server_ip;
int server_port;

class localClass{
    public:
        map<string, int> command_map;
        int cfd;
        int port_num;
        int total_connections;
        int num_msg_sent;
        int num_msg_rcv;
        vector<string> commands;
        string status;
        string ip;
        map<string, int>::iterator it;
        string port;
        bool flag;
        string hostname;
        vector<string> blockeduser;
        vector<string> msgbuffer;
        localClass(int cfd, string hostname, string ip, string port);
};

localClass::localClass(int cfd, string hostname, string ip, string port){
    this->cfd = cfd;
    this->ip = ip;
    this->port = port;
    status = "logged-in";
    this->hostname = hostname;
    port_num = string_to_int(port);
    num_msg_sent = 0;
    num_msg_rcv = 0;

    command_map.insert(pair<string, int>("AUTHOR", 1));
    command_map.insert(pair<string, int>("IP", 2));
    command_map.insert(pair<string, int>("PORT", 3));
    command_map.insert(pair<string, int>("LIST", 4));
    command_map.insert(pair<string, int>("REFRESH", 5));
    command_map.insert(pair<string, int>("EXIT", 6));
    command_map.insert(pair<string, int>("LOGIN", 7));

    for (it = command_map.begin(); it != command_map.end(); it++) {
        commands.push_back(it->first);
    }
}

void update_msg_statistics(SocketObject* source_sc, SocketObject* destination_sc){
    destination_sc->num_msg_rcv = destination_sc->num_msg_rcv + 1;
    source_sc->num_msg_sent = source_sc->num_msg_sent + 1;
}

vector<SocketObject> server_socketlist;

SocketObject* newSocketObject(int cfd, string hostname, string ip, string port) 
{
    SocketObject* hd = new SocketObject;
    localClass* lc = new localClass(cfd, hostname, ip, port);
    hd->cfd = lc->cfd;
    hd->ip = lc->ip;
    hd->port = lc->port;
    hd->status = lc->status;
    hd->hostname = lc->hostname;
    hd->port_num = lc->port_num;
    hd->num_msg_sent = lc->num_msg_sent;
    hd->num_msg_rcv = lc->num_msg_rcv;
    delete lc;
    return hd;
}

SocketObject* server_find_socket(int cfd, string ip) 
{
    vector<SocketObject>::iterator it;
    for (it = server_socketlist.begin(); it != server_socketlist.end(); ++it) {
        if ((cfd != -1 && it->cfd == cfd) || (ip != "" && it->ip == ip)) {
            return &(*it);
        }
    }
    return NULL;
}

struct KeyComparator {
    bool operator()(const std::string& a, const std::string& b) const {
        return string_to_int(a) < string_to_int(b);
    }
};

int print_List(string command, string requested_ip){
    if(command == "LIST" && requested_ip == "NONE"){
        sort(server_socketlist.begin(), server_socketlist.end());
        print_log_success("LIST");
        int i = 0;
        for (vector<SocketObject>::iterator it = server_socketlist.begin(); it != server_socketlist.end();i++,it++)
        {
            if(it->status == "logged-in")
                cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i + 1, it->hostname.c_str(), it->ip.c_str(), it->port_num);
        }
        print_log_end("LIST");
    }
    else if(command == "BLOCKED"){
        if(ip_exception_check(requested_ip)){
            if(server_find_socket(-1, requested_ip)){
                SocketObject *requested_socket = server_find_socket(-1, requested_ip);

                map<string, SocketObject> unsortedMap;

                for (vector<string>::iterator it = requested_socket->blockeduser.begin(); it != requested_socket->blockeduser.end();it++)
                {
                    SocketObject *element_socket = server_find_socket(-1,*it);
                    if(element_socket == NULL){
                        printf("Error in finding blocked clients...\n"); 
                        return -1;
                    }
                    unsortedMap.insert(std::make_pair(element_socket->port, *element_socket));
                }

                std::map<std::string, SocketObject, KeyComparator> sortedMap(unsortedMap.begin(), unsortedMap.end());

                print_log_success("BLOCKED");
                int i=0;
                for (std::map<std::string, SocketObject, KeyComparator>::const_iterator it = sortedMap.begin(); it != sortedMap.end(); i++, it++) {
                    const SocketObject& element_socket = it->second;
                    cse4589_print_and_log("%-5d%-35s%-20s%-8s\n", i + 1, element_socket.hostname.c_str(), element_socket.ip.c_str(), element_socket.port.c_str());
                }
                print_log_end("BLOCKED");
            }
            else{
                // print_log_error("BLOCKED");
                printf("Server can't find the socket related to the requested ip...\n");
                return -1;
            }
        }
        else{
            // print_log_error("BLOCKED");
            printf("Invalid requested IP...\n");            
            return -1;
        }
    }
    else{
        printf("incorrect command to print list\n");
    }

    return 0;
}

void print_server_Statistics(){
    sort(server_socketlist.begin(), server_socketlist.end());
    print_log_success("STATISTICS");
    int i = 0;
    for (vector<SocketObject>::iterator it = server_socketlist.begin(); it != server_socketlist.end();i++,it++)
    {
        cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i + 1, it->hostname.c_str(), it->num_msg_sent, it->num_msg_rcv, it->status.c_str());
    }
    print_log_end("STATISTICS");
}

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
    memset(buf, '\0', sizeof(buf));
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

                            vector<string> split_command = split_string(command, " ");

                            if(split_command.size() == 2){
                                if(split_command[0] == "BLOCKED"){
                                    string requested_ip = split_command[1];
                                    int print_status = print_List("BLOCKED",requested_ip);
                                    if(print_status == -1){
                                        print_log_error("BLOCKED");
                                    }
                                }
                                else{
                                    perror("BLOCKED");
                                    printf("Incorrect command...\n");
                                }
                            }
                            else if(command == "AUTHOR"){
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
                                print_List("LIST", "NONE");
                            }
                            else if(command == "STATISTICS"){
                                print_server_Statistics();
                            }
                            else{
                                printf("Incorrect command...\n");
                            }
                        }
                        // listening from new connections
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
                        // listening from existing connections
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
                                string client_command = buf;
                                cout<<"buf= "<<buf<<endl;
                                vector<string> split_client_command;
                                split_client_command = split_string(client_command, " ");
                                memset(buf, '\0', sizeof(buf));
                                cout<<"client_command= "<<client_command<<endl;

                                if(split_client_command[0] == "LOGIN"){
                                    SocketObject* currentSocketObject = server_find_socket(-1, split_client_command[2]);
                                    // Existing Client
                                    if(currentSocketObject != NULL){
                                        currentSocketObject->status = "logged-in";
                                    }
                                    // New Client, Add it in Socket List
                                    else{
                                        currentSocketObject = newSocketObject(newfd, split_client_command[1], split_client_command[2], split_client_command[3]);
                                        server_socketlist.push_back(*currentSocketObject);
                                        cout<<"Client with HOSTNAME= "<< split_client_command[1]<<", IP= "<< split_client_command[2]<<", PORT= "<<split_client_command[3]<<" Logged In Successfully"<<endl;
                                    }

                                    // Inform the client about existing live conections
                                    string message = "LIST_LOGIN ";
                                    for (vector<SocketObject>::iterator it = server_socketlist.begin(); it != server_socketlist.end(); ++it) {
                                        if (it->status == "logged-in") {
                                            message += it->hostname + " " + it->ip + " " + it->port + " ";
                                        }
                                    }
                                    message += "\n";

                                    cout<<"LIST_LOGIN message: "<<message<<endl;

                                    int ll_send_status = send(currentSocketObject->cfd, message.c_str(), strlen(message.c_str()), 0);
                                    if(ll_send_status <= 0){
                                        perror("send");
                                    }
                                    else{
                                        printf("List Login Info Successfully Sent\n");
                                    }
                                }
                                else if(split_client_command[0] == "LOGOUT"){
                                    string incoming_ip = split_client_command[1];
                                    printf("logout requested by client ip:%s \n", incoming_ip.c_str());
                                    SocketObject* currentSocketObject = server_find_socket(-1, incoming_ip);
                                    if(currentSocketObject == NULL){
                                        printf("client fd not found on this IP \n");
                                        return -1;
                                    }
                                    else{
                                        currentSocketObject->status = "logged-out";
                                        printf("Client set to Logout succesful by server...\n");
                                    }
                                }
                                else if(split_client_command[0] == "REFRESH"){
                                    string incoming_ip = split_client_command[1];
                                    printf("refresh requested by client ip:%s \n", incoming_ip.c_str());
                                    SocketObject* currentSocketObject = server_find_socket(-1, incoming_ip);
                                    if(currentSocketObject == NULL){
                                        printf("client fd not found on this IP \n");
                                        return -1;
                                    }
                                    else{
                                        // Inform the client about existing live conections
                                        string message = "LIST_LOGIN ";
                                        for (vector<SocketObject>::iterator it = server_socketlist.begin(); it != server_socketlist.end(); ++it) {
                                            if (it->status == "logged-in") {
                                                message += it->hostname + " " + it->ip + " " + it->port + " ";
                                            }
                                        }
                                        message += "\n";

                                        cout<<"LIST_LOGIN from REFRESH message: "<<message<<endl;

                                        int ll_send_status = send(currentSocketObject->cfd, message.c_str(), strlen(message.c_str()), 0);
                                        if(ll_send_status <= 0){
                                            perror("send");
                                        }
                                        else{
                                            printf("Refreshed List Login Info Successfully Sent\n");
                                        }
                                        cout<< "server_socketlist.size() after refresh = "<< server_socketlist.size() << endl;                
                                    }
                                    
                                }
                                else if(split_client_command[0] == "SEND"){
                                    string source_ip = split_client_command[1];
                                    string destination_ip = split_client_command[2];

                                    printf("send requested by source_ip:%s \n", source_ip.c_str());
                                    printf("send requested to destination_ip:%s \n", destination_ip.c_str());

                                    SocketObject* source_SocketObject = server_find_socket(-1, source_ip);
                                    SocketObject* destination_SocketObject = server_find_socket(-1, destination_ip);

                                    if(source_SocketObject == NULL){
                                        printf("source_SocketObject not found on this IP \n");
                                        continue;
                                    }
                                    if(destination_SocketObject == NULL){
                                        printf("destination_SocketObject not found on this IP \n");
                                        continue;
                                    }

                                    if(check_blocked(destination_SocketObject->blockeduser, source_ip)){
                                        cout<<"source_ip: "<<source_ip << " blocked by destination_ip: "<<destination_ip<<endl;
                                        continue;
                                    }

                                    update_msg_statistics(source_SocketObject, destination_SocketObject);

                                    vector<string>::iterator it = split_client_command.begin();
                                    it++; // Skip "SEND" word
                                    it++; // Skip <source IP>
                                    it++; // Skip <destination IP>

                                    string message = "SEND " + source_ip + " " + destination_ip + " " +  *it;
                                    it++;
                                    for (; it != split_client_command.end();it++)
                                    {
                                        message += " " + *it;
                                    }

                                    // check if logged in
                                    if(destination_SocketObject->status == "logged-in")
                                    {
                                        int send_status = send(destination_SocketObject->cfd, (const char*)message.c_str(),message.length(), 0);
                                        if(send_status < 0){
                                            perror("Unable to send() to logged-in client..\n");
                                        }
                                        else{
                                            print_log_success("RELAYED");
                                            cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", source_ip.c_str(), destination_ip.c_str(), message.c_str());
                                            print_log_end("RELAYED");
                                        }
                                    }else{
                                        // if not logged-in, then buffer the message
                                        destination_SocketObject->msgbuffer.push_back(message);
                                        printf("Message pushed into respective buffer for logged out client\n");
                                    }
                                }
                                else if(split_client_command[0] == "EXIT"){
                                    cout<< "server_socketlist.size() before erase = "<< server_socketlist.size() << endl;
                                    for (vector<SocketObject>::iterator it = server_socketlist.begin(); it != server_socketlist.end();)
                                    {
                                        if (it->cfd == index)
                                        {
                                            printf("client fd found, deleteing it from the server_socketlist...\n");
                                            it = server_socketlist.erase(it);
                                        }
                                        else
                                        {
                                            ++it;
                                        }
                                    }
                                    cout<< "server_socketlist.size() after erase = "<< server_socketlist.size() << endl;
                                }
                                else if (split_client_command[0] == "BLOCK"){
                                    string source_ip = split_client_command[1];
                                    string destination_ip = split_client_command[2];

                                    printf("block requested by source_ip:%s \n", source_ip.c_str());
                                    printf("block requested to destination_ip:%s \n", destination_ip.c_str());

                                    SocketObject* source_SocketObject = server_find_socket(-1, source_ip);
                                    SocketObject* destination_SocketObject = server_find_socket(-1, destination_ip);

                                    if(source_SocketObject == NULL || !ip_exception_check(source_ip)){
                                        print_log_error("BLOCK");
                                        printf("source_SocketObject not found or invalid IP. Please Check \n");
                                        continue;
                                    }
                                    if(destination_SocketObject == NULL || !ip_exception_check(destination_ip)){
                                        print_log_error("BLOCK");
                                        printf("destination_SocketObject not found or invalid IP. Please Check \n");
                                        continue;
                                    }

                                    vector<string>::iterator it = source_SocketObject->blockeduser.begin();

                                    while(it != source_SocketObject->blockeduser.end()){
                                        if(*it == destination_ip) break;
                                        it++;
                                    }

                                    if(it != source_SocketObject->blockeduser.end()){
                                        printf("User already blocked...\n");
                                        continue;
                                    }
                                    else{
                                        source_SocketObject->blockeduser.push_back(destination_ip);
                                        printf("server successfully blocked the requested user...\n");
                                    }
                                }
                                else if (split_client_command[0] == "UNBLOCK"){
                                    string source_ip = split_client_command[1];
                                    string destination_ip = split_client_command[2];

                                    printf("unblock requested by source_ip:%s \n", source_ip.c_str());
                                    printf("unblock requested to destination_ip:%s \n", destination_ip.c_str());

                                    SocketObject* source_SocketObject = server_find_socket(-1, source_ip);
                                    SocketObject* destination_SocketObject = server_find_socket(-1, destination_ip);

                                    if(source_SocketObject == NULL || !ip_exception_check(source_ip)){
                                        print_log_error("UNBLOCK");
                                        printf("source_SocketObject not found or invalid IP. Please Check \n");
                                        continue;
                                    }
                                    if(destination_SocketObject == NULL || !ip_exception_check(destination_ip)){
                                        print_log_error("UNBLOCK");
                                        printf("destination_SocketObject not found or invalid IP. Please Check \n");
                                        continue;
                                    }

                                    vector<string>::iterator it = source_SocketObject->blockeduser.begin();

                                    while(it != source_SocketObject->blockeduser.end()){
                                        if(*it == destination_ip) break;
                                        it++;
                                    }

                                    if(it != source_SocketObject->blockeduser.end()){
                                        source_SocketObject->blockeduser.erase(it);
                                        printf("server successfully unblocked the requested user...\n");
                                    }
                                    else{
                                        printf("requested client is not present in the blocked list...\n");
                                    }
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