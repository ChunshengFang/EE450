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
#include <map>

#include "serverS.h"

using namespace std;

const string localhost = "127.0.0.1";
const string central_udp_port = "24779";
const string server_port_S = "22779";

//read the scores from the file
map<string,int> read_scores();

int main()
{
    map<string,int> scores = read_scores();
    /*
    for(auto begin = scores.begin();begin != scores.end();++begin)
    {
        cout << begin->first <<  " " << begin->second <<endl;
    }
    */
    int server_fd_S = init_udp_server(server_port_S);
    int central_fd = init_udp_connect(localhost,central_udp_port);
    printf("The ServerS is up and running using UDP on port %s\n",server_port_S.c_str());

    for(;;)
    {
        char read_buf[32768];
        size_t read_bytes = recv(server_fd_S,read_buf,32768,0);
        if(read_bytes < 0)
        {
            fprintf(stderr,"serverS recv error\n");
            continue;
        }
        printf("The ServerS received a request from Central to get the scores.\n");
        string data(read_buf,read_bytes);
        string result = "";
        vector<string> edges = split(data,"!");
        //add the score to the graph's node
        //and send it back
        for(auto begin = edges.begin();begin != edges.end();++begin)
        {
            vector<string> keys = split(*begin,":");
            string key = keys[0];
            result += *begin;
            result += ":";
            result += to_string(scores[key]);
            if(begin != edges.end()-1)
            {
                result+="!";
            }
        }
        size_t send_bytes = send(central_fd,result.c_str(),result.size(),0);
        if(send_bytes < 0)
        {
            fprintf(stderr,"serverS send error\n");
            break;
        }
        printf("The ServerS finished sending the scores to Central.\n");
    }

    close(central_fd);
    close(server_fd_S);
}

//read the scores from the file
map<string,int> read_scores()
{
    map<string,int> ret;
    FILE* fp = fopen("scores.txt","r");
    char buf[64];
    int score = 0;
    while(fscanf(fp,"%s %d",buf,&score)!=EOF)
    {
        string username(buf);
        ret[username] = score;
    }
    fclose(fp);
    return ret;
}
