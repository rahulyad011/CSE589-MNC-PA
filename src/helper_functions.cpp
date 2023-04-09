#include "../include/global.h"
#include "../include/logger.h"

int string_to_int(string str) {
    return atoi(str.c_str());
}

string set_ip(){
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
    string machine_ip = inet_ntop(AF_INET, &server_sockaddr_in2.sin_addr, (char*) malloc(INET_ADDRSTRLEN), INET_ADDRSTRLEN);

    close(server_socket);
    return machine_ip;
}

vector<string> split_string(string input_string, string delimiter) {
    vector<string> tokens;
    size_t pos = 0;
    while ((pos = input_string.find(delimiter)) != string::npos) {
        string token = input_string.substr(0, pos);
        tokens.push_back(token);
        input_string.erase(0, pos + delimiter.length());
    }
    tokens.push_back(input_string);
    if (!tokens.empty() && tokens.back().empty()) {
        tokens.pop_back();
    }
    return tokens;
}

void print_fd_set(fd_set& set) {
    std::cout << "fd_set: ";
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &set)) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// printing client_socketlist or server_socketlist
void printSocketList(const vector<SocketObject>& socketlist) {
    cout << "Socket List:\n";
    // for (vector<SocketObject>::size_type i = 0; i < socketlist.size(); ++i) {
    //     const SocketObject& socket = socketlist[i];
    //     cout << socket.hostname << ", " << socket.ip << ", " << socket.port << "\n";
    // }

    for (vector<SocketObject>::const_iterator it = socketlist.begin(); it != socketlist.end();it++)
    {
        cout << it->hostname << ", " << it->ip << ", " << it->port << "\n";
    }
}

string trimString(string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos != first) {
        size_t last = str.find_last_not_of(' ');
        str = str.substr(first, (last - first + 1));
    }
    else {
        str.clear();
    }
    return str;
}
bool ip_exception_check(const string& ip_addr) {
    int num_dots = 0;
    int num = 0;

    for (int i = 0; i < ip_addr.length(); i++) {
        char c = ip_addr[i];
        if (c == '.') {
            if (++num_dots > 3 || num > 255) {
                return false; // too many dots or number out of range
            }
            num = 0;
        } else if (!isdigit(c)) {
            return false; // non-digit character in the IP address
        } else {
            num = num * 10 + (c - '0');
        }
    }

    if (num_dots != 3 || num > 255) {
        return false; // not enough dots or last number out of range
    }

    return true;
}

bool port_exception_check(const string& port_number) {
    if (port_number.empty()) {
        return false;
    }

    for (int i = 0; i < port_number.length(); i++) {
        if (!isdigit(port_number[i])) {
            return false;
        }
    }

    int port = string_to_int(port_number);
    return (port >= 1024 && port <= 49151);
}