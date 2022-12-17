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

#include <queue> //for find shortest path
#include <iostream>
#include "common.h"

#define LISENQ 20

using namespace std;

//create the tcp socket over port
//bind and listen
//return the server socket
int init_tcp_server(string port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo("localhost", port.c_str(), &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd < 0)
    {
        perror("ServerT: socket");
        exit(1);
    }

    if (::bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        close(sockfd);
        perror("ServerT: bind");
        exit(1);
    }
    if (listen(sockfd, LISENQ) < 0)
    {
        close(sockfd);
        perror("Central: listen");
        exit(1);
    }

    freeaddrinfo(res);
    return sockfd;
}

//create the udp socket over port and bind
//return the server socket
int init_udp_server(string port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo("localhost", port.c_str(), &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd < 0)
    {
        perror("ServerT: socket");
        exit(1);
    }

    if (::bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        close(sockfd);
        perror("ServerT: bind");
        exit(1);
    }

    freeaddrinfo(res);
    return sockfd;
}

//create the tcp socket
//and connect to the server on localhost and port
//use getaddrinfo to get address
//return the socket
int init_tcp_connect(string localhost, string port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(localhost.c_str(), port.c_str(), &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd < 0)
    {
        perror("Client A: socket");
        exit(1);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0)
    {
        close(sockfd);
        perror("Client A: connect");
        exit(1);
    }

    freeaddrinfo(res);
    return sockfd;
}

//create the udp socket
//and connect to the server on localhost and port
//use getaddrinfo to get address
//return the socket
int init_udp_connect(string localhost, string port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if ((status = getaddrinfo(localhost.c_str(), port.c_str(), &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sockfd < 0)
    {
        perror("Client A: socket");
        exit(1);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0)
    {
        close(sockfd);
        perror("Client A: connect");
        exit(1);
    }

    freeaddrinfo(res);
    return sockfd;
}

//split the string s with sp,stores the reuslt in the vector and return
vector<string> split(string s, string sp)
{
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size())
    {
        int flag = 0;
        while (i != s.size() && flag == 0)
        {
            flag = 1;
            for (string_size x = 0; x < sp.size(); ++x)
            {
                if (s[i] == sp[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
            }
        }

        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0)
        {
            for (string_size x = 0; x < sp.size(); ++x)
            {
                if (s[j] == sp[x])
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
                ++j;
        }
        if (i != j)
        {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}

//add the edge from nameA to nameB
void Graph::add_single_edge(string nameA,string nameB)
{
    edges[nameA].push_back(nameB);
}

//add two edge between nameA and nameB
void Graph::add_edges(string nameA,string nameB)
{
    add_single_edge(nameA,nameB);
    add_single_edge(nameB,nameA);
}

//dfs algorithm
void Graph::dfs(string name)
{
    visited[name] = true;
    for(auto begin = edges[name].begin();begin != edges[name].end();++begin)
    {
        if(visited.find(*begin) == visited.end())
        {
            dfs(*begin);
        }
    }
}

//return the string of the graph
//use by serverT to get the graph of nameA
//and nameB
string Graph::graph_string(string nameA,string nameB)
{
    string ret = "";
    if(edges.find(nameA) == edges.end() || edges.find(nameB) == edges.end())
    {
        return ret;
    }
    visited.clear();
    dfs(nameA);
    if(visited.find(nameB) == visited.end())
    {
        return ret;
    }
    for(auto begin = visited.begin();begin != visited.end();++begin)
    {
        ret+= begin->first;
        ret += ":";
        for(auto start = edges[begin->first].begin();start != edges[begin->first].end();++start)
        {
            ret += *start;
            if(start != --(edges[begin->first].end()))
            {
                ret+=",";
            }
        }
        if(begin != (--visited.end()))
        {
            ret+='!';
        }
    }
    return ret;
}

//set the score of the name
void Graph::set_scores(string name,int score)
{
    scores[name] = score;
}

//make the edge of graph has weight
//calculate the weight with abs(a-b)/a+b
void Graph::init_weighted_edges()
{
    for(auto begin = edges.begin();begin != edges.end();++begin)
    {
        for(auto start = begin->second.begin();start != begin->second.end();++start)
        {
            int scoreA = scores[begin->first];
            int scoreB = scores[*start];
            int gap = scoreA > scoreB ? scoreA - scoreB : scoreB - scoreA;
            weighted_edges[begin->first][*start] = gap*1.0/(scoreA+scoreB);
            //cout << begin->first << " " << *start << " " <<  gap*1.0/(scoreA+scoreB) << endl;
        }
    }
}

//get the shortest path of nameA and nameB
//store the path in vector
void Graph::retrieve_path(vector<string>& path,string nameA,string nameB)
{
    if(paths[nameB] != nameA)
    {
        retrieve_path(path,nameA,paths[nameB]);
    }
    else
    {
        path.push_back(nameA);
    }
    path.push_back(nameB);
}

//find the shortest path with belloman_ford algorithm,
//return the string of path and the gap
string Graph::find_shortest_path(string nameA,string nameB)
{
    string result = "";
    distances.clear();
    visited.clear();
    paths.clear();
    bellman_ford(nameA);
    //get the gap of nameA and nameB
    double value = distances[nameB];
    int v = (value+0.005)*100;
    value = v / 100.0;
    vector<string> pathAB;
    retrieve_path(pathAB,nameA,nameB);
    for(size_t i = 0;i < pathAB.size();++i)
    {
        result += pathAB[i];
        if(i != pathAB.size()-1)
        {
            result += "!";
        }
    }
    result += "#";
    result += to_string(value);
    //cout << result << endl;
    return result;
}

//bfs,calculate all the gap of name's edges
//use queue to simplify the algorithm
void Graph::bellman_ford(string name)
{
    queue<string> name_list;
    name_list.push(name);
    visited[name] = true;
    distances[name] = 0.0;
    while(!name_list.empty())
    {
        string name = name_list.front();
        name_list.pop();
        for(auto begin = weighted_edges[name].begin();
            begin!=weighted_edges[name].end();++begin)
        {
            string nameA = begin->first;
            if(distances.find(nameA) == distances.end())
            {
                distances[nameA] = 99999;
            }
            if((distances[name]+begin->second) < distances[nameA])
            {
                distances[nameA] = distances[name] + begin->second;
                paths[nameA] = name;
                if(visited.find(begin->first) == visited.end())
                {
                    visited[begin->first] = true;
                    name_list.push(begin->first);
                }
            }

        }
        visited[name] = false;
    }
}
