#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <list>
#include <map>

int init_tcp_server(std::string port);
int init_udp_server(std::string port);
int init_tcp_connect(std::string localhost,std::string port);
int init_udp_connect(std::string localhost,std::string port);
std::vector<std::string> split(std::string s, std::string sp);

class Graph
{
public:
    Graph() = default;
    void add_single_edge(std::string nameA,std::string nameB);
    void add_edges(std::string nameA,std::string nameB);
    void dfs(std::string name);
    std::string graph_string(std::string nameA,std::string nameB);
    void set_scores(std::string name,int score);
    std::string find_shortest_path(std::string nameA,std::string nameB);
    void bellman_ford(std::string name);
    void init_weighted_edges();
    void retrieve_path(std::vector<std::string>& path,
        std::string nameA,std::string nameB);
private:
    std::map<std::string,std::list<std::string> > edges;
    std::map<std::string,std::map<std::string,double>> weighted_edges;
    std::map<std::string,bool> visited;
    std::map<std::string,int> scores;
    std::map<std::string,std::string> paths;
    std::map<std::string,double> distances;
};

#endif
