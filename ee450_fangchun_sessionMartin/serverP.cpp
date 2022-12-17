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

#include "serverP.h"

using namespace std;

const string localhost = "127.0.0.1";
const string central_udp_port = "24779";
const string server_port_P = "23779";

//split the data and use it to create the graph
void parse_graph(Graph& graph,string data);

int main()
{
    printf("The ServerP is up and running using UDP on port %s\n", server_port_P.c_str());

    int server_fd_P = init_udp_server(server_port_P);
    int central_fd = init_udp_connect(localhost,central_udp_port);
    for(;;)
    {
        char read_buf[32768];
        size_t read_bytes = recv(server_fd_P,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"serverP recv error\n");
            continue;
        }
        printf("The ServerP received the topology and score information.\n");
        string data(read_buf,read_bytes);
        //split the read data,and create the graph
        vector<string> datas = split(data,"#");
        vector<string> names = split(datas[0],"!");
        Graph graph;
        parse_graph(graph,datas[1]);
        //add the weight to the graph
        graph.init_weighted_edges();
        //get the shortest path and send back to central as the result
        string result = graph.find_shortest_path(names[0],names[1]);
        size_t send_bytes = send(central_fd,result.c_str(),result.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"serverP send error\n");
            break;
        }
        printf("The ServerP finished sending the results to the Central.\n");
    }
    close(central_fd);
    close(server_fd_P);
}

//split the data and use it to create the graph
void parse_graph(Graph& graph,string data)
{
    vector<string> pieces = split(data,"!");
    for(size_t i = 0;i < pieces.size();++i)
    {
        vector<string> edges = split(pieces[i],":");
        vector<string> nodes = split(edges[1],",");
        for(size_t j = 0;j < nodes.size();++j)
        {
            graph.add_single_edge(edges[0],nodes[j]);
        }
        graph.set_scores(edges[0],stoi(edges[2]));
    }
}
