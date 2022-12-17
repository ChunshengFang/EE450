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
#include <string>
#include <map>
#include <vector>
#include <list>

#include "serverT.h"

using namespace std;

const string localhost = "127.0.0.1";
const string central_udp_port = "24779";
const string server_port_T = "21779";

//read the file and create the graph
void read_graph(Graph& graph);

int main()
{
    Graph graph;
    read_graph(graph);
    int server_fd_T = init_udp_server(server_port_T);
    int central_fd = init_udp_connect(localhost,central_udp_port);
    printf("The ServerT is up and running using UDP on port %s\n", server_port_T.c_str());
    for(;;)
    {
        char read_buf[32768];
        size_t read_bytes = recv(server_fd_T,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"serverT recv error\n");
            continue;
        }
        //the data is usernameA!usernameB
        string data(read_buf,read_bytes);
        vector<string> usernames = split(data,"!");
        printf("The ServerT received a request from Central to get the topology.\n");
        string result = graph.graph_string(usernames[0],usernames[1]);
        //error,two username is not linked or
        //the username is wrong
        if(result.size() == 0)
        {
            result = "wrong";
        }
        //send back wrong or the string of graph to central
        size_t send_bytes = send(central_fd,result.c_str(),result.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"serverT send error\n");
            break;
        }
        printf("The ServerT finished sending the topology to Central.\n");
    }
    close(central_fd);
    close(server_fd_T);
}

//read the file and create the graph
void read_graph(Graph& graph)
{
    FILE* fp = fopen("edgelist.txt","r");
    char buf1[512];
    char buf2[512];
    while(fscanf(fp,"%s %s",buf1,buf2) != EOF)
    {
        string username1 = string(buf1);
        string username2 = string(buf2);
        graph.add_edges(username1,username2);
    }
    fclose(fp);
}
