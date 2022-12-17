#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>

#include "central.h"

using namespace std;

const string localhost = "127.0.0.1";
const string client_port_A = "25779";
const string client_port_B = "26779";
const string central_udp_port = "24779";
const string server_port_T = "21779";
const string server_port_S = "22779";
const string server_port_P = "23779";

int main()
{
    //create all the server
    int server_tcp_A = init_tcp_server(client_port_A);
    int server_tcp_B = init_tcp_server(client_port_B);
    int server_udp_C = init_udp_server(central_udp_port);
    printf("The Central server is up and running.\n");

    struct sockaddr_in client_addr_A;
    socklen_t addr_len_A = sizeof(client_addr_A);
    struct sockaddr_in client_addr_B;
    socklen_t addr_len_B = sizeof(client_addr_B);
    for(;;)
    {
        //accept the tcp socket from clientA
        char read_buf[32768];
        int clientA = accept(server_tcp_A,(struct sockaddr*)&client_addr_A,&addr_len_A);
        if(clientA < 0)
        {
            fprintf(stderr,"accept client A error\n");
            return 1;
        }
        //read the username of clientA
        size_t read_bytes = recv(clientA,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"read client A error\n");
            return 1;
        }
        string usernameA(read_buf,read_bytes);
        printf("The Central server received input=\"%s\" from the client using TCP over port %s\n",usernameA.c_str(),client_port_A.c_str());
        //accept the tcp socket from clientB
        int clientB = accept(server_tcp_B,(struct sockaddr*)&client_addr_B,&addr_len_B);
        if(clientB < 0)
        {
            fprintf(stderr,"accept client B error\n");
            return 1;
        }
        //read the username of clientB
        read_bytes = recv(clientB,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"read client B error\n");
            return 1;
        }
        string usernameB(read_buf,read_bytes);
        printf("The Central server received input=\"%s\" from the client using TCP over port %s\n",usernameB.c_str(),client_port_B.c_str());
        string data_to_server_T = usernameA + "!" + usernameB;
        //connect to the serverT with udp
        int udp_fd_T = init_udp_connect(localhost,server_port_T);
        if(udp_fd_T < 0)
        {
            fprintf(stderr,"udp connect to serverT error\n");
            return 1;
        }
        size_t send_bytes = send(udp_fd_T,data_to_server_T.c_str(),data_to_server_T.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"send data to serverT error\n");
            return 1;
        }
        printf("The Central server sent a request to Backend-Server T.\n");
        read_bytes = recv(server_udp_C,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"recv data from serverT error\n");
            return 1;
        }
        printf("The Central server received information from Backend-Server T using UDP over port %s.\n",server_port_T.c_str());
        //connect to the serverS with udp
        int udp_fd_S = init_udp_connect(localhost,server_port_S);
        if(udp_fd_S < 0)
        {
            fprintf(stderr,"udp connect to serverT error\n");
            return 1;
        }
        string data_to_server_S(read_buf,read_bytes);
        //cout << data_to_server_S << endl;
        //if the received data is wrong
        //the username is not found or usernames are not linked
        if((read_bytes == 5 && data_to_server_S == "wrong") || usernameA == usernameB)
        {
            //send the usernames to clientA and clientB
            string result = usernameB+"!"+usernameA;
            send(clientA,data_to_server_T.c_str(),data_to_server_T.size(),0);
            send(clientB,result.c_str(),result.size(),0);
            close(clientA);
            close(clientB);
            close(udp_fd_T);
            continue;
        }
        //just send the data receive from serverT to serverS
        //serverS will handle the split
        send_bytes = send(udp_fd_S,data_to_server_S.c_str(),data_to_server_S.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"send data to serverS error\n");
            return 1;
        }
        printf("The Central server sent a request to Backend-Server S.\n");
        read_bytes = recv(server_udp_C,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"recv data from serverS error\n");
            return 1;
        }
        printf("The Central server received information from Backend-Server S using UDP over port %s.\n",server_port_S.c_str());
        //connect to the serverP with udp
        int udp_fd_P = init_udp_connect(localhost,server_port_P);
        if(udp_fd_P < 0)
        {
            fprintf(stderr,"udp connect to serverT error\n");
            return 1;
        }
        string data_to_server_P(read_buf,read_bytes);
        //add the usernames to the data receive from serverS
        //and send it to serverP
        //serverP will handle the split and calculate
        data_to_server_P = data_to_server_T + "#"+data_to_server_P;
        send_bytes = send(udp_fd_P,data_to_server_P.c_str(),data_to_server_P.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"send data to serverP error\n");
            return 1;
        }
        printf("The Central server sent a processing request to Backend-Server P.\n");
        //receive data from serverP
        read_bytes = recv(server_udp_C,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"recv data from serverP error\n");
            return 1;
        }
        printf("The Central server received the results from backend server P.\n");
        //send the result to clientA and clientB
        string data_to_client_A(read_buf,read_bytes);
        send_bytes = send(clientA,data_to_client_A.c_str(),data_to_client_A.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"send data to clientA error\n");
            return 1;
        }
        printf("The Central server sent the results to client A.\n");
        string data_to_client_B(read_buf,read_bytes);
        send_bytes = send(clientB,data_to_client_B.c_str(),data_to_client_B.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"send data to clientB error\n");
            return 1;
        }
        printf("The Central server sent the results to client B.\n");
        //close the udp and client's tcp
        close(clientA);
        close(clientB);
        close(udp_fd_T);
        close(udp_fd_S);
        close(udp_fd_P);
    }

    close(server_tcp_A);
    close(server_tcp_B);
    close(server_udp_C);
}